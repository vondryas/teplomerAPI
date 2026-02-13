#pragma once

#include <facades/EMQX_provisioning/EMQX.h> // EmqxUserCredential, EmqxPublishMessage
#include <models/list_model/FireportDeviceDataList.h>
#include <models/model_utils/mvector.h>
#include <models/orm_model/FireportDeviceData.h>

#include <string>
#include <vector>

namespace emqx_mapper
{
    class EmqxModelMapper
    {
    public:
        using FireportDeviceData = drogon_model::defaultdb::FireportDeviceData;

        // --- ENTITY -> EMQX ---
        static EmqxUserCredential
            toCredential(const FireportDeviceData& entity,
                bool isSuperuser = false,
                bool skipEmptyPassword = true);

        static std::string
            toDeviceUuid(const FireportDeviceData& entity);

        static EmqxPublishMessage
            toPublishMessageForDevice(const FireportDeviceData& entity,
                const std::string& topicSuffix,
                const std::string& payload,
                int qos = 0,
                bool retain = false);

        // --- LIST MODEL / COLLECTIONS -> EMQX ---
        static std::vector<EmqxUserCredential>
            toCredentials(const list_model::FireportDeviceDataList& list,
                bool isSuperuser = false,
                bool skipEmptyPassword = true);

        static std::vector<EmqxUserCredential>
            toCredentials(const mvector<FireportDeviceData>& data,
                bool isSuperuser = false,
                bool skipEmptyPassword = true);

        static std::vector<std::string>
            toDeviceUuids(const list_model::FireportDeviceDataList& list);

        static std::vector<std::string>
            toDeviceUuids(const mvector<FireportDeviceData>& data);

        static std::vector<EmqxPublishMessage>
            toPublishMessagesForDevices(const std::vector<std::string>& deviceUuids,
                const std::string& topicSuffix,
                const std::string& payload,
                int qos = 0,
                bool retain = false);

        // Bonus: kolekce entity -> publish messages
        static std::vector<EmqxPublishMessage>
            toPublishMessagesForDevices(const mvector<FireportDeviceData>& data,
                const std::string& topicSuffix,
                const std::string& payload,
                int qos = 0,
                bool retain = false);
    };
}
