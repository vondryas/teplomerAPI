#pragma once
#include "FacadeBase.h"
#include <functional>
#include <models/SensorData.h>
#include <models/request/SensorDataRequest.h>
#include <drogon/orm/Mapper.h>
#include <drogon/orm/CoroMapper.h>
#include <models/model_utils/mvector.h>

using namespace drogon;
namespace facade
{
	class SensorDataFacade : public FacadeBase
	{
		using SensorData = drogon_model::teplomer_db::SensorData;
	public:
		// Type aliases for callback functions

		SensorDataFacade()
		{
		}

		void hello() const;
		Task<SensorData> getById(const std::string& id) const; // Use drogon::Task
		Task<mvector<SensorData>> getAll() const;
		Task<mvector<SensorData>> getPaginated(const size_t page, const size_t limit) const;
		Task<SensorData> create(const request_model::SensorData& data) const;
		Task<size_t> update(const request_model::SensorData& data) const;
		Task<size_t> deleteById(const int32_t id) const;

	private:
	};
}