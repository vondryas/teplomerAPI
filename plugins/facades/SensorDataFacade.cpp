#include "SensorDataFacade.h"

void facade::SensorDataFacade::initAndStart(const Json::Value& config)
{
	getDbClient();
}

void facade::SensorDataFacade::shutdown()
{
}
