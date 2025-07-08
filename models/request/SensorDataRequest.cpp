#include "SensorDataRequest.h"

request_model::SensorData::SensorData() 
{
	this->id = -1;
	this->battery = -1;
	this->temperature = "";
	this->humidity = "";
	this->pressure = "";
	this->date = trantor::Date();
	this->time = "";
}

bool request_model::SensorData::isEmpty()
{
	if (this->id == -1 &&
		this->battery == -1 &&
		this->temperature == "" &&
		this->humidity == "" &&
		this->pressure == "" &&
		this->date == trantor::Date() &&
		this->time == "")
	{
		return true;
	}
	return false;
}
