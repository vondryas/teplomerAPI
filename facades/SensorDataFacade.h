#pragma once
#include "FacadeBase.h"
#include <controllers/callbacks/ICallbackBase.h>
#include <functional>
#include <models/SensorData.h>
#include <models/request/SensorDataRequest.h>
#include <drogon/orm/Mapper.h>


namespace facade
{
	class SensorDataFacade : public FacadeBase
	{
		using SingleRowCallback =  SingleRowCallback<drogon_model::teplomer_db::SensorData>;
		using MultipleRowsCallback = MultipleRowsCallback<drogon_model::teplomer_db::SensorData>;
	public:
		// Type aliases for callback functions

		SensorDataFacade() : FacadeBase()
		{
			mapper_ = std::make_shared<drogon::orm::Mapper<drogon_model::teplomer_db::SensorData>>(dbClient_);
		}

		void hello() const;
		void getById(const int32_t id,const SingleRowCallback& callback,const DbErrorCallback& err) const;
		void getAll(const MultipleRowsCallback& callback, const DbErrorCallback& err) const;
		void getPaginated(const uint32_t page, const uint32_t limit, const MultipleRowsCallback& callback,const DbErrorCallback& err) const;
		void create(const request_model::SensorData& data, const SingleRowCallback& callback, const DbErrorCallback& err) const;
		void update(const request_model::SensorData& data, const CountCallback& callback, const DbErrorCallback& err) const;
		void deleteById(const int32_t id, const CountCallback& callback, const DbErrorCallback& err) const;

	private:
		std::shared_ptr<drogon::orm::Mapper<drogon_model::teplomer_db::SensorData>> mapper_;
	};
}