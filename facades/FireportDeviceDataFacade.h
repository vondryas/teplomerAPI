#pragma once

#include "FacadeBase.h"
#include <models/orm_model/FireportDeviceData.h>
#include <models/request_model/FireportDeviceDataRequest.h>
#include <models/list_model/FireportDeviceDataList.h>

#include <models/filter_model/FireportDeviceFilter.h>
#include <facades/mapper/JoinCoroMapper.h>

using namespace drogon;
namespace facade
{
	class FireportDeviceDataFacade : public FacadeBase
	{
		using FireportDeviceData = drogon_model::defaultdb::FireportDeviceData;
		using FireportDeviceDataList = list_model::FireportDeviceDataList;
		using FireportDeviceDataRequest = request_model::FireportDeviceDataRequest;
		using FireportDeviceFilter = filter_model::FireportDeviceFilter;
	public:
		FireportDeviceDataFacade() = default;
		Task<FireportDeviceData> getById(const std::string& id) const; // Use drogon::Task
		Task<FireportDeviceData> getByIdOwner(const std::string& id, const std::string& ownerId = "") const;
		Task<FireportDeviceDataList> getAll() const;
		Task<FireportDeviceDataList> getAllByOwner(const std::string& ownerId) const;
		Task<FireportDeviceDataList> getByFilterAll(const FireportDeviceFilter& filter) const;
		Task<FireportDeviceDataList> getByFilterByOwner(const FireportDeviceFilter& filter, const std::string& ownerId = "") const;
		Task<FireportDeviceData> create(const FireportDeviceDataRequest& data) const;
		Task<size_t> update(const FireportDeviceDataRequest& data) const;
		Task<size_t> deleteById(const std::string& id) const;
		Task<void> NotifyDeviceById(const std::string& id) const;
		Task<void> NotifyDeviceAllOwner(const FireportDeviceFilter& filter) const;
		Task<void> NotifyAllDevices() const;
	};
}