#include "EMQXMapper.h"

#include <algorithm>

namespace emqx_mapper
{
    // -------- ENTITY -> EMQX --------

    EmqxUserCredential
        EmqxModelMapper::toCredential(const FireportDeviceData& entity,
            bool isSuperuser,
            bool skipEmptyPassword)
    {
        // Pøedpoklad: id = uuid, password = mqtt password
        const std::string userId = entity.getValueOfId();
        const std::string password = entity.getValueOfPassword();

        if (skipEmptyPassword && password.empty())
        {
            // Vrátíme prázdný záznam; volající si mùže zkontrolovat userId/password
            return EmqxUserCredential{};
        }

        EmqxUserCredential c;
        c.userId = userId;
        c.password = password;
        c.isSuperuser = isSuperuser;
        return c;
    }

    std::string
        EmqxModelMapper::toDeviceUuid(const FireportDeviceData& entity)
    {
        return entity.getValueOfId();
    }

    EmqxPublishMessage
        EmqxModelMapper::toPublishMessageForDevice(const FireportDeviceData& entity,
            const std::string& topicSuffix,
            const std::string& payload,
            int qos,
            bool retain)
    {
        std::string suffix = topicSuffix;
        if (!suffix.empty() && suffix.front() == '/')
            suffix.erase(suffix.begin());

        EmqxPublishMessage m;
        m.topic = "fireport/device/" + entity.getValueOfId() + (suffix.empty() ? "" : "/" + suffix);
        m.payload = payload;
        m.qos = qos;
        m.retain = retain;
        return m;
    }

    // -------- LIST/COLLECTIONS -> EMQX --------

    std::vector<EmqxUserCredential>
        EmqxModelMapper::toCredentials(const list_model::FireportDeviceDataList& list,
            bool isSuperuser,
            bool skipEmptyPassword)
    {
        return toCredentials(list.getData(), isSuperuser, skipEmptyPassword);
    }

    std::vector<EmqxUserCredential>
        EmqxModelMapper::toCredentials(const mvector<FireportDeviceData>& data,
            bool isSuperuser,
            bool skipEmptyPassword)
    {
        std::vector<EmqxUserCredential> out;
        out.reserve(data.size());

        for (const auto& row : data)
        {
            auto c = toCredential(row, isSuperuser, skipEmptyPassword);
            if (c.userId.empty()) continue;
            if (skipEmptyPassword && c.password.empty()) continue;
            out.push_back(std::move(c));
        }

        return out;
    }

    std::vector<std::string>
        EmqxModelMapper::toDeviceUuids(const list_model::FireportDeviceDataList& list)
    {
        return toDeviceUuids(list.getData());
    }

    std::vector<std::string>
        EmqxModelMapper::toDeviceUuids(const mvector<FireportDeviceData>& data)
    {
        std::vector<std::string> out;
        out.reserve(data.size());

        for (const auto& row : data)
        {
            out.push_back(row.getValueOfId());
        }

        std::sort(out.begin(), out.end());
        out.erase(std::unique(out.begin(), out.end()), out.end());
        return out;
    }

    std::vector<EmqxPublishMessage>
        EmqxModelMapper::toPublishMessagesForDevices(const std::vector<std::string>& deviceUuids,
            const std::string& topicSuffix,
            const std::string& payload,
            int qos,
            bool retain)
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

        return msgs;
    }

    std::vector<EmqxPublishMessage>
        EmqxModelMapper::toPublishMessagesForDevices(const mvector<FireportDeviceData>& data,
            const std::string& topicSuffix,
            const std::string& payload,
            int qos,
            bool retain)
    {
        std::string suffix = topicSuffix;
        if (!suffix.empty() && suffix.front() == '/')
            suffix.erase(suffix.begin());

        std::vector<EmqxPublishMessage> msgs;
        msgs.reserve(data.size());

        for (const auto& row : data)
        {
            // entity -> publish message
            msgs.push_back(toPublishMessageForDevice(row, suffix, payload, qos, retain));
        }

        return msgs;
    }
}
