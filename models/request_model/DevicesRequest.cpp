#include "DevicesRequest.h"
#include <models/orm_model/Devices.h>

void request_model::DevicesRequest::mapToOrmModel(model_interface::IModel& ormModel) const
{
	LOG_INFO << "Mapping request model to ORM model";
	auto& sensorDataOrmModel = dynamic_cast<Devices&>(ormModel);
	if (!name.empty())
		sensorDataOrmModel.setName(name);
	if (type.has_value())
		sensorDataOrmModel.setType(*type);
	if (userId.has_value())
		sensorDataOrmModel.setUserId(uuids::to_string(*userId));
}