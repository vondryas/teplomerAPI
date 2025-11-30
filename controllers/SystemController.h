#pragma once

#include <drogon/HttpSimpleController.h>
#include <drogon/HttpController.h>
#include <controllers/responses/Response.h>

using namespace drogon;

class SystemController : public drogon::HttpController<SystemController>
{
public:
    METHOD_LIST_BEGIN
        // Health check
        ADD_METHOD_TO(SystemController::health,
            "/health",
            drogon::Get,
            drogon::Options);

    // Vrácení OpenAPI specifikace
    ADD_METHOD_TO(SystemController::openapi,
        "/openapi.json",
        drogon::Get,
        drogon::Options);
    // Vrácení OpenAPI specifikace
    ADD_METHOD_TO(SystemController::openapi,
        "/openapi",
        drogon::Get,
        drogon::Options);
    METHOD_LIST_END

    Task<HttpResponsePtr> health(const HttpRequestPtr req);
    Task<HttpResponsePtr> openapi(const HttpRequestPtr req);
};