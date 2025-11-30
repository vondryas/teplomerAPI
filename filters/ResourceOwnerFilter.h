/**
 *
 *  ResourceOwnerFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <enums/UserRoles.h>

using namespace drogon;

/**
	 * Generic ownership filter.
	 *
	 * Config examples (in custom_config.ownership):
	 *
	 * "device": {
	 *   "baseTable": "devices",
	 *   "idColumn": "name",
	 *   "ownerColumn": "user_id",
	 *   "idInPath": true
	 * },
	 * "weather": {
	 *   "baseTable": "weather_station_data",
	 *   "idColumn": "id",
	 *   "ownerColumn": "user_id",
	 *   "idInPath": true,
	 *   "joins": [
	 *     { "table":"devices", "localColumn":"device_id", "foreignColumn":"name" }
	 *   ]
	 * },
	 * "weather:PUT": {
	 *   "baseTable": "weather_station_data",
	 *   "idColumn": "id",
	 *   "ownerColumn": "user_id",
	 *   "idInPath": false,
	 *   "bodyIdField": "id",
	 *   "joins": [
	 *     { "table":"devices", "localColumn":"device_id", "foreignColumn":"name" }
	 *   ]
	 * },
	 * "weather:POST": {
	 *   "baseTable": "devices",
	 *   "idColumn": "name",
	 *   "ownerColumn": "user_id",
	 *   "idInPath": false,
	 *   "bodyIdField": "deviceId"
	 * }
	 *
	 * Admin (UserRoles::admin) is treated as owner (bypasses DB lookup).
	 */

namespace filters {
	class ResourceOwnerFilter : public HttpFilter<ResourceOwnerFilter>
	{
	public:
		ResourceOwnerFilter();
		void doFilter(const HttpRequestPtr& req,
			FilterCallback&& fcb,
			FilterChainCallback&& fccb) override;
	private:
		struct JoinStep
		{
			std::string table;        // join target table
			std::string localColumn;  // column in previous table alias (t{i-1})
			std::string foreignColumn;// column in this join table alias (t{i})
		};

		struct ResourceConfig
		{
			std::string baseTable;            // starting table (t0)
			std::string idColumn;             // column on t0 used for the ID
			std::string ownerColumn;          // column on last table (tN) that stores user id
			std::vector<JoinStep> joins;      // join chain t0 -> t1 -> ... -> tN

			bool idInPath{ true };              // if true, try to read ID from URL path
			// Seznam možných JSON cest k ID:
			// napø. [ ["deviceId"], ["device_id"], ["end_device_ids","device_id"] ]
			std::vector<std::vector<std::string>> bodyIdPaths;
		};

		// key is either "resource" or "resource:METHOD" (METHOD = GET/POST/PUT/DELETE)
		std::unordered_map<std::string, ResourceConfig> ownershipMap_;

		/// Get userId from JWT attribute "jwt_id".
		std::string getUserIdFromRequest(const drogon::HttpRequestPtr& req) const;

		/// Get user role from JWT attribute "jwt_role".
		UserRoles getRoleFromRequest(const drogon::HttpRequestPtr& req) const;

		/// From URL path, return resource name and path ID candidate.
		/// For paths like "/device/abc" or "/weather/123":
		///   segments = ["device","abc"], resource="device", idCandidate="abc"
		/// For "/weather" -> segments=["weather"], resource="weather", idCandidate=""
		std::pair<std::string, std::string> getResourceAndPathId(const drogon::HttpRequestPtr& req) const;

		/// Get configuration for given resource + method (first tries "resource:METHOD", then "resource").
		const ResourceConfig* getConfig(const std::string& resource, drogon::HttpMethod method) const;

		/// Try to extract ID value according to config (from path and/or JSON body).
		std::string extractId(const ResourceConfig& cfg,
			const std::string& pathId,
			const drogon::HttpRequestPtr& req) const;

		/// Find JSON value by path segments like ["end_device_ids","device_id"].
		const Json::Value* findJsonPath(const Json::Value& root,
			const std::vector<std::string>& segments) const;

		/// Convert HTTP method to string used in config key ("GET","POST","PUT","DELETE",...).
		std::string methodToString(drogon::HttpMethod method) const;

		/// Perform asynchronous ownership check.
		void checkOwnership(const ResourceConfig& cfg,
			const std::string& id,
			const std::string& userId,
			drogon::FilterCallback&& fcb,
			drogon::FilterChainCallback&& fccb) const;
	};
}

