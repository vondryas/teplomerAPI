#pragma once

#include "../IDrogonModel.h"
#include <json/json.h>
#include <string>
#include <vector>

template <typename T>
Json::Value toJson(const std::vector<T>& vec) {
	static_assert(std::is_base_of<drogon_model::teplomer_db::IDrogonModel, T>::value,
		"T must derive from DrogonModelBase");

	Json::Value arr(Json::arrayValue);
	for (const auto& item : vec) {
		arr.append(item.toJson());
	}
	return arr;
}

std::string jsonToString(const Json::Value& json);
