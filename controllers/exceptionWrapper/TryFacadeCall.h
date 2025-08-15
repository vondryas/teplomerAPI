#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <stdexcept>
#include <drogon/HttpResponse.h>
#include <controllers/responses/Response.h>
#include <fmt/core.h>
#include <type_traits>
#include <models/model_utils/mvector.h>

namespace TryFacadeCall {
	template <class T> inline constexpr bool always_false = false;
}

template <typename T>
struct AwaitableResult {
	static_assert(sizeof(T) == 0, "AwaitableResult<T>: Unsupported awaitable type");
};

// Specializace pro Task<T>
template <typename T>
struct AwaitableResult<Task<T>> {
	using type = T;
};


template <typename Awaitable>
Task<std::optional<typename AwaitableResult<std::decay_t<Awaitable>>::type>>
coroTryFacadeCall(
	Awaitable&& awaitable,
	const std::string& context,
	const std::string& idStr,
	HttpResponsePtr& outErrorResp
)
{
	using T = typename AwaitableResult<std::decay_t<Awaitable>>::type;

	static_assert((std::is_base_of_v<model_interface::IModel, T>
		|| std::is_same_v<T, std::size_t>),
		"T must derive from IModel or type must be size_t");
	try {
		T result = co_await std::forward<Awaitable>(awaitable);
		co_return result;
	}
	catch (const drogon::orm::DrogonDbException& e) {
		const auto* s = dynamic_cast<const drogon::orm::Failure*>(&e.base());
		const auto* unexpectedRows = dynamic_cast<const drogon::orm::UnexpectedRows*>(&e.base());

		if (s) {
			LOG_ERROR << "Unexpected DB error in " << context << " with id " << idStr << ": " << e.base().what();
			outErrorResp = responses::internalServerErrorResponse(
				fmt::format("Unexpected DB error in {} with id {}: {}", context, idStr, e.base().what()),
				drogon::k500InternalServerError);
		}
		else if (unexpectedRows) {
			LOG_ERROR << "Database not found " << context << " with id " << idStr << ": " << e.base().what();
			outErrorResp = responses::notFoundResponse(
				fmt::format("Database not found {} with id {}: {}", context, idStr, e.base().what()));
		}
		else
		{
			LOG_ERROR << "Database error in " << context << " with id " << idStr << ": " << e.base().what();
			outErrorResp = responses::wrongRequestResponse(
				fmt::format("Database error in {} with {}: {}", context, idStr, e.base().what()));
		}
	}
	catch (const std::exception& except)
	{
		LOG_ERROR << "Exception in " << context << " with id " << idStr << ": " << except.what();
		outErrorResp = responses::wrongRequestResponse(
			fmt::format("{}: {}", idStr, except.what()));
	}

	co_return std::nullopt;
}




