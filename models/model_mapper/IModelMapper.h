#pragma once


namespace model_interface {
	class IModel;
}

namespace model_mapper
{
	class IModelMapper
	{
	public:
		virtual ~IModelMapper() = default;
		/**
		 * Maps a request model to an ORM model.
		 * @tparam RequestModelType The type of the request model.
		 * @return The ORM model corresponding to the request model.
		 */

		virtual void mapToOrmModel(model_interface::IModel& model) const = 0;
		virtual void mapToListModel(model_interface::IModel& model) const = 0;
		virtual void mapToRequestModel(model_interface::IModel& model) const = 0;
	};
}