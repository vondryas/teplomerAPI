#pragma once

#include "FacadeBase.h"
#include <models/orm_model/WeatherStationData.h>
#include <models/request_model/WeatherStationDataRequest.h>
#include <models/list_model/WeatherStationDataList.h>
#include <functional>
#include <drogon/orm/Mapper.h>
#include <drogon/orm/CoroMapper.h>
#include <models/model_utils/mvector.h>
#include <models/filter_model/WeatherFilter.h>
#include <models/orm_model/Users.h>
#include <models/request_model/UsersRequest.h>
#include <models/list_model/UsersList.h>
#include <models/auth_model/AuthModel.h>
#include <models/orm_model/Devices.h>
#include <models/request_model/DevicesRequest.h>
#include <models/list_model/DevicesList.h>



using namespace drogon;
namespace facade
{
	class WeatherStationDataFacade : public FacadeBase
	{
		using WeatherStationData = drogon_model::defaultdb::WeatherStationData;
		using WeatherStationDataList = list_model::WeatherStationDataList;
		using WeatherStationDataRequest = request_model::WeatherStationDataRequest;
		using WeatherFilter = filter_model::WeatherFilter;
		using Users = drogon_model::defaultdb::Users;
		using UsersList = list_model::UsersList;
		using UsersRequest = request_model::UsersRequest;
		using AuthModel = auth_model::AuthModel;
		using Devices = drogon_model::defaultdb::Devices;
		using DevicesList = list_model::DevicesList;
		using DevicesRequest = request_model::DevicesRequest;
	public:
		WeatherStationDataFacade() = default;


		Task<WeatherStationData> getById(const std::string& id) const; // Use drogon::Task
		Task<WeatherStationDataList> getAll() const;
		Task<WeatherStationDataList> getPaginated(const size_t page, const size_t limit) const;
		Task<WeatherStationDataList> getByFilter(const WeatherFilter& filter) const;
		Task<WeatherStationData> create(const WeatherStationDataRequest& data) const;
		Task<size_t> update(const WeatherStationDataRequest& data) const;
		Task<size_t> deleteById(const std::string& id) const;
	};
}