
#include "UsersFacade.h"

using UsersList = list_model::UsersList;
using Users = drogon_model::defaultdb::Users;

constexpr char asciitolower(char in) {
	constexpr char diff = 'Z' - 'z';
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}

Task<auth_model::AuthModel> facade::UsersFacade::create(UsersRequest& user) const
{
	AuthModel authModel;
	auto mapper = drogon::orm::CoroMapper<Users>(drogon::app().getFastDbClient());
	Users ormUser;
	if (!user.role.has_value())
	{
		user.role = std::make_optional((int)UserRoles::user); // Default role if not provided
	}
	// Normalize email to lowercase
	std::transform(user.email.begin(), user.email.end(), user.email.begin(), asciitolower);
	user.password = drassist::passwdhash::hash(user.password); // Hash the password
	user.mapToOrmModel(ormUser);

	auto storedUser = co_await mapper.insert(ormUser); // Insert user into the database
	LOG_DEBUG << "User created with ID: " << storedUser.toJson().toStyledString();

	if (storedUser.getId() == nullptr || storedUser.getRole() == nullptr)
	{
		throw std::runtime_error("Failed to create user: ID is null");
	}
	auto uuidId = uuids::uuid::from_string(storedUser.getValueOfId());

	JWT jwtGenerated = JWT::generateToken({
		{ "email", picojson::value(user.email) },
		{ "id", picojson::value(storedUser.getValueOfId()) },
		{ "role", picojson::value((int64_t)storedUser.getValueOfRole()) }
		}, user.remember.has_value() ? user.remember.value() : false);

	authModel.userId = uuidId.has_value() ? uuidId.value() : uuids::uuid();
	authModel.email = user.email;
	authModel.username = user.username;
	authModel.token = jwtGenerated.getToken();

	co_return authModel;
}

Task<auth_model::AuthModel> facade::UsersFacade::login(UsersRequest& user) const
{
	AuthModel authModel;
	auto mapper = drogon::orm::CoroMapper<Users>(drogon::app().getFastDbClient());
	std::string criteriaCol;
	std::string criteriaValue;
	if (!user.email.empty())
	{
		criteriaCol = Users::Cols::_email;
		std::transform(user.email.begin(), user.email.end(), user.email.begin(), asciitolower);
		criteriaValue = user.email;
	}
	else if (!user.username.empty())
	{
		criteriaCol = Users::Cols::_username;
		criteriaValue = user.username;
	}
	auto storedUser = co_await mapper.findOne(drogon::orm::Criteria(criteriaCol, drogon::orm::CompareOperator::EQ, criteriaValue));
	bool isPasswordValid = drassist::passwdhash::verify(user.password, storedUser.getValueOfPasshash());
	if (!isPasswordValid)
	{
		LOG_ERROR << "Invalid password for user: " << storedUser.getValueOfUsername();
		throw std::runtime_error("Invalid password");
	}

	JWT jwtGenerated = JWT::generateToken({
		{ "email", picojson::value(user.email) },
		{ "id", picojson::value(storedUser.getValueOfId()) },
		{ "role", picojson::value((int64_t)storedUser.getValueOfRole()) }
		}, user.remember.has_value() ? user.remember.value() : false);

	authModel.userId = uuids::uuid::from_string(storedUser.getValueOfId()).value_or(uuids::uuid());
	authModel.email = storedUser.getValueOfEmail();
	authModel.username = storedUser.getValueOfUsername();
	authModel.token = jwtGenerated.getToken();

	// Normalize email to lowercase
	co_return authModel;
}

Task<size_t> facade::UsersFacade::deleteById(const std::string& id) const
{
	auto mapper = drogon::orm::CoroMapper<Users>(drogon::app().getFastDbClient());
	co_return co_await mapper.deleteByPrimaryKey(id);
}

Task<UsersList> facade::UsersFacade::getAll() const
{
	auto mapper = drogon::orm::CoroMapper<Users>(drogon::app().getFastDbClient());
	auto data = co_await mapper.findAll();
	UsersList usersList;
	usersList.setData(mvector<Users>(data));
	usersList.setTotalCount(data.size());
	usersList.setLimit(data.size());
	co_return usersList;
}

Task<Users> facade::UsersFacade::getById(const std::string& id) const
{
	auto mapper = drogon::orm::CoroMapper<Users>(drogon::app().getFastDbClient());
	co_return co_await mapper.findByPrimaryKey(id);
}