#pragma once

#include <facades/utils/jwt/JWT.h>
#include <drogon/drogon.h>
#include <drogon/HttpController.h>
#include <controllers/responses/Response.h>

using namespace drogon;
using namespace facade;

class Auth : public drogon::HttpController<Auth>
{
public:
	METHOD_LIST_BEGIN
		METHOD_ADD(Auth::getToken, "/login", Post, Options);
	METHOD_ADD(Auth::verifyToken, "/verify", Get, Options, "filters::JwtFilter");
	METHOD_LIST_END

		Task<HttpResponsePtr> getToken(const HttpRequestPtr req);
	Task<HttpResponsePtr> verifyToken(const HttpRequestPtr req);
};