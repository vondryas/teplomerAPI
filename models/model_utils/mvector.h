#pragma once

#include <vector>
#include <json/json.h>
#include <models/model_interface/IModel.h>


template<typename T>
class mvector : public model_interface::IModel
{
	static_assert(std::is_base_of<model_interface::IModel, T>::value,
		"T must derive from DrogonModelBase");

private:
	std::vector<T> data;

public:
	// create vector
	mvector(std::vector<T> v) : data(v) {};
	mvector() = default;

	// access to vector
	std::vector<T>& raw() { return data; }
	const std::vector<T>& raw() const { return data; }


	// convert to Json
	Json::Value toJson() const
	{
		Json::Value jsonArray(Json::arrayValue);
		for (const auto& item : data) {
			jsonArray.append(item.toJson());
		}
		return jsonArray;
	}

	void mapToOrmModel(model_interface::IModel& ormModel) const override
	{
		throw std::runtime_error("Mapping to ORM model is not implemented for mvector");
	}
	void mapToListModel(model_interface::IModel& ormModel) const override
	{
		throw std::runtime_error("Mapping to list model is not implemented for mvector");
	}
	void mapToRequestModel(model_interface::IModel& ormModel) const override
	{
		throw std::runtime_error("Mapping to request model is not implemented for mvector");
	}

	// std::vector functions

	T& at(size_t pos) { return data.at(pos); }
	const T& at(size_t pos) const { return data.at(pos); }
	T& operator[](size_t pos) { return data[pos]; }
	const T& operator[](size_t pos) const { return data[pos]; }
	T& front() { return data.front(); }
	const T& front() const { return data.front(); }
	T& back() { return data.back(); }
	const T& back() const { return data.back(); }
	T* dataPtr() { return data.data(); }
	const T* dataPtr() const { return data.data(); }

	auto begin() { return data.begin(); }
	auto end() { return data.end(); }
	auto begin() const { return data.begin(); }
	auto end() const { return data.end(); }
	auto cbegin() const { return data.cbegin(); }
	auto cend() const { return data.cend(); }
	auto rbegin() { return data.rbegin(); }
	auto rend() { return data.rend(); }
	auto rbegin() const { return data.rbegin(); }
	auto rend() const { return data.rend(); }

	bool empty() const { return data.empty(); }
	size_t size() const { return data.size(); }
	size_t capacity() const { return data.capacity(); }
	void reserve(size_t new_cap) { data.reserve(new_cap); }
	void shrink_to_fit() { data.shrink_to_fit(); }

	void clear() { data.clear(); }
	void push_back(const T& value) { data.push_back(value); }
	void push_back(T&& value) { data.push_back(std::move(value)); }
	void pop_back() { data.pop_back(); }
	void resize(size_t count) { data.resize(count); }
	void resize(size_t count, const T& value) { data.resize(count, value); }
	void swap(mvector& other) { data.swap(other.data); }

	auto insert(typename std::vector<T>::const_iterator pos, const T& value) { return data.insert(pos, value); }
	auto insert(typename std::vector<T>::const_iterator pos, T&& value) { return data.insert(pos, std::move(value)); }
	auto insert(typename std::vector<T>::const_iterator pos, size_t count, const T& value) { return data.insert(pos, count, value); }
	template<class InputIt>
	auto insert(typename std::vector<T>::const_iterator pos, InputIt first, InputIt last) { return data.insert(pos, first, last); }

	auto erase(typename std::vector<T>::const_iterator pos) { return data.erase(pos); }
	auto erase(typename std::vector<T>::const_iterator first, typename std::vector<T>::const_iterator last) { return data.erase(first, last); }

};