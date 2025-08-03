#pragma once

#include <facades/utils/jwt/JWT.h>
#include <drogon/drogon.h>
#include <controllers/responses/Response.h>

using namespace drogon;
using namespace facade;
namespace filters {
	class JwtFilter : public HttpFilter<JwtFilter> {
	public:
		JwtFilter() = default;

		virtual void doFilter(const HttpRequestPtr& request, FilterCallback&& fcb, FilterChainCallback&& fccb) override;
	};
}
