#pragma once
#include "query_relation_impl/result_vector.hpp"
#include "query_relation_impl/result_scalar.hpp"
#include "query_relation_impl/result_map.hpp"

namespace active_record {
    /*
     * raw query implementation
     */
    template<typename Result, Attribute... Attrs>
    query_relation<Result, std::tuple<const Attrs*...>> raw_query(const active_record::string_view query_str, Attrs... attrs) {
        query_relation<Result, std::tuple<const Attrs*...>> ret;
        ret.operation = query_operation::unspecified;
        ret.query_op_arg.push_back(active_record::string{ query_str });

        if constexpr (sizeof...(Attrs) > 0){
            std::apply(
                [&ret]<typename Attr>(Attr& attr){ ret.temporary_attrs.push_back(std::move(attr)); },
                std::make_tuple(std::ref(attrs)...)
            );

            detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        }

        return ret;
    }

    template<Tuple BindAttrs>
    template<std::derived_from<adaptor> Adaptor>
    struct query_relation_common<BindAttrs>::sob_to_string_impl {
        struct visitor_impl;
        visitor_impl visitor;

        struct lazy_string_view {
            const active_record::string& str;
            const active_record::string::difference_type begin; // from begin(str)
            const active_record::string::difference_type end; // from begin(str)
            operator active_record::string_view() const {
                return active_record::string_view{ str.begin() + begin, str.begin() + end };
            }
        };

        lazy_string_view to_string(const std::vector<str_or_bind>& sobs) {
            const auto begin = std::distance(visitor.buff.cbegin(), visitor.buff.cend());
            for(const auto& sob : sobs) {
                std::visit(visitor, sob);
            }
            return lazy_string_view {
                .str = visitor.buff,
                .begin = begin,
                .end = std::distance(visitor.buff.cbegin(), visitor.buff.cend())
            };
        }
    };

    template<Tuple BindAttrs>
    template<std::derived_from<adaptor> Adaptor>
    struct query_relation_common<BindAttrs>::sob_to_string_impl<Adaptor>::visitor_impl {
        const std::array<active_record::string, std::tuple_size_v<BindAttrs>> attr_strings;
        active_record::string buff;

        visitor_impl(const BindAttrs& ba) :
            attr_strings([&ba](){
                if constexpr (Adaptor::bindable){
                    std::array<active_record::string, std::tuple_size_v<BindAttrs>> ret;
                    for(std::size_t i = 0; i < std::tuple_size_v<BindAttrs>; ++i){
                        ret[i] = Adaptor::bind_variable_str(i);
                    }
                    return ret;
                }
                else {
                    return std::apply([]<typename... Attrs>(const Attrs*... attrs) {
                        return std::array<active_record::string, std::tuple_size_v<BindAttrs>>{ active_record::to_string<Adaptor>(*attrs)... };
                    }, ba);
                }
            }()){
            const auto bit_ceil = [](std::size_t v) -> std::size_t{
                if (v <= 4) return 4; // minimum reserve size
                return std::bit_ceil(v);
            };
            buff.reserve(bit_ceil(std::transform_reduce(
                attr_strings.begin(), attr_strings.end(), static_cast<std::size_t>(0),
                [](auto acc, const auto len){ return acc += len; },
                [](const auto& str){ return str.length(); }
            )) * 2);
        }

        void operator()(const active_record::string& str) {
            buff += str;
        }
        void operator()(const std::size_t idx) {
            buff += attr_strings[idx];
        }
    };
}