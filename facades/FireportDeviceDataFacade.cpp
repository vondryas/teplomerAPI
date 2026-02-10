#include "FireportDeviceDataFacade.h"

#include <drogon/orm/CoroMapper.h>  
#include <drogon/orm/Exception.h>  
#include <memory>
#include <models/orm_model/Devices.h>
#include <models/orm_model/Users.h>

using FireportDeviceData = drogon_model::defaultdb::FireportDeviceData;
using Devices = drogon_model::defaultdb::Devices;
using Users = drogon_model::defaultdb::Users;
using FireportDeviceDataList = list_model::FireportDeviceDataList;
using FireportDeviceDataRequest = request_model::FireportDeviceDataRequest;
using JoinCoroMapper = drogon::orm::JoinCoroMapper<FireportDeviceData>;

Task<FireportDeviceData> facade::FireportDeviceDataFacade::getById(const std::string& id) const
{
	auto mapper = drogon::orm::CoroMapper<FireportDeviceData>(drogon::app().getFastDbClient());
	co_return co_await mapper.findByPrimaryKey(id);
}

Task<FireportDeviceData> facade::FireportDeviceDataFacade::getByIdOwner(const std::string& id, const std::string& ownerId) const
{
	auto mapper = JoinCoroMapper(drogon::app().getFastDbClient());
	co_return co_await mapper
		.join(Devices::tableName, "d", "t.device_id = d.name")
		.join(Users::tableName, "u", "d.user_id = u.id")
		.findOneJoin(drogon::orm::Criteria("d.user_id", drogon::orm::CompareOperator::EQ, ownerId)
			&& drogon::orm::Criteria("t.id", drogon::orm::CompareOperator::EQ, id));

}

Task<FireportDeviceDataList> facade::FireportDeviceDataFacade::getAll() const
{

	co_return co_await facade::FireportDeviceDataFacade::getAllByOwner("");
}

Task<FireportDeviceDataList> facade::FireportDeviceDataFacade::getAllByOwner(const std::string& ownerId) const
{
	auto mapper = JoinCoroMapper(drogon::app().getFastDbClient());
	std::vector<FireportDeviceData> data;
	if (!ownerId.empty())
	{
		data = co_await mapper
			.join(Devices::tableName, "d", "t.device_id = d.name")
			.orderBy(FireportDeviceData::Cols::_device_id, drogon::orm::SortOrder::DESC)
			.findByJoin(drogon::orm::Criteria("d.user_id", drogon::orm::CompareOperator::EQ, ownerId));
	}
	else
	{
		data = co_await mapper.orderBy(FireportDeviceData::Cols::_device_id, drogon::orm::SortOrder::DESC)
			.findAll();
	}
	FireportDeviceDataList FireportDeviceDataList;
	FireportDeviceDataList.setData(mvector<FireportDeviceData>(data));
	FireportDeviceDataList.setTotalCount(data.size());
	FireportDeviceDataList.setLimit(data.size());
	co_return FireportDeviceDataList;
}

Task<FireportDeviceDataList> facade::FireportDeviceDataFacade::getByFilterAll(const FireportDeviceFilter& filter) const
{

	co_return co_await facade::FireportDeviceDataFacade::getByFilterByOwner(filter, "");
}

Task<FireportDeviceDataList> facade::FireportDeviceDataFacade::getByFilterByOwner(const FireportDeviceFilter& filter, const std::string& ownerId) const
{
	auto mapper = drogon::orm::JoinCoroMapper<FireportDeviceData>(drogon::app().getFastDbClient());
	drogon::orm::Criteria criteria;
	if (!filter.deviceId.empty())
	{
		criteria = criteria && drogon::orm::Criteria(FireportDeviceData::Cols::_device_id, drogon::orm::CompareOperator::EQ, filter.deviceId);
	}
	if (!filter.type.has_value())
	{
		criteria = criteria && drogon::orm::Criteria(FireportDeviceData::Cols::_type, drogon::orm::CompareOperator::EQ, *filter.type);
	}
	if (!filter.deviceAddress.empty())
	{
		criteria = criteria && drogon::orm::Criteria(FireportDeviceData::Cols::_device_address, drogon::orm::CompareOperator::EQ, filter.deviceAddress);
	}
	if (!ownerId.empty())
	{
		criteria = criteria && drogon::orm::Criteria(Devices::Cols::_user_id, drogon::orm::CompareOperator::EQ, ownerId);
	}
	if (!filter.orderBy.empty())
	{
		if (filter.orderBy == "device_id" || filter.orderBy == "deviceId" || filter.orderBy == "device id")
		{
			if (filter.orderDirection == "DESC")
			{
				mapper = mapper.orderBy(FireportDeviceData::Cols::_device_id, drogon::orm::SortOrder::DESC);
			}
			else
			{
				mapper = mapper.orderBy(FireportDeviceData::Cols::_device_id, drogon::orm::SortOrder::ASC);
			}
		}
		else if (filter.orderBy == "type")
		{
			if (filter.orderDirection == "DESC")
			{
				mapper = mapper.orderBy(FireportDeviceData::Cols::_type, drogon::orm::SortOrder::DESC);
			}
			else
			{
				mapper = mapper.orderBy(FireportDeviceData::Cols::_type, drogon::orm::SortOrder::ASC);
			}
		}
		else if (filter.orderBy == "device_address" || filter.orderBy == "deviceAddress")
		{
			if (filter.orderDirection == "DESC")
			{
				mapper = mapper.orderBy(FireportDeviceData::Cols::_device_address, drogon::orm::SortOrder::DESC);
			}
			else
			{
				mapper = mapper.orderBy(FireportDeviceData::Cols::_device_address, drogon::orm::SortOrder::ASC);
			}
		}
		int offset = 1;
		if (filter.offset > 0)
		{
			offset = filter.offset;
		}
		int limit = 20;
		if (filter.limit >= 0)
		{
			limit = filter.limit;
		}
		mapper = mapper.paginate(offset, limit);
		FireportDeviceDataList FireportDeviceDataList;
		std::vector<FireportDeviceData> data;
		if (!ownerId.empty())
		{
			mapper = mapper.join(Devices::tableName, "d", "t.device_id = d.name");
			 data = co_await mapper.findByJoin(criteria);
			 FireportDeviceDataList.setTotalCount(co_await mapper.countJoin(criteria));
		}
		else
		{
			data = co_await mapper.findBy(criteria);
			FireportDeviceDataList.setTotalCount(co_await mapper.count(criteria));
		}
		FireportDeviceDataList.setData(mvector<FireportDeviceData>(data));
		FireportDeviceDataList.setLimit(limit);
		FireportDeviceDataList.setPage(offset);
		co_return FireportDeviceDataList;
	}
}

Task<FireportDeviceData> facade::FireportDeviceDataFacade::create(const FireportDeviceDataRequest& data) const
{
	auto mapper = drogon::orm::CoroMapper<FireportDeviceData>(drogon::app().getFastDbClient());
	FireportDeviceData FireportDeviceData;
	data.mapToOrmModel(FireportDeviceData);
	co_return co_await mapper.insert(FireportDeviceData);
}

Task<size_t> facade::FireportDeviceDataFacade::update(const FireportDeviceDataRequest& data) const
{
	auto mapper = drogon::orm::CoroMapper<FireportDeviceData>(drogon::app().getFastDbClient());
	auto shared_data = std::make_shared<FireportDeviceDataRequest>(data);
	auto fireportDeviceData = co_await mapper.findByPrimaryKey(uuids::to_string(*data.id));
	shared_data->mapToOrmModel(fireportDeviceData);
	co_return co_await mapper.update(fireportDeviceData);
}

Task<size_t> facade::FireportDeviceDataFacade::deleteById(const std::string& id) const
{
	auto mapper = drogon::orm::CoroMapper<FireportDeviceData>(drogon::app().getFastDbClient());
	co_return co_await mapper.deleteByPrimaryKey(id);
}

Task<void> facade::FireportDeviceDataFacade::NotifyDeviceById(const std::string& id) const
{
	co_return;
}

Task<void> facade::FireportDeviceDataFacade::NotifyDeviceAllOwner(const FireportDeviceFilter& filter) const
{
	co_return;
}

Task<void> facade::FireportDeviceDataFacade::NotifyAllDevices() const
{
	co_return;
}
