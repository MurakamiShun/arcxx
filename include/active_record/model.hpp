#pragma once
#include <array>
#include <utility>
#include <vector>
#include <algorithm>
#include "query.hpp"
#include "attribute.hpp"

namespace active_record {
    template<typename T>
    concept WhereArgs = Attribute<T> || std::same_as<T, query_condition>;

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
        
    public:
        static constexpr bool has_table_name = has_table_name_impl::value;
        static constexpr bool has_attributes = has_attributes_impl::value;
        static constexpr auto column_names() noexcept {
            return std::apply(
                []<typename... Attrs>(Attrs...){ return std::array<const active_record::string_view, sizeof...(Attrs)>{(Attrs::column_name)...}; },
                Derived{}.attributes
            );
        }

        constexpr auto get_attribute_strings() const {
            return std::apply(
                []<typename... Attrs>(const Attrs&... attrs){ return std::array<const active_record::string, sizeof...(Attrs)>{attrs.to_string()...}; },
                dynamic_cast<const Derived*>(this)->attributes
            );
        }

        auto get_attribute_string_convertors() {
            return std::apply(
                []<typename... Attrs>(Attrs&... attrs){ return std::array<attribute_string_convertor* const, sizeof...(Attrs)>{&attrs...}; },
                dynamic_cast<Derived*>(this)->attributes
            );
        }
        const auto get_attribute_string_convertors() const {
            return std::apply(
                []<typename... Attrs>(const Attrs&... attrs){ return std::array<const attribute_string_convertor* const, sizeof...(Attrs)>{&attrs...}; },
                dynamic_cast<const Derived*>(this)->attributes
            );
        }
        attribute_string_convertor& operator[](const active_record::string_view col_name) {
            static const auto names = column_names();
            const auto col_idx = std::distance(names.begin(), std::find(names.begin(), names.end(), col_name));
            return *(get_attribute_string_convertors()[col_idx]);
        }
        const attribute_string_convertor& operator[](const active_record::string_view col_name) const {
            static const auto names = column_names();
            const auto col_idx = std::distance(names.begin(), std::find(names.begin(), names.end(), col_name));
            return *(get_attribute_string_convertors()[col_idx]);
        }
        
        template<typename Adaptor>
        static query_relation<bool> table_definition();

        constexpr virtual ~model() {}

        /* 
         * Implementations are query_impl/model_queries.hpp
         */
        template<Container C>
        static constexpr query_relation<bool> insert(const C&);        
        template<std::same_as<Derived>... Models>
        static constexpr query_relation<bool> insert(const Models&...);

        static constexpr query_relation<std::vector<Derived>> all();

        template<Attribute... Attrs>
        static constexpr query_relation<std::vector<std::tuple<Attrs...>>> select(const Attrs...);

        template<Attribute Attr>
        static constexpr query_relation<std::vector<Attr>> pluck(const Attr);
        
        template<WhereArgs... Attrs>
        static constexpr query_relation<std::vector<Derived>> where(const Attrs...);        

        query_relation<bool> destroy();
        query_relation<bool> save();
    };

    template<typename T>
    concept Model = std::derived_from<T, model<T>>;
}