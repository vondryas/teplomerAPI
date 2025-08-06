#include <models/orm_model/Devices.h>
#include "UsersRequest.h"


void request_model::UsersRequest::mapToOrmModel(model_interface::IModel& ormModel) const
{
	LOG_INFO << "Mapping request model to ORM model";
	auto& sensorDataOrmModel = dynamic_cast<Users&>(ormModel);
	if (id.has_value())
		sensorDataOrmModel.setId(uuids::to_string(*id));
	if (!username.empty())
		sensorDataOrmModel.setUsername(username);
	if (!email.empty())
		sensorDataOrmModel.setEmail(email);
	if (password.empty())
		sensorDataOrmModel.setPasshash(password);
	if (role.has_value())
		sensorDataOrmModel.setRole(*role);
}