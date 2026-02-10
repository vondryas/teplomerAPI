#pragma once

#include <drogon/HttpRequest.h>
#include <enums/UserRoles.h>

using namespace drogon;

inline UserRoles getRoleFromRequest(const HttpRequestPtr& req)
{
	auto attrs = req->getAttributes();
	if (!attrs || !attrs->find("jwt_role"))
	{
		throw std::runtime_error("JWT role claim (jwt_role) not found");
	}

	auto val = attrs->get<std::int64_t>("jwt_role");

	return static_cast<UserRoles>(val);
}

inline std::string getUserIdFromRequest(const HttpRequestPtr& req)
{
	auto attrs = req->getAttributes();
	if (!attrs || !attrs->find("jwt_id"))
	{
		throw std::runtime_error("JWT id claim (jwt_id) not found");
	}

	const std::string& val = attrs->get<std::string>("jwt_id");

	return val;
}