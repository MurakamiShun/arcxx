#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include "query.hpp"

namespace active_record {
    template<typename Derived>
    struct model {
        struct schema {
            template<std::derived_from<adaptor> Adaptor>
            [[nodiscard]] static active_record::string to_sql(bool abort_if_exist = true);
        };

        static constexpr bool has_table_name() noexcept { return requires {Derived::table_name;}; }

        [[nodiscard]] static constexpr auto column_names() noexcept {
            return std::apply(
                []<Attribute... Attrs>([[maybe_unused]]Attrs...) constexpr { return std::array<const active_record::string_view, sizeof...(Attrs)>{(Attrs::column_name)...}; },
                Derived{}.attributes_as_tuple()
            );
        }

        [[nodiscard]] auto attributes_as_tuple() noexcept {
            using namespace tuptup::type_placeholders;
            auto attributes_tuple = tuptup::struct_binder<Derived>{}(*reinterpret_cast<Derived*>(this));
            return tuptup::tuple_filter<is_attribute<defer<std::remove_reference<_1>>>>(attributes_tuple);
        }
        [[nodiscard]] auto attributes_as_tuple() const noexcept {
            using namespace tuptup::type_placeholders;
            const auto attributes_tuple = tuptup::struct_binder<Derived>{}(*reinterpret_cast<const Derived*>(this));
            return tuptup::tuple_filter<is_attribute<defer<std::remove_reference<_1>>>>(attributes_tuple);
        }

        [[nodiscard]] static auto insert(const Derived& model);
        [[nodiscard]] static auto insert(Derived&& model);

        [[nodiscard]] static query_relation<std::vector<Derived>, std::tuple<>> all();

        template<Attribute... Attrs>
        [[nodiscard]] static query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<>> select();

        template<AttributeAggregator... Aggregators>
        [[nodiscard]] static query_relation<std::tuple<typename Aggregators::attribute_type...>, std::tuple<>> select();

        template<Attribute Attr>
        [[nodiscard]] static query_relation<std::vector<Attr>, std::tuple<>> pluck();
        template<AttributeAggregator Aggregator>
        [[nodiscard]] static query_relation<typename Aggregator::attribute_type, std::tuple<>> pluck();

        // delete is identifier word
        template<Attribute Attr>
        [[nodiscard]] static query_relation<bool, std::tuple<Attr>> destroy(const Attr&&);
        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] static query_relation<bool, SrcBindAttrs> destroy(query_condition<SrcBindAttrs>&&);

        template<Attribute Attr>
        [[nodiscard]] static query_relation<std::vector<Derived>, std::tuple<Attr>> where(const Attr&);
        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] static query_relation<std::vector<Derived>, SrcBindAttrs> where(query_condition<SrcBindAttrs>&&);

        [[nodiscard]] static query_relation<std::vector<Derived>, std::tuple<>> limit(const std::size_t);

        template<Attribute Attr>
        [[nodiscard]] static query_relation<std::vector<Derived>, std::tuple<>> order_by(const active_record::order = active_record::order::asc);

        template<typename ReferModel>
        requires std::derived_from<ReferModel, model<ReferModel>>
        [[nodiscard]] static query_relation<std::vector<Derived>, std::tuple<>> join();

        template<typename ReferModel>
        requires std::derived_from<ReferModel, model<ReferModel>>
        [[nodiscard]] static query_relation<std::vector<Derived>, std::tuple<>> left_join();

        template<Attribute Attr>
        [[nodiscard]] static query_relation<std::unordered_map<Attr, std::tuple<>>, std::tuple<>> group_by();

        [[nodiscard]] static query_relation<std::size_t, std::tuple<>> count();

        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] static query_relation<typename Attr::sum::attribute_type, std::tuple<>> sum();

        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] static query_relation<typename Attr::avg::attribute_type, std::tuple<>> avg();

        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] static query_relation<typename Attr::max::attribute_type, std::tuple<>> max();

        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] static query_relation<typename Attr::min::attribute_type, std::tuple<>> min();
    };

    template<typename T>
    concept is_model = requires {
        std::derived_from<T, model<T>>;
        requires T::has_table_name();
    };
}