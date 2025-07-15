#include "FacadeProvider.h"
#include <facades/SensorDataFacade.h>

void facade::FacadeProvider::initAndStart(const Json::Value&)
{
	registerFacade<SensorDataFacade>(std::make_shared<SensorDataFacade>());
}

void facade::FacadeProvider::shutdown()
{
	facades_.clear();
}
