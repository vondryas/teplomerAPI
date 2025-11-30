#include "AdminFilter.h"
#include <any>
#include <stdexcept>
#include <controllers/responses/Response.h>

using namespace drogon;
using namespace filters;

static constexpr UserRoles REQUIRED_ROLE = UserRoles::admin;

UserRoles filters::AdminFilter::getRoleFromRequest(const HttpRequestPtr& req) const
{
	auto attrs = req->getAttributes();
	if (!attrs || !attrs->find("jwt_role"))
	{
		throw std::runtime_error("JWT role claim (jwt_role) not found");
	}

	const auto& val = attrs->get<std::int64_t>("jwt_role");
	auto r = static_cast<UserRoles>(val);
	return r;
}

void filters::AdminFilter::doFilter(const HttpRequestPtr& req,
	FilterCallback&& fcb,
	FilterChainCallback&& fccb)
{
	// OPTIONS neøešíme (CORS preflight) – stejnì jako v JwtFilteru
	if (req->getMethod() == HttpMethod::Options)
	{
		return fccb();
		
	}

	try
	{
		auto role = getRoleFromRequest(req);

		// Kontrola "dostateèné" role: user(0) < admin(1)
		if (static_cast<int>(role) < static_cast<int>(REQUIRED_ROLE))
		{
			// -> role existuje, ale je nedostateèná -> 403 Forbidden
			return fcb(responses::forbiddenResponse(R"({"error":"forbidden","message":"Insufficient permissions"})"));
			
		}

		// OK, role je dostateèná
		return fccb();
	}
	catch (const std::exception& ex)
	{
		return fcb(responses::unauthorizedResponse(std::string(R"({"error":"unauthorized","message":")") +
			ex.what() + "\"}"));
	}
}
