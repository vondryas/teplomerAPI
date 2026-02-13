#pragma once

#include "IListModel.h"

#include <models/model_utils/mvector.h>
#include <models/orm_model/FireportDeviceData.h> // <-- FIX: include correct header for FireportDeviceData

namespace list_model
{
	// <-- FIX: make type accessible
	class FireportDeviceDataList : public IListModel
	{
		using FireportDeviceData = drogon_model::defaultdb::FireportDeviceData;
	public:
		FireportDeviceDataList() = default;
		explicit FireportDeviceDataList(const mvector<FireportDeviceData>& data)
			: data_(data) {
		}

		const mvector<FireportDeviceData>& getData() const noexcept { return data_; }
		void setTotalCount(size_t count) { totalCount_ = count; }
		void setData(const mvector<FireportDeviceData>& data) { data_ = data; }
		void setLimit(size_t limit) { limit_ = limit; }
		void setPage(size_t page) { page_ = page; }

		// Inherited via IListModel
		void mapToOrmModel(model_interface::IModel& model) const override;
		void mapToListModel(model_interface::IModel& model) const override;
		void mapToRequestModel(model_interface::IModel& model) const override;
		Json::Value toJson() const override;

	private:
		mvector<FireportDeviceData> data_;
		size_t totalCount_ = 0;
		size_t limit_ = 0;
		size_t page_ = 1;
	};
}
