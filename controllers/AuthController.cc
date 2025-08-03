#include "AuthController.h"



Task<HttpResponsePtr> Auth::getToken(const HttpRequestPtr request) {
    Json::Value responseJson = *request->getJsonObject();
    Json::Value resultJson;

    // Verify if there's a missing values on body of request
    if (!responseJson.isMember("email") || !responseJson.isMember("password")) {
        resultJson["error"] = "Missing email or password.";
        resultJson["status"] = 0;

        co_return responses::wrongRequestResponse("Id parameter is required");;
    }

    JWT jwtGenerated = JWT::generateToken({
        { "email", picojson::value(responseJson["email"].asString()) },
        }, responseJson.isMember("remember") && responseJson["remember"].asBool());
    std::int64_t jwtExpiration = jwtGenerated.getExpiration();

    resultJson["token"] = jwtGenerated.getToken();
    resultJson["expiresIn"] = jwtExpiration - std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    resultJson["expiresAt"] = jwtExpiration;
    
    co_return responses::jsonOkResponse(resultJson);
}

Task<HttpResponsePtr> Auth::verifyToken(const HttpRequestPtr request) {
    Json::Value resultJson;

    resultJson["aud"] = request->getAttributes()->get<std::string>("jwt_aud");
    resultJson["exp"] = request->getAttributes()->get<std::int64_t>("jwt_exp");
    resultJson["iat"] = request->getAttributes()->get<std::int64_t>("jwt_iat");
    resultJson["iss"] = request->getAttributes()->get<std::string>("jwt_iss");
    resultJson["nbf"] = request->getAttributes()->get<std::int64_t>("jwt_nbf");
    resultJson["email"] = request->getAttributes()->get<std::string>("jwt_email");
    resultJson["jwt_debugger"] = "https://jwt.io/#debugger-io?token=" + request->getHeader("Authorization").substr(7);

    co_return responses::jsonOkResponse(resultJson);
}