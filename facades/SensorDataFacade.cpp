#include "SensorDataFacade.h"

void facade::SensorDataFacade::hello() const
{
	LOG_INFO << "HELLO";
}

void facade::SensorDataFacade::getById(const int32_t id, const SingleRowCallback& callback, const DbErrorCallback& err) const
{
	mapper_->findByPrimaryKey(id, callback, err);
}

void facade::SensorDataFacade::getAll(const MultipleRowsCallback& callback, const DbErrorCallback& err) const
{
	mapper_->findAll(callback, err);
}


void facade::SensorDataFacade::getPaginated(const uint32_t page, const uint32_t limit, const MultipleRowsCallback& callback, const DbErrorCallback& err) const
{
	mapper_->paginate(page,limit).findAll(
		callback,
		err
	);
}

void facade::SensorDataFacade::create(const request_model::SensorData& data, const SingleRowCallback& callback, const DbErrorCallback& err) const
{
}

void facade::SensorDataFacade::update(const request_model::SensorData& data, const CountCallback& callback, const DbErrorCallback& err) const
{
}

void facade::SensorDataFacade::deleteById(const int32_t id, const CountCallback& callback, const DbErrorCallback& err) const
{
}
