#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "connector.hpp"

namespace arcxx {
    template<typename Model, typename Attribute, typename Type>
    struct attribute;

    template<typename Model, typename Attribute, typename Type>
    struct attribute_common;

    template<typename Type>
    struct constraint{ using value_type = Type; };

    template<typename T>
    concept is_attribute = requires {
        std::derived_from<T, attribute_common<typename T::model_type, T, typename T::value_type>>;
        requires T::has_column_name();
    };
    template<typename T>
    struct is_attribute_type{ static constexpr bool value = is_attribute<T>; };

    template<typename Model, typename Attribute, typename Aggregator>
    struct attribute_aggregator;

    template<typename T>
    concept is_attribute_aggregator = std::derived_from<T, attribute_aggregator<typename T::model_type, typename T::attribute_type, typename T::aggregator_type>>;

    template<is_connector Connector, is_attribute Attr>
    requires false
    [[nodiscard]] arcxx::string to_string(const Attr& attr, arcxx::string&& buff = {});

    template<is_connector Connector, is_attribute Attr>
    requires false
    void from_string(Attr& attr, const arcxx::string_view str);
}

template<arcxx::is_attribute Attr>
struct std::hash<Attr> {
    std::hash<std::optional<typename Attr::value_type>> inner_hash;
    std::size_t operator()(const Attr& key) const {
        return inner_hash(static_cast<std::optional<typename Attr::value_type>>(key));
    }
};