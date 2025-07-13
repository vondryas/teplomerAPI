#include "SensorDataFacade.h"  
#include <drogon/orm/CoroMapper.h>  
#include <drogon/orm/Exception.h>  

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
	auto data = co_await mapper.findAll();
	co_return mvector<SensorData>(data);
}

Task<mvector<SensorData>> facade::SensorDataFacade::getPaginated(const size_t page, const size_t limit) const
{
	auto mapper = drogon::orm::CoroMapper<SensorData>(drogon::app().getFastDbClient());
	auto data = co_await mapper.paginate(page, limit).findAll();
	co_return mvector<SensorData>(data);
}

Task<SensorData> facade::SensorDataFacade::create(const request_model::SensorData& data) const
{
	auto mapper = drogon::orm::CoroMapper<SensorData>(drogon::app().getFastDbClient());
	co_return co_await mapper.findByPrimaryKey("2ef539a2-58c3-43f5-9f9f-b9243959c28c");
}

Task<size_t> facade::SensorDataFacade::update(const request_model::SensorData& data) const
{
	co_return 1;
}

Task<size_t> facade::SensorDataFacade::deleteById(const int32_t id) const
{
	co_return 1;
}
