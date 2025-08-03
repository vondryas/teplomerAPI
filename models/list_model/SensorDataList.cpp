#include "SensorDataList.h"

void list_model::SensorDataList::mapToOrmModel(model_interface::IModel& model) const
{
	LOG_INFO << "Mapping list model to ORM model";
	throw std::runtime_error("Mapping to ORM model is not implemented for SensorData list model");
}

void list_model::SensorDataList::mapToListModel(model_interface::IModel& model) const
{
	LOG_INFO << "Mapping list model to another list model";
	auto& sensorDataList = dynamic_cast<list_model::SensorDataList&>(model);
	sensorDataList.setData(data_);
	sensorDataList.setTotalCount(totalCount_);
}

void list_model::SensorDataList::mapToRequestModel(model_interface::IModel& model) const
{
	LOG_INFO << "Mapping list model to request model";
	throw std::runtime_error("Mapping to request model is not implemented for SensorData list model");
}

Json::Value list_model::SensorDataList::toJson() const
{
	Json::Value json;
	json["Data"] = data_.toJson();
	json["TotalICount"] = totalCount_;
	json["ICount"] = data_.size();
	json["PerPage"] = limit_;
	json["Page"] = page_;
	return json;
}
