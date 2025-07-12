#include "FacadeProvider.h"
#include <facades/SensorDataFacade.h>

void facade::FacadeProvider::initAndStart(const Json::Value&)
{
}

void facade::FacadeProvider::shutdown()
{
	facades_.clear();
}
