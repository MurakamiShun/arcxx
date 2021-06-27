#pragma once
#include <optional>
#include <utility>
#include <typeinfo>
#include "utils.hpp"
#include "adaptor.hpp"

namespace active_record {
    template<typename Model, typename Attribute, typename Type>
    struct attribute;

    struct attribute_string_convertor {
        virtual active_record::string to_string() const = 0;
        virtual void from_string(const active_record::string&) = 0;
        virtual const void* value_ptr() const = 0;
    };

    template<typename Model, typename Attribute, typename Type>
    class attribute_common;

    template<typename T>
    concept Attribute = std::derived_from<T, attribute_common<typename T::model_type, typename T::attribute_type, typename T::value_type>>;

    template<Attribute Attr>
    requires false
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr);
    template<Attribute Attr>
    requires false
    void from_string(Attr& attr, const active_record::string_view str);
}