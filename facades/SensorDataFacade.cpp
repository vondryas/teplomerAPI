#include "SensorDataFacade.h"  
#include <drogon/orm/CoroMapper.h>  
#include <drogon/orm/Exception.h>  
#include <memory>

using SensorData = drogon_model::teplomer_db::SensorData;

void facade::SensorDataFacade::hello() const
{
	LOG_INFO << "HELLO";
}

Task<SensorData> facade::SensorDataFacade::getById(const std::string& id) const
{
	auto mapper = drogon::orm::CoroMapper<SensorData>(drogon::app().getFastDbClient());
	co_return co_await mapper.findByPrimaryKey(id);
}

Task<mvector<SensorData>> facade::SensorDataFacade::getAll() const
{
	auto mapper = drogon::orm::CoroMapper<SensorData>(drogon::app().getFastDbClient());
	auto data = co_await mapper.orderBy(SensorData::Cols::_measured_at, drogon::orm::SortOrder::DESC)
		.findAll();
	co_return mvector<SensorData>(data);
}

Task<mvector<SensorData>> facade::SensorDataFacade::getPaginated(const size_t page, const size_t limit) const
{
	auto mapper = drogon::orm::CoroMapper<SensorData>(drogon::app().getFastDbClient());
	auto data = co_await mapper.orderBy(SensorData::Cols::_measured_at, drogon::orm::SortOrder::DESC)
		.paginate(page, limit)
		.findAll();
	co_return mvector<SensorData>(data);
}

Task<SensorData> facade::SensorDataFacade::create(const request_model::SensorDataRequest& data) const
{
	auto mapper = drogon::orm::CoroMapper<SensorData>(drogon::app().getFastDbClient());
	SensorData sensorData;
	data.mapToOrmModel(sensorData);
	co_return co_await mapper.insert(sensorData);
}

Task<size_t> facade::SensorDataFacade::update(const request_model::SensorDataRequest& data) const
{
	auto shared_data = std::make_shared<request_model::SensorDataRequest>(data);
	auto mapper = drogon::orm::CoroMapper<SensorData>(drogon::app().getFastDbClient());
	auto sensorData = co_await mapper.findByPrimaryKey(uuids::to_string(*data.id));
	shared_data->mapToOrmModel(sensorData);
	co_return co_await mapper.update(sensorData);
}

Task<size_t> facade::SensorDataFacade::deleteById(const std::string& id) const
{
	auto mapper = drogon::orm::CoroMapper<SensorData>(drogon::app().getFastDbClient());
	co_return co_await mapper.deleteByPrimaryKey(id);
}
