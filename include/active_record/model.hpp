#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include "query.hpp"
#include "attribute.hpp"

namespace active_record {
    template<typename Derived>
    class model {
    private:
        [[nodiscard]] static active_record::string insert_column_names_to_string();
    public:
        struct schema {
            template<std::derived_from<adaptor> Adaptor>
            [[nodiscard]] static active_record::string to_sql(bool abort_if_exist = true);
        };

        static constexpr bool has_table_name = requires {Derived::table_name;};

        [[nodiscard]] static constexpr auto column_names() noexcept;

        [[nodiscard]] auto attributes_as_tuple() noexcept;
        [[nodiscard]] auto attributes_as_tuple() const noexcept;

        [[nodiscard]] static auto insert(const Derived& model);
        [[nodiscard]] static auto insert(Derived&& model);

        [[nodiscard]] static query_relation<std::vector<Derived>, std::tuple<>> all();

        template<Attribute... Attrs>
        [[nodiscard]] static query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<>> select();

        template<AttributeAggregator... Aggregators>
        [[nodiscard]] static query_relation<std::tuple<typename Aggregators::attribute_type...>, std::tuple<>> select();

        template<Attribute Attr>
        [[nodiscard]] static query_relation<std::vector<Attr>, std::tuple<>> pluck();

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
        T::has_table_name;
    };
}

#include "model_impl/model_impl.ipp"