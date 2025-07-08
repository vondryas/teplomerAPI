#include "SensorDataCallback.h"

callbacks::ICallbackBase::DbErrorCallback callbacks::SensorDataCallback::onError(HttpResponseCallback& cb) const
{
	return [cb](const drogon::orm::DrogonDbException& e) {
			LOG_ERROR << "Error fetching SensorData: " << e.base().what();
			auto resp = drogon::HttpResponse::newHttpResponse();
			resp->setStatusCode(drogon::k500InternalServerError);
			resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
			resp->setBody(e.base().what());
			cb(resp);
		};
}

callbacks::ICallbackBase::SingleRowCallback<drogon_model::teplomer_db::SensorData> callbacks::SensorDataCallback::onSingleRow(HttpResponseCallback& cb) const
{
	return SingleRowCallback<drogon_model::teplomer_db::SensorData>();
}

callbacks::ICallbackBase::MultipleRowsCallback<drogon_model::teplomer_db::SensorData> callbacks::SensorDataCallback::onMultipleRows(HttpResponseCallback& cb) const
{
	return [cb](const std::vector<drogon_model::teplomer_db::SensorData>& data) {
			auto resp = drogon::HttpResponse::newHttpJsonResponse(toJson(data));
			resp->setStatusCode(drogon::k200OK);
			resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
			resp->setBody(jsonToString(&data));
			cb(resp);
		};
}

callbacks::ICallbackBase::CountCallback callbacks::SensorDataCallback::onCount(HttpResponseCallback& cb) const
{
	return CountCallback();
}
