#pragma once

#include "IListModel.h"
#include <models/model_utils/mvector.h>
#include <models/orm_model/Users.h> 

namespace list_model
{
	class UsersList : public IListModel
	{
	public:
		UsersList() = default;
		void setTotalCount(size_t count) { totalCount_ = count; }
		void setData(const mvector<drogon_model::defaultdb::Users>& data) { data_ = data; }
		void setLimit(size_t limit) { limit_ = limit; }
		void setPage(size_t page) { page_ = page; }
		void mapToOrmModel(model_interface::IModel& model) const override;
		void mapToListModel(model_interface::IModel& model) const override;
		void mapToRequestModel(model_interface::IModel& model) const override;
		Json::Value toJson() const override;

	private:
		mvector<drogon_model::defaultdb::Users> data_;
		size_t totalCount_ = 0;
		size_t limit_ = 0;
		size_t page_ = 1;
	};
}