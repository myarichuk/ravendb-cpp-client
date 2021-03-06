#pragma once
#include <typeindex>
#include "ConcurrencyCheckMode.h"
#include "Constants.h"
#include "MetadataAsDictionary.h"
#include "Parameters.h"

namespace ravendb::client::documents::session
{
	struct DocumentInfo
	{
	private:
		template<typename T>
		static std::shared_ptr<void> inner_default_from_json(const nlohmann::json& json)
		{
			if constexpr (std::is_constructible_v<T>)
			{
				auto temp = std::make_shared<T>(json.get<T>());//conversion(deserialization)
				return std::static_pointer_cast<void>(temp);
			}else
			{
				return {};
			}
		}

		template<typename T>
		static void inner_default_entity_update(std::shared_ptr<void> entity, const nlohmann::json& new_json)
		{
			if constexpr (std::is_default_constructible_v<T>)
			{
				auto document = std::static_pointer_cast<T>(entity);
				*document = new_json.get<T>(); //change the old document with the new one obtained from 'new_json'
			}
		}

	public:
		using FromJsonConverter = std::function<std::shared_ptr<void>(const nlohmann::json&)>;
		using ToJsonConverter = std::function<nlohmann::json(std::shared_ptr<void>)>;
		using EntityUpdater = std::function<void(std::shared_ptr<void>, const nlohmann::json&)>;

#ifndef _MSC_VER
		template<typename T>
        static const ToJsonConverter default_to_json;

		template<typename T>
		static const FromJsonConverter default_from_json;

		template<typename T>
		static const EntityUpdater default_entity_update;
#else
		template <typename T>
		inline static const ToJsonConverter default_to_json = 
			[](std::shared_ptr<void> entity) -> nlohmann::json
		{
			return nlohmann::json(*std::static_pointer_cast<T>(entity));
		};

		template<typename T>
		inline static const FromJsonConverter default_from_json = 
			[](const nlohmann::json& json) -> std::shared_ptr<void>
		{
			return inner_default_from_json<T>(json);
		};

		template<typename T>
		inline static const EntityUpdater default_entity_update =
			[](std::shared_ptr<void> entity, const nlohmann::json& new_json) -> void
		{
			inner_default_entity_update<T>(entity, new_json);
		};
#endif

		nlohmann::json document{};
		nlohmann::json metadata{};

		std::shared_ptr<json::MetadataAsDictionary> metadata_instance{};

		std::string id{};
		std::string change_vector{};

		FromJsonConverter from_json_converter;
		ToJsonConverter to_json_converter;
		EntityUpdater update_from_json;
		
		ConcurrencyCheckMode concurrency_check_mode = ConcurrencyCheckMode::AUTO;
		bool ignore_changes = false;

		std::shared_ptr<void> entity{};
		bool new_document = false;
		std::string collection{};

		std::optional<std::type_index> stored_type;//to be filled by store()

		DocumentInfo() = default;

		//TODO from entity ctor(perhaps template)

		explicit DocumentInfo(nlohmann::json document_);
	};

#ifndef _MSC_VER
    template <typename T>
    const DocumentInfo::ToJsonConverter DocumentInfo::default_to_json = [](std::shared_ptr<void> entity) -> nlohmann::json
    {
        return nlohmann::json(*std::static_pointer_cast<T>(entity));
    };

    template<typename T>
    const DocumentInfo::FromJsonConverter DocumentInfo::default_from_json = [](const nlohmann::json& json) -> std::shared_ptr<void>
    {
        return inner_default_from_json<T>(json);
    };

    template<typename T>
    const DocumentInfo::EntityUpdater DocumentInfo::default_entity_update =
        [](std::shared_ptr<void> entity, const nlohmann::json& new_json) -> void
    {
        inner_default_entity_update<T>(entity, new_json);
    };
#endif
}
