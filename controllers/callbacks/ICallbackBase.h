#pragma once

#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <vector>
#include <functional>
#include <models/IDrogonModel.h>
#include <json/json.h>
#include <string>
#include <models/model_utils/ModelUtils.h>
#include <controllers/responses/Response.h>

// Type aliases for various callback types
using HttpResponseCallback = std::function<void(const drogon::HttpResponsePtr&)>;
using DbErrorCallback =
std::function<void(const drogon::orm::DrogonDbException&)>;
template <typename T>
using SingleRowCallback = std::function<void(T)>;
template <typename T>
using MultipleRowsCallback = std::function<void(std::vector<T>)>;
using CountCallback = std::function<void(const size_t)>;

namespace callbacks
{
	// Interface for callback handling for database operations
	class ICallbackBase
	{
	public:
		virtual ~ICallbackBase() = default;

	protected:
		// Error callback to handle database errors Interface
		virtual DbErrorCallback onError(HttpResponseCallback& cb) const = 0;

		// Callback for single row results Interface make sure to implement
		template <typename T>
		SingleRowCallback<T> onSingleRow(HttpResponseCallback& cb) const
		{
			throw std::logic_error("onSingleRow not implemented");
		}

		// Callback for multiple rows results Interface make sure to implement
		template <typename T>
		MultipleRowsCallback<T> onMultipleRows(HttpResponseCallback& cb) const
		{
			throw std::logic_error("onMultipleRows not implemented");
		}

		// Callback for count results Interface make sure to implement                                                                                                                                                                                                                    
		virtual CountCallback onCount(HttpResponseCallback& cb) const = 0;

		// Optional: Add more common functionality if needed
	};
} // namespace callbacks