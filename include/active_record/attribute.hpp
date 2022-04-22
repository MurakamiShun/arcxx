#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "adaptor.hpp"

namespace active_record {
    template<typename Model, typename Attribute, typename Type>
    struct attribute;

    template<typename Model, typename Attribute, typename Type>
    struct attribute_common;

    template<typename T>
    concept is_attribute = requires {
        std::derived_from<T, attribute_common<typename T::model_type, typename T::attribute_type, typename T::value_type>>;
        requires T::has_column_name();
    };
    template<typename T>
    struct is_attribute_type{ static constexpr bool value = is_attribute<T>; };

    template<is_attribute Attr>
    constexpr std::size_t attribute_size([[maybe_unused]] const Attr&) noexcept { return sizeof(typename Attr::value_type); }
    template<is_attribute Attr>
    requires std::invocable<typename Attr::value_type::size>
    constexpr std::size_t attribute_size(const Attr& attr){ return attr ? attr.value().size() : 0; }

    template<typename Model, typename Attribute, typename Aggregator>
    struct attribute_aggregator;

    template<typename T>
    concept is_attribute_aggregator = std::derived_from<T, attribute_aggregator<typename T::model_type, typename T::attribute_type, typename T::aggregator_type>>;

    template<std::derived_from<adaptor> Adaptor, is_attribute Attr>
    requires false
    [[nodiscard]] active_record::string to_string(const Attr& attr, active_record::string&& buff = active_record::string{});

    template<std::derived_from<adaptor> Adaptor, is_attribute Attr>
    requires false
    void from_string(Attr& attr, const active_record::string_view str);
}

template<active_record::is_attribute Attr>
struct std::hash<Attr> {
    std::hash<std::optional<typename Attr::value_type>> inner_hash;
    std::size_t operator()(const Attr& key) const {
        return inner_hash(static_cast<std::optional<typename Attr::value_type>>(key));
    }
};