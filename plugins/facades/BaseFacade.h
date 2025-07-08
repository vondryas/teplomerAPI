/**
 *
 *  facade_BaseFacade.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include <memory>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>
#include <drogon/orm/Field.h>
#include <drogon/orm/SqlBinder.h>
#include <drogon/orm/Mapper.h>
#include <drogon/orm/BaseBuilder.h>
#include <trantor/utils/Date.h>
#include <trantor/utils/Logger.h>
#include <drogon/drogon.h>

namespace facade
{

	class BaseFacade
	{
	protected:
		BaseFacade() = default;

		void getDbClient();

		/// This method must be called by drogon to shutdown the plugin.
		/// It must be implemented by the user.
		void shutdown() override;
	protected:
		drogon::orm::DbClientPtr dbClient;
	};

}
