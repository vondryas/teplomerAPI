/**
 *
 *  AdminFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include "../enums/UserRoles.h"


using namespace drogon;

namespace filters {
	class AdminFilter : public HttpFilter<AdminFilter>
	{
	public:
		AdminFilter() {}
		void doFilter(const HttpRequestPtr& req,
			FilterCallback&& fcb,
			FilterChainCallback&& fccb) override;
	private:
		/// Získá UserRoles z requestu (z atributu "jwt_role").
		/// Mùže být string ("user"/"admin") nebo integer (0/1).
		UserRoles getRoleFromRequest(const drogon::HttpRequestPtr& req) const;
	};
}
