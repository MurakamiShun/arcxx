#pragma once
#include "utils.hpp"
#include "adaptor.hpp"

namespace active_record {
    template<typename Model, typename Attribute, typename Type>
    struct attribute;

    template<typename Model, typename Attribute, typename Type>
    class attribute_common;

    template<typename T>
    concept Attribute = requires {
        std::derived_from<T, attribute_common<typename T::model_type, typename T::attribute_type, typename T::value_type>>;
        T::has_column_name;
    };
    template<typename T>
    struct is_attribute{ static constexpr bool value = Attribute<T>; };

    template<Attribute Attr>
    constexpr std::size_t attribute_size([[maybe_unused]] const Attr&){ return sizeof(typename Attr::value_type); }
    template<Attribute Attr>
    requires std::invocable<typename Attr::value_type::size>
    constexpr std::size_t attribute_size(const Attr& attr){ return attr ? attr.value().size() : 0; }

    template<typename Model, typename Attribute, typename Aggregator>
    struct attribute_aggregator;

    template<typename T>
    concept AttributeAggregator = std::derived_from<T, attribute_aggregator<typename T::model_type, typename T::attribute_type, typename T::aggregator_type>>;

    template<std::derived_from<adaptor> Adaptor, Attribute Attr>
    requires false
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr);

    template<std::derived_from<adaptor> Adaptor, Attribute Attr>
    requires false
    void from_string(Attr& attr, const active_record::string_view str);
}

template<active_record::Attribute Attr>
struct std::hash<Attr> {
    std::hash<std::optional<typename Attr::value_type>> inner_hash;
    std::size_t operator()(const Attr& key) const {
        return inner_hash(static_cast<std::optional<typename Attr::value_type>>(key));
    }
};