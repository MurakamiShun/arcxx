#pragma once
#include "utils.hpp"
#include "adaptor.hpp"

namespace active_record {
    template<typename Model, typename Attribute, typename Type>
    struct attribute;

    template<typename Model, typename Attribute, typename Type>
    class attribute_common;

    template<typename T>
    concept Attribute = std::derived_from<T, attribute_common<typename T::model_type, typename T::attribute_type, typename T::value_type>>;

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