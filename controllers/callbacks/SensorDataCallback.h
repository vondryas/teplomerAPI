#pragma once

#include "ICallbackBase.h"
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <models/SensorData.h>
#include <controllers/responses/Response.h>

namespace callbacks
{
	class SensorDataCallback : public ICallbackBase
	{
	public:
		~SensorDataCallback() = default;
		SensorDataCallback() = default;
	public:
		// Inherited via ICallbackBase
		DbErrorCallback onError(HttpResponseCallback& cb) const override;

		// Implementation of SingleRowCallback for SensorData
		SingleRowCallback<drogon_model::teplomer_db::SensorData> onSingleRow(HttpResponseCallback& cb) const;

		// Implementation of MultipleRowsCallback for SensorData
		MultipleRowsCallback<drogon_model::teplomer_db::SensorData> onMultipleRows(HttpResponseCallback& cb) const;

		// Inherited via ICallbackBase
		CountCallback onCount(HttpResponseCallback& cb) const override;

	};
} // namespace callbacks