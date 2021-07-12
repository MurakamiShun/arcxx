#pragma once
#include <array>
#include <utility>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <type_traits>
#include <any>
#include "query.hpp"
#include "attribute.hpp"

namespace active_record {
    struct void_model;

    template<typename... T>
    auto reference_tuple_to_ptr_tuple(std::tuple<T&...>){
        return std::tuple<const T*...>{};
    }

    template<typename Derived>
    class model {
    private:
        struct has_table_name_impl {
            template<typename S>
            static decltype(S::table_name, std::true_type{}) check(S);
            static std::false_type check(...);
            static constexpr bool value = decltype(check(std::declval<Derived>()))::value;
        };
        struct has_attributes_impl {
            template<typename S>
            static decltype(std::declval<S>().attributes, std::true_type{}) check(S);
            static std::false_type check(...);
            static constexpr bool value = decltype(check(std::declval<Derived>()))::value;
        };

        template<typename Model>
        static constexpr active_record::string insert_column_names_to_string() {
            active_record::string table = active_record::string{ "\"" } + active_record::string{ Model::table_name } + "\"(";
            active_record::string delimiter = "";
            const auto column_names = Model::column_names();
            for (const auto& col_name : column_names) {
                table += delimiter + "\"" + active_record::string{ col_name } + "\"";
                delimiter = ",";
            }
            table += ")";
            return table;
        }
        
    public:
        struct schema {
            template<std::derived_from<adaptor> Adaptor>
            static active_record::string to_sql(bool create_if_not_exist = false);
        };

        static constexpr bool has_table_name = has_table_name_impl::value;
        static constexpr bool has_attributes = has_attributes_impl::value;
        static constexpr auto column_names() noexcept {
            return std::apply(
                []<typename... Attrs>(Attrs...){ return std::array<const active_record::string_view, sizeof...(Attrs)>{(Attrs::column_name)...}; },
                Derived{}.attributes
            );
        }

        constexpr virtual ~model() {}

        static auto insert(const Derived& model) {
            query_relation<bool, decltype(reference_tuple_to_ptr_tuple(model.attributes))> ret;
            ret.operation = query_operation::insert;
            // get attribute pointers in model
            ret.bind_attrs = std::apply(
                []<Attribute... Attrs>(const Attrs&... attrs){ return std::make_tuple(&attrs...); },
                model.attributes
            );
            ret.query_table.push_back(insert_column_names_to_string<Derived>());
            // insert values
            ret.query_op_arg.push_back("(");
            for(std::size_t i = 0; i < std::tuple_size_v<decltype(model.attributes)>; ++i){
                if (i != 0) ret.query_op_arg.push_back(",");
                ret.query_op_arg.push_back(i);
            }
            ret.query_op_arg.push_back(")");
            return ret;
        }
        static auto insert(Derived&& model){
            query_relation<bool, decltype(reference_tuple_to_ptr_tuple(model.attributes))> ret;
            ret.operation = query_operation::insert;
            // copy to temporary
            ret.temporary_attrs.push_back(std::move(model));
            // get attribute pointers in temporary model
            ret.bind_attrs = std::apply(
                []<Attribute... Attrs>(const Attrs&... attrs){ return std::make_tuple(&attrs...); },
                std::any_cast<Derived>(ret.temporary_attrs[0]).attributes
            );
            ret.query_table.push_back(insert_column_names_to_string<Derived>());
            // insert values
            ret.query_op_arg.push_back("(");
            for(auto i = 0; i < std::tuple_size_v<decltype(model.attributes)>; ++i){
                if (i != 0) ret.query_op_arg.push_back(",");
                ret.query_op_arg.push_back(i);
            }
            ret.query_op_arg.push_back(")");
            return ret;
        }
        /*
         * Implementations are query_impl/model_queries.hpp
         */
        static query_relation<std::vector<Derived>, std::tuple<>> all();

        template<Attribute... Attrs>
        static query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<>> select(const Attrs...);

        template<Attribute Attr>
        static query_relation<std::vector<Attr>, std::tuple<>> pluck(const Attr);
        
        template<Attribute... Attrs>
        static query_relation<std::vector<Derived>, std::tuple<const Attrs*...>> where(const Attrs...);
    };

    template<typename T>
    concept Model = std::derived_from<T, model<T>>;
}