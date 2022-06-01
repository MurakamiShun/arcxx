#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
namespace active_record {
    template<typename Derived>
    inline auto model<Derived>::insert(const Derived& model) {
        using namespace tuptup::type_placeholders;
        using bindattr_t = tuptup::apply_type_t<std::remove_cvref<_1>, decltype(model.attributes_as_tuple())>;
        query_relation<void, bindattr_t> ret{ query_operation::insert };
        // get attribute copy from model
        ret.bind_attrs = model.attributes_as_tuple();
        ret.tables.push_back(detail::insert_column_names_to_string<Derived>());
        // insert values
        ret.op_args.reserve(2 + std::tuple_size_v<decltype(model.attributes_as_tuple())> * 2);
        ret.op_args.push_back("(");
        for(std::size_t i = 0; i < std::tuple_size_v<decltype(model.attributes_as_tuple())>; ++i){
            if (i != 0) ret.op_args.push_back(",");
            ret.op_args.push_back(i);
        }
        ret.op_args.push_back(")");
        return ret;
    }

    template<typename Derived>
    inline auto model<Derived>::insert(Derived&& model) {
        using namespace tuptup::type_placeholders;
        using bindattr_t = tuptup::apply_type_t<std::remove_cvref<_1>, decltype(model.attributes_as_tuple())>;
        query_relation<void, bindattr_t> ret{ query_operation::insert };
        // get attribute from model
        ret.bind_attrs = std::move(model.attributes_as_tuple());
        ret.tables.push_back(detail::insert_column_names_to_string<Derived>());
        // insert values
        ret.op_args.reserve(2 + std::tuple_size_v<decltype(model.attributes_as_tuple())> * 2);
        ret.op_args.push_back("(");
        for(std::size_t i = 0; i < std::tuple_size_v<decltype(model.attributes_as_tuple())>; ++i){
            if (i != 0) ret.op_args.push_back(",");
            ret.op_args.push_back(i);
        }
        ret.op_args.push_back(")");
        return ret;
    }

    template<typename Derived>
    inline auto model<Derived>::all() {
        query_relation<std::vector<Derived>, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));

        return ret;
    }

    template<typename Derived>
    template<is_attribute... Attrs>
    inline auto model<Derived>::select() {
        query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<Attrs...>());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));
        return ret;
    }

    template<typename Derived>
    template<is_attribute_aggregator... Aggregators>
    inline auto model<Derived>::select() {
        query_relation<std::tuple<typename Aggregators::attribute_type...>, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<Aggregators...>());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));
        return ret;
    }
    template<typename Derived>
    template<is_attribute Attr>
    inline auto model<Derived>::pluck() {
        query_relation<std::vector<Attr>, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<Attr>());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));
        return ret;
    }
    template<typename Derived>
    template<is_attribute_aggregator Aggregator>
    inline auto model<Derived>::pluck(){
        query_relation<typename Aggregator::attribute_type, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<Aggregator>());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));
        return ret;
    }
    template<typename Derived>
    template<is_attribute Attr>
    inline auto model<Derived>::destroy(const Attr&& attr){
        return destroy(Attr::cmp == attr);
    }

    template<typename Derived>
    template<specialized_from<std::tuple> SrcBindAttrs>
    inline auto model<Derived>::destroy(query_condition<SrcBindAttrs>&& cond) {
        query_relation<void, SrcBindAttrs> ret{ query_operation::destroy };
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));
        ret.conditions = std::move(cond.condition);
        ret.bind_attrs = std::move(cond.bind_attrs);
        return ret;
    }

    template<typename Derived>
    template<is_attribute Attr>
    inline auto model<Derived>::where(const Attr& attr) {
        return where(Attr::cmp == attr);
    }

    template<typename Derived>
    template<specialized_from<std::tuple> SrcBindAttrs>
    inline auto model<Derived>::where(query_condition<SrcBindAttrs>&& cond) {
        query_relation<std::vector<Derived>, SrcBindAttrs> ret{ query_operation::condition };
        ret.op_args.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));
        ret.conditions = std::move(cond.condition);
        ret.bind_attrs = std::move(cond.bind_attrs);

        return ret;
    }

    template<typename Derived>
    inline auto model<Derived>::limit(const std::size_t lim) {
        query_relation<std::vector<Derived>, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));
        ret.options.push_back(concat_strings("LIMIT ", std::to_string(lim)));

        return ret;
    }

    template<typename Derived>
    template<is_attribute Attr>
    inline auto model<Derived>::order_by(const active_record::order order) {
        query_relation<std::vector<Derived>, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));

        ret.options.push_back(concat_strings(
            "ORDER BY ", detail::column_full_names_to_string<Attr>(),
            (order == active_record::order::asc ? " ASC" : " DESC" )
        ));

        return ret;
    }

    namespace detail {
        template<is_model Referis_model>
        struct get_reference_attr{
            template<is_attribute Attr>
            requires std::same_as<typename Attr::foreign_key_type::model_type, Referis_model>
            auto operator()([[maybe_unused]]std::tuple<Attr&>) {
                return Attr{};
            }
            template<is_attribute Attr>
            auto operator()([[maybe_unused]]std::tuple<Attr&>) {
                return std::false_type{};
            }
            template<is_attribute Head, is_attribute... Tail>
            auto operator()([[maybe_unused]]std::tuple<Head&, Tail&...>) {
                using head_type = std::invoke_result_t<get_reference_attr<Referis_model>, std::tuple<Head&>>;
                if constexpr(std::is_same_v<head_type, std::false_type>) {
                    using tail_type = std::invoke_result_t<get_reference_attr<Referis_model>, std::tuple<Tail&...>>;
                    return tail_type{};
                }
                else {
                    return head_type{};
                }
            }
        };
    }

    template<typename Derived>
    template<typename Referis_model>
    requires std::derived_from<Referis_model, model<Referis_model>>
    inline auto model<Derived>::join() {
        query_relation<std::vector<Derived>, std::tuple<>> ret{ query_operation::select };

        using ReferenceAttribute = std::invoke_result_t<detail::get_reference_attr<Referis_model>, decltype(Derived{}.attributes_as_tuple())>;

        static_assert(!std::is_same_v<ReferenceAttribute, std::false_type>, "Derived model does not have reference to given model");

        ret.op_args.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.tables.push_back(concat_strings(
            "\"", Derived::table_name, "\" INNER JOIN \"",
            ReferenceAttribute::foreign_key_type::model_type::table_name,
            "\" ON ", detail::column_full_names_to_string<typename ReferenceAttribute::foreign_key_type>(),
            " = ", detail::column_full_names_to_string<ReferenceAttribute>()
        ));

        return ret;
    }

    template<typename Derived>
    template<typename Referis_model>
    requires std::derived_from<Referis_model, model<Referis_model>>
    inline auto model<Derived>::left_join() {
        query_relation<std::vector<Derived>, std::tuple<>> ret{ query_operation::select };

        using ReferenceAttribute = std::invoke_result_t<detail::get_reference_attr<Referis_model>, decltype(Derived{}.attributes_as_tuple())>;

        static_assert(!std::is_same_v<ReferenceAttribute, std::false_type>, "Derived model does not have reference to given model");

        ret.op_args.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.tables.push_back(concat_strings(
            "\"", Derived::table_name, "\" LEFT OUTER JOIN \"",
            ReferenceAttribute::foreign_key_type::model_type::table_name,
            "\" ON ", detail::column_full_names_to_string<typename ReferenceAttribute::foreign_key_type>(),
            " = ", detail::column_full_names_to_string<ReferenceAttribute>()
        ));

        return ret;
    }

    template<typename Derived>
    template<is_attribute Attr>
    inline auto model<Derived>::group_by() {
        query_relation<std::unordered_map<Attr, std::tuple<>>, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back(detail::column_full_names_to_string<Attr>());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));
        ret.options.push_back(concat_strings("GROUP BY", detail::column_full_names_to_string<Attr>()));

        return ret;
    }


    template<typename Derived>
    inline auto model<Derived>::count() {
        query_relation<std::size_t, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back("count(*)");
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));

        return ret;
    }

    template<typename Derived>
    template<is_attribute Attr>
    requires requires{ typename Attr::sum; }
    inline auto model<Derived>::sum(){
        query_relation<typename Attr::sum::attribute_type, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back(Attr::sum::column_full_name());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));

        return ret;
    }

    template<typename Derived>
    template<is_attribute Attr>
    requires requires{ typename Attr::avg; }
    inline auto model<Derived>::avg(){
        query_relation<typename Attr::avg::attribute_type, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back(Attr::avg::column_full_name());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));

        return ret;
    }

    template<typename Derived>
    template<is_attribute Attr>
    requires requires{ typename Attr::max; }
    inline auto model<Derived>::max(){
        query_relation<typename Attr::max::attribute_type, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back(Attr::max::column_full_name());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));

        return ret;
    }

    template<typename Derived>
    template<is_attribute Attr>
    requires requires{ typename Attr::min; }
    inline auto model<Derived>::min(){
        query_relation<typename Attr::min::attribute_type, std::tuple<>> ret{ query_operation::select };
        ret.op_args.push_back(Attr::min::column_full_name());
        ret.tables.push_back(concat_strings("\"", Derived::table_name, "\""));

        return ret;
    }

    namespace detail{
        template<typename Derived, is_connector Connector>
        inline auto schema_to_sql(bool abort){
            const auto column_definitions = std::apply(
                []<typename... Attrs>(const Attrs...){
                    return std::array<active_record::string, sizeof...(Attrs)>{(Connector::template column_definition<Attrs>())...};
                },
                Derived{}.attributes_as_tuple()
            );

            active_record::string col_defs;
            active_record::string_view delimiter = "";
            for(const auto& col_def : column_definitions){
                col_defs += delimiter;
                col_defs += col_def;
                delimiter = ",";
            }

            return concat_strings(
                "CREATE TABLE ", abort ? "" : "IF NOT EXISTS ",
                Derived::table_name, "(", col_defs, ");"
            );
        }
    }

    template<typename Derived>
    template<is_connector Connector>
    inline active_record::string model<Derived>::schema::to_sql(decltype(abort_if_exists)) {
        return detail::schema_to_sql<Derived, Connector>(true);
    }
    template<typename Derived>
    template<is_connector Connector>
    inline active_record::string model<Derived>::schema::to_sql() {
        return detail::schema_to_sql<Derived, Connector>(false);
    }
}