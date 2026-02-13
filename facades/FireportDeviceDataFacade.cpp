#include "FireportDeviceDataFacade.h"

#include <drogon/orm/CoroMapper.h>  
#include <drogon/orm/Exception.h>  
#include <memory>
#include <models/orm_model/Devices.h>
#include <models/orm_model/Users.h>
#include <models/model_mapper/EMQXMapper.h>
#include <facades/EMQX_provisioning/EMQX.h>

using FireportDeviceData = drogon_model::defaultdb::FireportDeviceData;
using Devices = drogon_model::defaultdb::Devices;
using Users = drogon_model::defaultdb::Users;
using FireportDeviceDataList = list_model::FireportDeviceDataList;
using FireportDeviceDataRequest = request_model::FireportDeviceDataRequest;
using JoinCoroMapper = drogon::orm::JoinCoroMapper<FireportDeviceData>;
using emqx_mapper::EmqxModelMapper;

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

Task<FireportDeviceData> facade::FireportDeviceDataFacade::create(const FireportDeviceDataRequest& data) const
{
	auto emqx = EmqxProvisioner();
	auto mapper = drogon::orm::CoroMapper<FireportDeviceData>(drogon::app().getFastDbClient());
	FireportDeviceData FireportDeviceData;
	data.mapToOrmModel(FireportDeviceData);
	auto res = co_await mapper.insert(FireportDeviceData);
	co_await emqx.importUser(EmqxModelMapper::toCredential(res));
	co_return res;
}

Task<size_t> facade::FireportDeviceDataFacade::update(const FireportDeviceDataRequest& data) const
{
	auto emqx = EmqxProvisioner();
	auto mapper = drogon::orm::CoroMapper<FireportDeviceData>(drogon::app().getFastDbClient());
	auto shared_data = std::make_shared<FireportDeviceDataRequest>(data);
	auto fireportDeviceData = co_await mapper.findByPrimaryKey(uuids::to_string(*data.id));
	shared_data->mapToOrmModel(fireportDeviceData);
	auto res = co_await mapper.update(fireportDeviceData);
	EmqxUserCredential credential{ uuids::to_string(*shared_data->id), shared_data->password };
	co_await emqx.importUser(credential);
	co_return res;
}

Task<size_t> facade::FireportDeviceDataFacade::deleteById(const std::string& id) const
{
	auto emqx = EmqxProvisioner();
	auto mapper = drogon::orm::CoroMapper<FireportDeviceData>(drogon::app().getFastDbClient());
	auto res = co_await mapper.deleteByPrimaryKey(id);
	co_await emqx.deleteUser(id);
	co_return res;
}

Task<bool> facade::FireportDeviceDataFacade::NotifyDeviceByIdOwner(const std::string& id) const
{
	auto emqx = EmqxProvisioner();
	auto data = co_await getByIdOwner(id);
	auto message = EmqxModelMapper::toPublishMessageForDevice(data, "", "Fire");
	co_return co_await emqx.publish(message);;
}

Task<bool> facade::FireportDeviceDataFacade::NotifyDeviceById(const std::string& id) const
{
	auto emqx = EmqxProvisioner();
	auto data = co_await getById(id);
	auto message = EmqxModelMapper::toPublishMessageForDevice(data, "", "Fire");
	co_return co_await emqx.publish(message);;
}

Task<bool> facade::FireportDeviceDataFacade::NotifyDeviceAllOwner(const std::string& ownerId) const
{
	auto emqx = EmqxProvisioner();
	auto dataList = co_await getAllByOwner(ownerId);
	auto uuids = EmqxModelMapper::toDeviceUuids(dataList);
	co_return co_await emqx.publishToDevices(uuids, "", "Fire");;
}

Task<bool> facade::FireportDeviceDataFacade::NotifyAllDevices() const
{
	auto emqx = EmqxProvisioner();
	EmqxPublishMessage message{"fireport/device/all","Fire", 0, false};
	co_return co_await emqx.publish(message);;
}
