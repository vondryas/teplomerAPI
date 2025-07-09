#pragma once
#include <drogon/plugins/Plugin.h>
#include <plugins/FacadeProvider.h>
#include <memory>

// This is a base class for controllers that provides access to a specific facade.
// Type T should be a derived class of facade::FacadeBase.
template <class T>
class ControllerBase
{
public:
	ControllerBase() {
		static_assert(std::is_base_of<facade::FacadeBase, T>::value,
			"T must derive from FacadeBase");
		facade_ = drogon::app().getSharedPlugin<facade::FacadeProvider>()->getFacade<T>();
	}
	virtual ~ControllerBase() = default;
protected:
	std::shared_ptr<T> facade_;
};