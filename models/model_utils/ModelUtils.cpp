#include "ModelUtils.h"



std::string jsonToString(const Json::Value& json)
{
	Json::StreamWriterBuilder writer;
	writer["indentation"] = ""; // No indentation for compact output
	std::string output = Json::writeString(writer, json);
	return output;
}
