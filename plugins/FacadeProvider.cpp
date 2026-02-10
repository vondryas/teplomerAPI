#include "FacadeProvider.h"
#include <facades/WeatherStationDataFacade.h>
#include <facades/UsersFacade.h>
#include <facades/DevicesFacade.h>
#include <facades/FireportDeviceDataFacade.h>

void facade::FacadeProvider::initAndStart(const Json::Value&)
{
	registerFacade<WeatherStationDataFacade>(std::make_shared<WeatherStationDataFacade>());
	registerFacade<UsersFacade>(std::make_shared<UsersFacade>());
	registerFacade<DevicesFacade>(std::make_shared<DevicesFacade>());
	registerFacade<FireportDeviceDataFacade>(std::make_shared<FireportDeviceDataFacade>());
}

void facade::FacadeProvider::shutdown()
{
	facades_.clear();
}
