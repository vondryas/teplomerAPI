/**
 *
 *  ResourceOwnerFilter.cc
 *
 */

#include "ResourceOwnerFilter.h"
#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Exception.h>
#include <controllers/responses/Response.h>
#include <controllers/exceptionWrapper/TryNumberParsing.h>
#include <any>
#include <stdexcept>
#include <sstream>

using namespace drogon;
using namespace filters;

static constexpr UserRoles ADMIN_ROLE = UserRoles::admin;

ResourceOwnerFilter::ResourceOwnerFilter()
{
	// Load ownership mapping from custom_config.ownership
	const auto& cfg = app().getCustomConfig();
	if (!cfg.isMember("ownership"))
	{
		LOG_WARN << "ResourceOwnerFilter: 'ownership' config not found, filter will deny unknown resources.";
		return;
	}

	const auto& ownership = cfg["ownership"];
	for (const auto& name : ownership.getMemberNames())
	{
		const auto& item = ownership[name];
		if (!item.isMember("baseTable") ||
			!item.isMember("idColumn") ||
			!item.isMember("ownerColumn"))
		{
			LOG_ERROR << "ResourceOwnerFilter: invalid config for resource key '" << name
				<< "' (missing baseTable/idColumn/ownerColumn)";
			continue;
		}

		ResourceConfig rc;
		rc.baseTable = item["baseTable"].asString();
		rc.idColumn = item["idColumn"].asString();
		rc.ownerColumn = item["ownerColumn"].asString();

		if (item.isMember("idInPath"))
		{
			rc.idInPath = item["idInPath"].asBool();
		}

		if (item.isMember("bodyIdPaths") && item["bodyIdPaths"].isArray())
		{
			const auto& pathsArr = item["bodyIdPaths"];
			for (const auto& pathVal : pathsArr)
			{
				if (!pathVal.isArray())
				{
					LOG_ERROR << "ResourceOwnerFilter: bodyIdPaths element for resource key '"
						<< name << "' is not an array";
					continue;
				}

				std::vector<std::string> segments;
				for (const auto& segVal : pathVal)
				{
					if (segVal.isString())
					{
						segments.emplace_back(segVal.asString());
					}
				}

				if (!segments.empty())
				{
					rc.bodyIdPaths.emplace_back(std::move(segments));
				}
			}
		}

		if (item.isMember("joins") && item["joins"].isArray())
		{
			for (const auto& j : item["joins"])
			{
				if (!j.isMember("table") ||
					!j.isMember("localColumn") ||
					!j.isMember("foreignColumn"))
				{
					LOG_ERROR << "ResourceOwnerFilter: invalid join step for resource key '" << name << "'";
					continue;
				}

				JoinStep step;
				step.table = j["table"].asString();
				step.localColumn = j["localColumn"].asString();
				step.foreignColumn = j["foreignColumn"].asString();
				rc.joins.emplace_back(std::move(step));
			}
		}

		ownershipMap_.emplace(name, std::move(rc));
	}
}

std::string ResourceOwnerFilter::getUserIdFromRequest(const HttpRequestPtr& req) const
{
	auto attrs = req->getAttributes();
	if (!attrs || !attrs->find("jwt_id"))
	{
		throw std::runtime_error("JWT id claim (jwt_id) not found");
	}

	const std::string& val = attrs->get<std::string>("jwt_id");

	return val;
}

UserRoles ResourceOwnerFilter::getRoleFromRequest(const HttpRequestPtr& req) const
{
	auto attrs = req->getAttributes();
	if (!attrs || !attrs->find("jwt_role"))
	{
		throw std::runtime_error("JWT role claim (jwt_role) not found");
	}

	auto val = attrs->get<std::int64_t>("jwt_role");

	return static_cast<UserRoles>(val);
}

std::pair<std::string, std::string>
ResourceOwnerFilter::getResourceAndPathId(const HttpRequestPtr& req) const
{
	std::string path = req->path(); // e.g. "/weather/123" or "/device/abc" or "/weather"

	// Strip leading '/'
	while (!path.empty() && path.front() == '/')
		path.erase(path.begin());

	// Strip query string if present (just in case)
	auto qpos = path.find('?');
	if (qpos != std::string::npos)
		path = path.substr(0, qpos);

	// Split into segments
	std::vector<std::string> segments;
	std::stringstream ss(path);
	std::string item;
	while (std::getline(ss, item, '/'))
	{
		if (!item.empty())
			segments.push_back(item);
	}

	if (segments.empty())
	{
		return { "", "" };
	}

	// For your API, paths start at root, e.g. "device", "device/{id}", "weather", "weather/{id}", ...
	// -> resource = first segment, ID candidate = second segment (if present).
	const std::string resource = segments[0];
	const std::string idCandidate = (segments.size() >= 2) ? segments[1] : "";

	return { resource, idCandidate };
}

std::string ResourceOwnerFilter::methodToString(HttpMethod method) const
{
	switch (method)
	{
	case HttpMethod::Get:     return "GET";
	case HttpMethod::Post:    return "POST";
	case HttpMethod::Put:     return "PUT";
	case HttpMethod::Delete:  return "DELETE";
	case HttpMethod::Patch:   return "PATCH";
	default:                  return "OTHER";
	}
}

const ResourceOwnerFilter::ResourceConfig*
ResourceOwnerFilter::getConfig(const std::string& resource, HttpMethod method) const
{
	if (resource.empty())
		return nullptr;

	const std::string methodStr = methodToString(method);
	const std::string keyWithMethod = resource + ":" + methodStr;

	// Prefer method-specific config: "resource:METHOD"
	auto it = ownershipMap_.find(keyWithMethod);
	if (it != ownershipMap_.end())
		return &it->second;

	// Fallback to generic resource config: "resource"
	it = ownershipMap_.find(resource);
	if (it != ownershipMap_.end())
		return &it->second;

	return nullptr;
}

const Json::Value* ResourceOwnerFilter::findJsonPath(const Json::Value& root,
	const std::vector<std::string>& segments) const
{
	const Json::Value* current = &root;
	for (const auto& key : segments)
	{
		if (key.empty())
			continue;

		if (!current->isObject() || !current->isMember(key))
		{
			return nullptr;
		}

		current = &((*current)[key]);
	}
	return current;
}

std::string ResourceOwnerFilter::extractId(const ResourceConfig& cfg,
	const std::string& pathId,
	const HttpRequestPtr& req) const
{
	std::string id;

	// 1) ID z path (pokud povoleno)
	if (cfg.idInPath && !pathId.empty())
	{
		id = pathId;
	}

	// 2) Pokud nemáme ID z path a máme definované cesty v JSONu
	if (id.empty() && !cfg.bodyIdPaths.empty())
	{
		auto json = req->getJsonObject();
		if (!json)
		{
			throw std::runtime_error("JSON body expected but not found");
		}

		const auto& root = *json;

		// Projdeme postupnì všechny cesty, první nalezená nenull hodnota vyhrává
		for (const auto& segments : cfg.bodyIdPaths)
		{
			const Json::Value* v = findJsonPath(root, segments);
			if (v && !v->isNull())
			{
				id = v->asString();
				if (!id.empty())
				{
					break;
				}
			}
		}
	}

	return id;
}


void ResourceOwnerFilter::checkOwnership(const ResourceConfig& cfg,
	const std::string& id,
	const std::string& userId,
	FilterCallback&& fcb,
	FilterChainCallback&& fccb) const
{
	auto client = drogon::app().getFastDbClient();

	// Build SQL: SELECT 1 FROM base t0 [JOIN ...] WHERE t0.idColumn = $1 AND tN.ownerColumn = $2 LIMIT 1;
	std::string sql = "SELECT 1 FROM " + cfg.baseTable + " t0";

	// Build JOIN chain t0 -> t1 -> ... -> tN
	for (size_t i = 0; i < cfg.joins.size(); ++i)
	{
		const auto& step = cfg.joins[i];
		const std::string prevAlias = "t" + std::to_string(i);
		const std::string alias = "t" + std::to_string(i + 1);

		sql += " JOIN " + step.table + " " + alias +
			" ON " + prevAlias + "." + step.localColumn +
			" = " + alias + "." + step.foreignColumn;
	}

	const std::string lastAlias = "t" + std::to_string(cfg.joins.size());
	sql += " WHERE t0." + cfg.idColumn + " = $1 AND " + lastAlias + "." + cfg.ownerColumn + " = $2 LIMIT 1";

	client->execSqlAsync(
		sql,
		[fcb, fccb](const drogon::orm::Result& r) mutable
		{
			if (r.empty())
			{
				return fcb(responses::forbiddenResponse(
					R"({"error":"forbidden","message":"You are not the owner of this resource"})"));
			}

			return fccb();
		},
		[fcb](const orm::DrogonDbException& e) mutable
		{
			LOG_ERROR << "ResourceOwnerFilter DB error: " << e.base().what();
			return fcb(responses::internalServerErrorResponse(
				R"({"error":"internal_server_error","message":"Database error while checking ownership"})"));
		},
		id,
		userId
	);
}

void ResourceOwnerFilter::doFilter(const HttpRequestPtr& req,
	FilterCallback&& fcb,
	FilterChainCallback&& fccb)
{
	try
	{
		// Skip preflight
		if (req->getMethod() == HttpMethod::Options)
		{
			return fccb();
		}

		// Admin is treated as owner -> bypass DB lookup
		UserRoles role = getRoleFromRequest(req);
		if (role == ADMIN_ROLE)
		{
			return fccb();
		}

		const auto userId = getUserIdFromRequest(req);

		auto [resource, pathId] = getResourceAndPathId(req);
		if (resource.empty())
		{
			return fcb(responses::wrongRequestResponse(
				R"({"error":"bad_request","message":"Cannot determine resource from URL"})"));
		}

		const auto* cfg = getConfig(resource, req->getMethod());
		if (!cfg)
		{
			// safe default: deny when resource isn't configured
			return fcb(responses::forbiddenResponse(
				R"({"error":"forbidden","message":"Ownership check not configured for this resource"})"));
		}

		const std::string id = extractId(*cfg, pathId, req);
		if (id.empty())
		{
			return fcb(responses::wrongRequestResponse(
				R"({"error":"bad_request","message":"Could not extract resource identifier"})"));
		}

		// Async DB lookup
		checkOwnership(*cfg, id, userId, std::move(fcb), std::move(fccb));
	}
	catch (const std::exception& ex)
	{
		return fcb(responses::unauthorizedResponse(
			std::string(R"({"error":"unauthorized","message":")") + ex.what() + "\"}"));
	}
}
