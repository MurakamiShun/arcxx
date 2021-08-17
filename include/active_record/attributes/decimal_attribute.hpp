#pragma once
#include "attribute_common_impl.hpp"

namespace active_record {
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
        return static_cast<bool>(attr) ? std::to_string(attr.value()) : "null";
    }
    template<std::same_as<common_adaptor> Adaptor, Attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    void from_string(Attr& attr, const active_record::string_view str){
        if(str != "null" && str != "NULL"){
            typename Attr::value_type tmp;
            std::from_chars(&*str.begin(), &*str.end(), tmp);
            attr = tmp;
        }
    }
    
    template<typename Model, typename Attribute, std::floating_point FP>
    struct attribute<Model, Attribute, FP> : attribute_common<Model, Attribute, FP> {
        using attribute_common<Model, Attribute, FP>::attribute_common;

        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        [[nodiscard]] constexpr active_record::string to_string() const {
            return active_record::to_string<Adaptor>(*this);
        }
        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        void from_string(const active_record::string_view str) {
            active_record::from_string<Adaptor>(*this, str);
        }

        template<std::convertible_to<FP> ArgType1, std::convertible_to<FP> ArgType2>
        [[nodiscard]] static auto between(const ArgType1 value1, const ArgType2 value2){
            query_condition<std::tuple<const Attribute*, const Attribute*>> ret;
            ret.temporary_attrs.push_back(static_cast<Attribute>(value1));
            std::get<0>(ret.bind_attrs) = std::any_cast<Attribute>(&(ret.temporary_attrs.back()));
            ret.temporary_attrs.push_back(static_cast<Attribute>(value2));
            std::get<1>(ret.bind_attrs) = std::any_cast<Attribute>(&(ret.temporary_attrs.back()));
            ret.condition.push_back(
                active_record::string{ "\"" } + Model::table_name + "\".\""
                + Attribute::column_name + "\" BETWEEN "
            );
            ret.condition.push_back(0UL);
            ret.condition.push_back(" AND ");
            ret.condition.push_back(1UL);
            return ret;
        }

        struct sum : public attribute_aggregator<Model, Attribute, sum> {
            static constexpr auto aggregation_func = "sum";
        };
        struct avg : public attribute_aggregator<Model, Attribute, avg> {
            static constexpr auto aggregation_func = "avg";
        };
        struct max : public attribute_aggregator<Model, Attribute, max> {
            static constexpr auto aggregation_func = "max";
        };
        struct min : public attribute_aggregator<Model, Attribute, min> {
            static constexpr auto aggregation_func = "min";
        };
    };

    namespace attributes {
        template<typename Model, typename Attribute, std::floating_point FP = float>
        struct decimal : public attribute<Model, Attribute, FP>{
            using attribute<Model, Attribute, FP>::attribute;
        };
    }
}