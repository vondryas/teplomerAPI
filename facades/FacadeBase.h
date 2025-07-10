#pragma once

#include <drogon/orm/DbClient.h>
#include <drogon/drogon.h>

namespace facade
{
	class FacadeBase
	{
	public:
		FacadeBase() {
			LOG_INFO << "OK";
			dbClient_ = drogon::app().getDbClient();
		}
		virtual ~FacadeBase() = default;
	protected:
		drogon::orm::DbClientPtr dbClient_;
		
	};
}