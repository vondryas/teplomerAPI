#pragma once

#include <drogon/HttpController.h>
#include <models/model_utils/ModelUtils.h>
#include <controllers/callbacks/ICallbackBase.h>

using HttpResponseCallback = std::function<void(const drogon::HttpResponsePtr&)>;

namespace responses
{
	void wrongRequestResponse(const std::string& reason,
		const HttpResponseCallback& callback,
		const drogon::HttpStatusCode statusCode = drogon::k400BadRequest);

	void jsonResponse(const Json::Value& json,
		const HttpResponseCallback& callback,
		const drogon::HttpStatusCode statusCode = drogon::k200OK);

	void textResponse(const std::string& text,
		const HttpResponseCallback& callback,
		const drogon::HttpStatusCode statusCode = drogon::k200OK);
}