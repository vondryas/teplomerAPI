#include "SensorDataCallback.h"

DbErrorCallback callbacks::SensorDataCallback::onError(HttpResponseCallback& cb) const
{
	return [cb](const drogon::orm::DrogonDbException& e) {
			LOG_ERROR << "Error fetching SensorData: " << e.base().what();
			responses::wrongRequestResponse(e.base().what(), cb, drogon::k500InternalServerError);
		};
}

SingleRowCallback<drogon_model::teplomer_db::SensorData> callbacks::SensorDataCallback::onSingleRow(HttpResponseCallback& cb) const
{
	return [cb](const drogon_model::teplomer_db::SensorData& data) {
			responses::jsonResponse(data.toJson(), cb);
		};
}

MultipleRowsCallback<drogon_model::teplomer_db::SensorData> callbacks::SensorDataCallback::onMultipleRows(HttpResponseCallback& cb) const
{
	return [cb](const std::vector<drogon_model::teplomer_db::SensorData>& data) {
			responses::jsonResponse(toJson(data), cb);
		};
}

CountCallback callbacks::SensorDataCallback::onCount(HttpResponseCallback& cb) const
{
	return [cb](const size_t changedCols) {
			responses::textResponse(std::to_string(changedCols), cb);
		};
}
