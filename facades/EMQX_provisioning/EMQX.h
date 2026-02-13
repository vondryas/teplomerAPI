#pragma once

#include <drogon/HttpClient.h>
#include <drogon/utils/coroutine.h>

#include <string>
#include <vector>

struct EmqxUserCredential
{
    std::string userId;     // device UUID
    std::string password;   // plaintext pro import (type=plain)
    bool isSuperuser{ false };
};

struct EmqxPublishMessage
{
    std::string topic;
    std::string payload;
    int qos{ 0 };
    bool retain{ false };
};

class EmqxProvisioner
{
public:
    // Lehký konstruktor: config se naète jen jednou, ale HttpClient se vytvoøí pokaždé nový
    EmqxProvisioner();

    // --- users / credentials ---
    drogon::Task<bool> importUser(const EmqxUserCredential& user,
        std::string* outError = nullptr);

    drogon::Task<bool> importUsersBatch(const std::vector<EmqxUserCredential>& users,
        std::string* outError = nullptr);

    drogon::Task<bool> importUsers(const std::vector<EmqxUserCredential>& users,
        std::string* outError = nullptr);

    drogon::Task<bool> deleteUser(const std::string& userId,
        std::string* outError = nullptr);

    drogon::Task<bool> updateUser(const EmqxUserCredential& user,
        std::string* outError = nullptr);

    // --- publish ---
    drogon::Task<bool> publish(const EmqxPublishMessage& msg,
        std::string* outError = nullptr);

    drogon::Task<bool> publishBulk(const std::vector<EmqxPublishMessage>& msgs,
        std::string* outError = nullptr);

    // Helper: fireport/device/<uuid>/<suffix>
    drogon::Task<bool> publishToDevices(const std::vector<std::string>& deviceUuids,
        const std::string& topicSuffix,
        const std::string& payload,
        int qos = 0,
        bool retain = false,
        std::string* outError = nullptr);

private:
    struct Config
    {
        std::string clientBaseUrl; // "https://host:8443"
        std::string apiBasePath;   // "/api/v5"
        std::string appId;
        std::string appSecret;
        size_t importBatchSize{ 20 };
        size_t publishBatchSize{ 20 };
    };

    static const Config& config(); // init-once cache
    static std::string makeBasicAuthHeader(const Config& c);

    static void splitBaseUrl(const std::string& apiBase,
        std::string& outClientBase,
        std::string& outBasePath);

    static std::string urlEncodePathSegment(const std::string& s);

    // kompatibilita: sendRequestCoro mùže vracet HttpResponsePtr nebo pair<ReqResult, HttpResponsePtr>
    template <class AwaitRet>
    static void unpackSendRequestResult(AwaitRet&& ret,
        drogon::ReqResult& outResult,
        drogon::HttpResponsePtr& outResp);

private:
    drogon::HttpClientPtr client_; // schválnì per-instance
};
