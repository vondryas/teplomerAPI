#pragma once 
#include <optional>
#include <string>
#include <string_view>
#include <stdexcept>
#include <drogon/HttpResponse.h>
#include <fmt/core.h>


using namespace drogon;

namespace TryNumberParsing {
	template <class T> inline constexpr bool always_false = false;
}

template <typename T>
std::optional<T> tryParse(const std::string& input, const std::string& paramName, HttpResponsePtr& outErrorResp)
{
	try {
		if constexpr (std::is_same_v<T, int>) {
			return std::stoi(input);
		}
		else if constexpr (std::is_same_v<T, long>) {
			return std::stol(input);
		}
		else if constexpr (std::is_same_v<T, long long>) {
			return std::stoll(input);
		}
		else if constexpr (std::is_same_v<T, unsigned long> ||
			std::is_same_v<T, size_t>) {
			return static_cast<unsigned long>(std::stoul(input));
		}
		else if constexpr (std::is_same_v<T, unsigned long long>) {
			return static_cast<unsigned long long>(std::stoull(input));
		}
		else if constexpr (std::is_same_v<T, float>) {
			return std::stof(input);
		}
		else if constexpr (std::is_same_v<T, double>) {
			return std::stod(input);
		}
		else {
			static_assert(TryNumberParsing::always_false<T>, "Unsupported type for tryParse");
		}
	}
	catch (const std::invalid_argument& e) {
		LOG_DEBUG << paramName << " parsing error: " << input;
		outErrorResp = responses::wrongRequestResponse(fmt::format("{} parsing error: {}", paramName, e.what()));
	}
	catch (const std::out_of_range& e) {
		LOG_DEBUG << paramName << " out of range: " << input;
		outErrorResp = responses::wrongRequestResponse(fmt::format("{} out of range. Error: {}", paramName, e.what()));
	}

	return std::nullopt;
}
