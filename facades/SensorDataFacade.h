#pragma once
#include "FacadeBase.h"


namespace facade
{
	class SensorDataFacade : public FacadeBase
	{
	public:
		SensorDataFacade() = default;

		void hello();
	};
}