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
}