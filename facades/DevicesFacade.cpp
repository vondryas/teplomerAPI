#include "DevicesFacade.h"

using Devices = drogon_model::defaultdb::Devices;
using DevicesList = list_model::DevicesList;
using DevicesRequest = request_model::DevicesRequest;

Task<Devices> facade::DevicesFacade::create(const DevicesRequest& data) const
{
	auto mapper = drogon::orm::CoroMapper<Devices>(drogon::app().getFastDbClient());
	Devices device;
	data.mapToOrmModel(device);
	co_return co_await mapper.insert(device);
}

Task<Devices> facade::DevicesFacade::getById(const std::string& id) const
{
	auto mapper = drogon::orm::CoroMapper<Devices>(drogon::app().getFastDbClient());
	co_return co_await mapper.findByPrimaryKey(id);
}

Task<DevicesList> facade::DevicesFacade::getAll() const
{
	auto mapper = drogon::orm::CoroMapper<Devices>(drogon::app().getFastDbClient());
	auto data = co_await mapper
		.findAll();
	DevicesList devicesList;
	devicesList.setData(mvector<Devices>(data));
	devicesList.setTotalCount(data.size());
	devicesList.setLimit(data.size());
	co_return devicesList;
}

Task<size_t> facade::DevicesFacade::deleteById(const std::string& id) const
{
	auto mapper = drogon::orm::CoroMapper<Devices>(drogon::app().getFastDbClient());
	co_return co_await mapper.deleteByPrimaryKey(id);
}