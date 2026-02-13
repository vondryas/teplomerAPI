#include "EMQX.h"

#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>
#include <json/json.h>

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>

using namespace drogon;

namespace {

    static std::string ensureLeadingSlash(std::string s)
    {
        if (s.empty()) return s;
        if (s.front() != '/') s.insert(s.begin(), '/');
        while (s.size() > 1 && s.back() == '/') s.pop_back();
        return s;
    }

    template <class T>
    struct is_pair : std::false_type {};
    template <class A, class B>
    struct is_pair<std::pair<A, B>> : std::true_type {};

} // namespace

// ---- config cache (read once) ----
const EmqxProvisioner::Config& EmqxProvisioner::config()
{
    static Config c;
    static std::once_flag once;

    std::call_once(once, []() {
        const Json::Value& cfg = app().getCustomConfig();
        const Json::Value& emqx = cfg["emqx"];

        const std::string apiBase = emqx.get("api_base", "").asString();
        c.appId = emqx.get("app_id", "").asString();
        c.appSecret = emqx.get("app_secret", "").asString();
        c.importBatchSize = static_cast<size_t>(emqx.get("import_batch_size", 200).asUInt());
        c.publishBatchSize = static_cast<size_t>(emqx.get("publish_batch_size", 200).asUInt());

        if (apiBase.empty() || c.appId.empty() || c.appSecret.empty())
        {
            throw std::runtime_error(
                "Missing EMQX config. Expected custom_config.emqx: api_base, app_id, app_secret.");
        }

        splitBaseUrl(apiBase, c.clientBaseUrl, c.apiBasePath);

        if (c.apiBasePath.empty())
            c.apiBasePath = "/api/v5";
        else
            c.apiBasePath = ensureLeadingSlash(c.apiBasePath);
        });

    return c;
}

void EmqxProvisioner::splitBaseUrl(const std::string& apiBase,
    std::string& outClientBase,
    std::string& outBasePath)
{
    auto schemePos = apiBase.find("://");
    if (schemePos == std::string::npos)
    {
        outClientBase = apiBase;
        outBasePath.clear();
        return;
    }

    auto firstSlashAfterHost = apiBase.find('/', schemePos + 3);
    if (firstSlashAfterHost == std::string::npos)
    {
        outClientBase = apiBase;
        outBasePath.clear();
        return;
    }

    outClientBase = apiBase.substr(0, firstSlashAfterHost);
    outBasePath = apiBase.substr(firstSlashAfterHost);
    outBasePath = ensureLeadingSlash(outBasePath);
}

std::string EmqxProvisioner::makeBasicAuthHeader(const Config& c)
{
    const std::string basic = c.appId + ":" + c.appSecret;
    return "Basic " + drogon::utils::base64Encode(basic);
}

std::string EmqxProvisioner::urlEncodePathSegment(const std::string& str)
{
    std::ostringstream oss;
    oss << std::hex << std::uppercase;

    for (unsigned char ch : str)
    {
        const bool safe =
            (std::isalnum(ch) != 0) || ch == '-' || ch == '_' || ch == '.' || ch == '~';
        if (safe)
        {
            oss << ch;
        }
        else
        {
            oss << '%' << std::setw(2) << std::setfill('0') << (int)ch;
        }
    }
    return oss.str();
}

template <class AwaitRet>
void EmqxProvisioner::unpackSendRequestResult(AwaitRet&& ret,
    ReqResult& outResult,
    HttpResponsePtr& outResp)
{
    using R = std::decay_t<AwaitRet>;
    if constexpr (is_pair<R>::value)
    {
        outResult = ret.first;
        outResp = ret.second;
    }
    else
    {
        outResp = ret; // varianta: jen HttpResponsePtr
        outResult = outResp ? ReqResult::Ok : ReqResult::BadResponse;
    }
}

// ---- ctor: create NEW client every time (as requested) ----
EmqxProvisioner::EmqxProvisioner()
{
    const auto& c = config();
    client_ = HttpClient::newHttpClient(c.clientBaseUrl);
}

// ---------------- users / credentials ----------------

Task<bool> EmqxProvisioner::importUser(const EmqxUserCredential& user, std::string* outError)
{
    std::vector<EmqxUserCredential> v{ user };
    co_return co_await importUsersBatch(v, outError);
}

Task<bool> EmqxProvisioner::importUsersBatch(const std::vector<EmqxUserCredential>& users,
    std::string* outError)
{
    if (users.empty()) co_return true;

    const auto& c = config();
    const std::string path =
        c.apiBasePath + "/authentication/password_based:built_in_database/import_users?type=plain";

    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Post);
    req->setPath(path);
    req->setContentTypeCode(CT_APPLICATION_JSON);
    req->addHeader("Authorization", makeBasicAuthHeader(c));

    Json::Value arr(Json::arrayValue);
    arr.resize(static_cast<Json::ArrayIndex>(users.size()));
    for (Json::ArrayIndex i = 0; i < arr.size(); ++i)
    {
        Json::Value item;
        item["user_id"] = users[i].userId;
        item["password"] = users[i].password;
        item["is_superuser"] = users[i].isSuperuser;
        arr[i] = std::move(item);
    }

    Json::StreamWriterBuilder w;
    w["indentation"] = "";
    req->setBody(Json::writeString(w, arr));

    auto ret = co_await client_->sendRequestCoro(req);
    ReqResult rr{ ReqResult::Ok };
    HttpResponsePtr resp;
    unpackSendRequestResult(ret, rr, resp);

    if (rr != ReqResult::Ok || !resp)
    {
        if (outError) *outError = "EMQX import request failed (ReqResult=" + std::to_string((int)rr) + ")";
        co_return false;
    }

    const int status = resp->getStatusCode();
    if (status < 200 || status >= 300)
    {
        if (outError) *outError = "EMQX import failed HTTP " + std::to_string(status) + " body=" + std::string(resp->getBody());
        co_return false;
    }

    co_return true;
}

Task<bool> EmqxProvisioner::importUsers(const std::vector<EmqxUserCredential>& users,
    std::string* outError)
{
    if (users.empty()) co_return true;

    const auto& c = config();
    bool allOk = true;

    for (size_t i = 0; i < users.size(); i += c.importBatchSize)
    {
        size_t end = std::min(users.size(), i + c.importBatchSize);
        std::vector<EmqxUserCredential> batch(users.begin() + i, users.begin() + end);

        std::string err;
        bool ok = co_await importUsersBatch(batch, outError ? &err : nullptr);
        if (!ok)
        {
            allOk = false;
            if (outError && outError->empty()) *outError = err;
        }
    }

    co_return allOk;
}

Task<bool> EmqxProvisioner::deleteUser(const std::string& userId, std::string* outError)
{
    const auto& c = config();
    const std::string encoded = urlEncodePathSegment(userId);

    const std::string path =
        c.apiBasePath + "/authentication/password_based:built_in_database/users/" + encoded;

    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Delete);
    req->setPath(path);
    req->addHeader("Authorization", makeBasicAuthHeader(c));

    auto ret = co_await client_->sendRequestCoro(req);
    ReqResult rr{ ReqResult::Ok };
    HttpResponsePtr resp;
    unpackSendRequestResult(ret, rr, resp);

    if (rr != ReqResult::Ok || !resp)
    {
        if (outError) *outError = "EMQX delete request failed (ReqResult=" + std::to_string((int)rr) + ")";
        co_return false;
    }

    const int status = resp->getStatusCode();
    if (status < 200 || status >= 300)
    {
        if (outError) *outError = "EMQX delete failed HTTP " + std::to_string(status) + " body=" + std::string(resp->getBody());
        co_return false;
    }

    co_return true;
}

drogon::Task<bool> EmqxProvisioner::updateUser(const EmqxUserCredential& user,
    std::string* outError)
{
    const auto& c = config();
    const std::string encoded = urlEncodePathSegment(user.userId);

    // PUT /authentication/password_based:built_in_database/users/{user_id}
    const std::string path =
        c.apiBasePath + "/authentication/password_based:built_in_database/users/" + encoded;

    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Put);
    req->setPath(path);
    req->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    req->addHeader("Authorization", makeBasicAuthHeader(c));

    Json::Value body;
    body["password"] = user.password;
    body["is_superuser"] = user.isSuperuser;

    Json::StreamWriterBuilder w;
    w["indentation"] = "";
    req->setBody(Json::writeString(w, body));

    auto ret = co_await client_->sendRequestCoro(req);
    drogon::ReqResult rr{ drogon::ReqResult::Ok };
    drogon::HttpResponsePtr resp;
    unpackSendRequestResult(ret, rr, resp);

    if (rr != drogon::ReqResult::Ok || !resp)
    {
        if (outError)
            *outError = "EMQX update user request failed (ReqResult=" + std::to_string((int)rr) + ")";
        co_return false;
    }

    const int status = resp->getStatusCode();
    if (status < 200 || status >= 300)
    {
        if (outError)
            *outError = "EMQX update user failed HTTP " + std::to_string(status) +
            " body=" + std::string(resp->getBody());
        co_return false;
    }

    co_return true;
}

// ---------------- publish ----------------

Task<bool> EmqxProvisioner::publish(const EmqxPublishMessage& msg, std::string* outError)
{
    const auto& c = config();
    const std::string path = c.apiBasePath + "/publish";

    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Post);
    req->setPath(path);
    req->setContentTypeCode(CT_APPLICATION_JSON);
    req->addHeader("Authorization", makeBasicAuthHeader(c));

    Json::Value body;
    body["topic"] = msg.topic;
    body["payload"] = msg.payload;
    body["qos"] = msg.qos;
    body["retain"] = msg.retain;

    Json::StreamWriterBuilder w;
    w["indentation"] = "";
    req->setBody(Json::writeString(w, body));

    auto ret = co_await client_->sendRequestCoro(req);
    ReqResult rr{ ReqResult::Ok };
    HttpResponsePtr resp;
    unpackSendRequestResult(ret, rr, resp);

    if (rr != ReqResult::Ok || !resp)
    {
        if (outError) *outError = "EMQX publish request failed (ReqResult=" + std::to_string((int)rr) + ")";
        co_return false;
    }

    const int status = resp->getStatusCode();
    if (status < 200 || status >= 300)
    {
        if (outError) *outError = "EMQX publish failed HTTP " + std::to_string(status) + " body=" + std::string(resp->getBody());
        co_return false;
    }

    co_return true;
}

Task<bool> EmqxProvisioner::publishBulk(const std::vector<EmqxPublishMessage>& msgs, std::string* outError)
{
    if (msgs.empty()) co_return true;

    const auto& c = config();
    const std::string path = c.apiBasePath + "/publish/bulk";

    auto doBatch = [&](const std::vector<EmqxPublishMessage>& batch) -> Task<bool> {
        auto req = HttpRequest::newHttpRequest();
        req->setMethod(Post);
        req->setPath(path);
        req->setContentTypeCode(CT_APPLICATION_JSON);
        req->addHeader("Authorization", makeBasicAuthHeader(c));

        Json::Value arr(Json::arrayValue);
        arr.resize(static_cast<Json::ArrayIndex>(batch.size()));

        for (Json::ArrayIndex i = 0; i < arr.size(); ++i)
        {
            Json::Value item;
            item["topic"] = batch[i].topic;
            item["payload"] = batch[i].payload;
            item["qos"] = batch[i].qos;
            item["retain"] = batch[i].retain;
            arr[i] = std::move(item);
        }

        Json::StreamWriterBuilder w;
        w["indentation"] = "";
        req->setBody(Json::writeString(w, arr));

        auto ret = co_await client_->sendRequestCoro(req);
        ReqResult rr{ ReqResult::Ok };
        HttpResponsePtr resp;
        unpackSendRequestResult(ret, rr, resp);

        if (rr != ReqResult::Ok || !resp)
        {
            if (outError) *outError = "EMQX publish/bulk request failed (ReqResult=" + std::to_string((int)rr) + ")";
            co_return false;
        }

        const int status = resp->getStatusCode();
        if (status < 200 || status >= 300)
        {
            if (outError) *outError = "EMQX publish/bulk failed HTTP " + std::to_string(status) + " body=" + std::string(resp->getBody());
            co_return false;
        }

        co_return true;
        };

    bool allOk = true;

    for (size_t i = 0; i < msgs.size(); i += c.publishBatchSize)
    {
        size_t end = std::min(msgs.size(), i + c.publishBatchSize);
        std::vector<EmqxPublishMessage> batch(msgs.begin() + i, msgs.begin() + end);

        bool ok = co_await doBatch(batch);
        if (!ok) allOk = false;
    }

    co_return allOk;
}

Task<bool> EmqxProvisioner::publishToDevices(const std::vector<std::string>& deviceUuids,
    const std::string& topicSuffix,
    const std::string& payload,
    int qos,
    bool retain,
    std::string* outError)
{
    std::string suffix = topicSuffix;
    if (!suffix.empty() && suffix.front() == '/')
        suffix.erase(suffix.begin());

    std::vector<EmqxPublishMessage> msgs;
    msgs.reserve(deviceUuids.size());

    for (const auto& uuid : deviceUuids)
    {
        EmqxPublishMessage m;
        m.topic = "fireport/device/" + uuid + (suffix.empty() ? "" : "/" + suffix);
        m.payload = payload;
        m.qos = qos;
        m.retain = retain;
        msgs.push_back(std::move(m));
    }

    co_return co_await publishBulk(msgs, outError);
}
