#include "FireportDeviceDataList.h"

void list_model::FireportDeviceDataList::mapToOrmModel(model_interface::IModel& model) const
{
	throw std::runtime_error("Mapping to ORM model is not implemented for FireportDeviceData list model");
}

void list_model::FireportDeviceDataList::mapToListModel(model_interface::IModel& model) const
{
	auto& fireportDeviceDataList = dynamic_cast<FireportDeviceDataList&>(model);
	fireportDeviceDataList.setData(data_);
	fireportDeviceDataList.setTotalCount(totalCount_);
	fireportDeviceDataList.setLimit(limit_);
	fireportDeviceDataList.setPage(page_);
}

void list_model::FireportDeviceDataList::mapToRequestModel(model_interface::IModel& model) const
{
	throw std::runtime_error("Mapping to request model is not implemented for FireportDeviceData list model");
}

Json::Value list_model::FireportDeviceDataList::toJson() const
{
	Json::Value json;
	json["Data"] = data_.toJson();
	json["TotalICount"] = totalCount_;
	json["ICount"] = data_.size();
	json["PerPage"] = limit_;
	json["Page"] = page_;
	return json;
}