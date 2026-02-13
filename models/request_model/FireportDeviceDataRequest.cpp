#include "FireportDeviceDataRequest.h"
#include <models/orm_model/FireportDeviceData.h>

void request_model::FireportDeviceDataRequest::mapToOrmModel(model_interface::IModel& ormModel) const
{
	LOG_INFO << "Mapping request model to ORM model";
	auto& sensorDataOrmModel = dynamic_cast<FireportDeviceData&>(ormModel);
	if (id.has_value())
		sensorDataOrmModel.setId(uuids::to_string(*id));
	if (!deviceId.empty())
		sensorDataOrmModel.setDeviceId(deviceId);
	if (!password.empty())
		sensorDataOrmModel.setPassword(password);
}