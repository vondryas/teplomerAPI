#pragma once
#include "BaseFacade.h"

namespace facade
{
	class SensorDataFacade : public BaseFacade, public drogon::Plugin<SensorDataFacade>
	{
		// Inherited via Plugin
		void initAndStart(const Json::Value& config) override;
		void shutdown() override;


	};
}