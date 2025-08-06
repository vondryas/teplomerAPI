#pragma once

#include "FacadeBase.h"
#include <models/orm_model/Devices.h>
#include <models/request_model/DevicesRequest.h>
#include <models/list_model/DevicesList.h>
#include <functional>
#include <drogon/orm/Mapper.h>
#include <drogon/orm/CoroMapper.h>
#include <models/model_utils/mvector.h>


using namespace drogon;
namespace facade
{
	class DevicesFacade : public FacadeBase
	{
		using Devices = drogon_model::defaultdb::Devices;
		using DevicesList = list_model::DevicesList;
		using DevicesRequest = request_model::DevicesRequest;
	public:
		DevicesFacade() = default;
		/*
		Task<Devices> getById(const std::string& id) const; // Use drogon::Task
		Task<DevicesList> getAll() const;
		Task<DevicesList> getPaginated(const size_t page, const size_t limit) const;
		Task<Devices> create(const DevicesRequest& data) const;
		Task<size_t> update(const DevicesRequest& data) const;
		Task<size_t> deleteById(const std::string& id) const;
		*/
	};
}