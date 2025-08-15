#include "WeatherStationDataFacade.h"
#include <drogon/orm/CoroMapper.h>  
#include <drogon/orm/Exception.h>  
#include <memory>

using WeatherStationData = drogon_model::defaultdb::WeatherStationData;
using WeatherStationDataList = list_model::WeatherStationDataList;
using WeatherStationDataRequest = request_model::WeatherStationDataRequest;

Task<WeatherStationData> facade::WeatherStationDataFacade::getById(const std::string& id) const
{
	auto mapper = drogon::orm::CoroMapper<WeatherStationData>(drogon::app().getFastDbClient());
	co_return co_await mapper.findByPrimaryKey(id);
}

Task<WeatherStationDataList> facade::WeatherStationDataFacade::getAll() const
{
	auto mapper = drogon::orm::CoroMapper<WeatherStationData>(drogon::app().getFastDbClient());
	auto data = co_await mapper.orderBy(WeatherStationData::Cols::_measure_at, drogon::orm::SortOrder::DESC)
		.findAll();
	WeatherStationDataList WeatherStationDataList;
	WeatherStationDataList.setData(mvector<WeatherStationData>(data));
	WeatherStationDataList.setTotalCount(data.size());
	WeatherStationDataList.setLimit(data.size());
	co_return WeatherStationDataList;
}

Task<WeatherStationDataList> facade::WeatherStationDataFacade::getPaginated(const size_t page, const size_t limit) const
{
	auto mapper = drogon::orm::CoroMapper<WeatherStationData>(drogon::app().getFastDbClient());
	auto data = co_await mapper.orderBy(WeatherStationData::Cols::_measure_at, drogon::orm::SortOrder::DESC)
		.paginate(page, limit)
		.findAll();
	WeatherStationDataList WeatherStationDataList;
	WeatherStationDataList.setData(mvector<WeatherStationData>(data));
	WeatherStationDataList.setTotalCount(co_await mapper.count());
	WeatherStationDataList.setLimit(limit);
	WeatherStationDataList.setPage(page);
	co_return WeatherStationDataList;
}

Task<WeatherStationDataList> facade::WeatherStationDataFacade::getByFilter(const WeatherFilter& filter) const
{
	auto mapper = drogon::orm::CoroMapper<WeatherStationData>(drogon::app().getFastDbClient());
	drogon::orm::Criteria criteria;
	if (!filter.stationId.empty())
	{
		criteria = criteria && drogon::orm::Criteria(WeatherStationData::Cols::_device_id, drogon::orm::CompareOperator::EQ, filter.stationId);
	}
	if (!filter.startDate.empty())
	{
		criteria = criteria && drogon::orm::Criteria(WeatherStationData::Cols::_measure_at, drogon::orm::CompareOperator::GE, filter.startDate);
	}
	if (!filter.endDate.empty())
	{
		criteria = criteria && drogon::orm::Criteria(WeatherStationData::Cols::_measure_at, drogon::orm::CompareOperator::LE, filter.endDate);
	}
	if (!filter.orderBy.empty())
	{
		if (filter.orderBy == "measure_at")
		{
			if (filter.orderDirection == "DESC")
			{
				mapper = mapper.orderBy(WeatherStationData::Cols::_measure_at, drogon::orm::SortOrder::DESC);
			}
			else
			{
				mapper = mapper.orderBy(WeatherStationData::Cols::_measure_at, drogon::orm::SortOrder::ASC);
			}
		}
		else if (filter.orderBy == "device_id")
		{
			if (filter.orderDirection == "DESC")
			{
				mapper = mapper.orderBy(WeatherStationData::Cols::_device_id, drogon::orm::SortOrder::DESC);
			}
			else
			{
				mapper = mapper.orderBy(WeatherStationData::Cols::_device_id, drogon::orm::SortOrder::ASC);
			}
		}
	}
	int offset = 1;
	int limit = 20;
	WeatherStationDataList weatherStationDataList;
	if (filter.limit > 0 || filter.offset > 1)
	{
		if (filter.limit > 0)
		{
			limit = filter.limit;
		}
		if (filter.offset > 1)
		{
			offset = filter.offset;
		}
		mapper = mapper.paginate(offset, limit);
	}
	auto sensorData = co_await mapper.findBy(criteria);
	weatherStationDataList.setData(mvector<WeatherStationData>(sensorData));
	weatherStationDataList.setTotalCount(co_await mapper.count());
	weatherStationDataList.setLimit(limit);
	weatherStationDataList.setPage(offset);
	co_return weatherStationDataList;
}

Task<WeatherStationData> facade::WeatherStationDataFacade::create(const request_model::WeatherStationDataRequest& data) const
{
	auto mapper = drogon::orm::CoroMapper<WeatherStationData>(drogon::app().getFastDbClient());
	WeatherStationData WeatherStationData;
	data.mapToOrmModel(WeatherStationData);
	co_return co_await mapper.insert(WeatherStationData);
}

Task<size_t> facade::WeatherStationDataFacade::update(const request_model::WeatherStationDataRequest& data) const
{
	auto shared_data = std::make_shared<request_model::WeatherStationDataRequest>(data);
	auto mapper = drogon::orm::CoroMapper<WeatherStationData>(drogon::app().getFastDbClient());
	auto sensorData = co_await mapper.findByPrimaryKey(uuids::to_string(*data.id));
	shared_data->mapToOrmModel(sensorData);
	co_return co_await mapper.update(sensorData);
}

Task<size_t> facade::WeatherStationDataFacade::deleteById(const std::string& id) const
{
	auto mapper = drogon::orm::CoroMapper<WeatherStationData>(drogon::app().getFastDbClient());
	co_return co_await mapper.deleteByPrimaryKey(id);
}
