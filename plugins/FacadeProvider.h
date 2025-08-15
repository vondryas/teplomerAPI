#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif


#include <facades/FacadeBase.h>
#include <drogon/plugins/Plugin.h>

#include <memory>
#include <typeindex>
#include <unordered_map>

namespace facade
{
	class FacadeProvider : public drogon::Plugin<FacadeProvider> {
	public:
		void initAndStart(const Json::Value&) override;
		void shutdown() override;

		template <class T>
		std::shared_ptr<T> getFacade() {
			static_assert(std::is_base_of<FacadeBase, T>::value,
				"T must derive from FacadeBase");
			auto it = facades_.find(std::type_index(typeid(T)));
			if (it != facades_.end()) {
				return std::dynamic_pointer_cast<T>(it->second);
			}
			return nullptr;
		}

		template <typename T>
		void registerFacade(std::shared_ptr<T> facade) {
			facades_[std::type_index(typeid(T))] = facade;
		}

	private:
		std::unordered_map<std::type_index, std::shared_ptr<FacadeBase>> facades_;
	};
}