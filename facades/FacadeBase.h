#pragma once

#include <drogon/orm/DbClient.h>
#include <drogon/drogon.h>
#include <drogon/orm/CoroMapper.h>

namespace facade
{
	class FacadeBase
	{
	public:
		FacadeBase() {
			LOG_INFO << "OK";
		}
		virtual ~FacadeBase() = default;
	protected:
		
	};
}