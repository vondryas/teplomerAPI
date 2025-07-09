#include "FacadeProvider.h"
#include <facades/SensorDataFacade.h>

void facade::FacadeProvider::initAndStart(const Json::Value&)
{
	registerFacade<facade::SensorDataFacade>(std::make_shared<facade::SensorDataFacade>());
}

void facade::FacadeProvider::shutdown()
{
	facades_.clear();
}
