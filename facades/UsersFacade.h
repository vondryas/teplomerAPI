#pragma once

#include "FacadeBase.h"
#include <models/orm_model/Users.h>
#include <models/request_model/UsersRequest.h>
#include <models/list_model/UsersList.h>
#include <functional>
#include <drogon/orm/Mapper.h>
#include <drogon/orm/CoroMapper.h>
#include <models/model_utils/mvector.h>


using namespace drogon;
namespace facade
{
	class UsersFacade : public FacadeBase
	{
		using Users = drogon_model::defaultdb::Users;
		using UsersList = list_model::UsersList;
		using UsersRequest = request_model::UsersRequest;
	public:
		UsersFacade() = default;
		/*
		Task<Users> getById(const std::string& id) const; // Use drogon::Task
		Task<UsersList> getAll() const;
		Task<UsersList> getPaginated(const size_t page, const size_t limit) const;
		Task<Users> create(const UsersRequest& data) const;
		Task<size_t> update(const UsersRequest& data) const;
		Task<size_t> deleteById(const std::string& id) const;
		*/
	};
}