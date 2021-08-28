#pragma once
#include "../model.hpp"
#include "../query_impl/query_relation_impl.hpp"

namespace active_record {
    namespace detail {
        template<typename... T>
        auto reference_tuple_to_ptr_tuple([[maybe_unused]]std::tuple<T&...>) -> std::tuple<const T*...>;
    }

    template<typename Derived>
    inline auto model<Derived>::insert(const Derived& model) {
        query_relation<bool, decltype(detail::reference_tuple_to_ptr_tuple(std::declval<Derived>().attributes))> ret;
        ret.operation = query_operation::insert;
        // get attribute pointers in model
        ret.bind_attrs = std::apply(
            []<Attribute... Attrs>(const Attrs&... attrs){ return std::make_tuple(&attrs...); },
            model.attributes
        );
        ret.query_table.push_back(insert_column_names_to_string());
        // insert values
        ret.query_op_arg.push_back("(");
        for(std::size_t i = 0; i < std::tuple_size_v<decltype(model.attributes)>; ++i){
            if (i != 0) ret.query_op_arg.push_back(",");
            ret.query_op_arg.push_back(i);
        }
        ret.query_op_arg.push_back(")");
        return ret;
    }

    template<typename Derived>
    inline auto model<Derived>::insert(Derived&& model) {
        query_relation<bool, decltype(detail::reference_tuple_to_ptr_tuple(std::declval<Derived>().attributes))> ret;
        ret.operation = query_operation::insert;
        // copy to temporary
        ret.temporary_attrs.push_back(std::move(model));
        // get attribute pointers in temporary model
        ret.bind_attrs = std::apply(
            []<Attribute... Attrs>(const Attrs&... attrs){ return std::make_tuple(&attrs...); },
            std::any_cast<Derived>(ret.temporary_attrs[0]).attributes
        );
        ret.query_table.push_back(insert_column_names_to_string());
        // insert values
        ret.query_op_arg.push_back("(");
        for(auto i = 0; i < std::tuple_size_v<decltype(model.attributes)>; ++i){
            if (i != 0) ret.query_op_arg.push_back(",");
            ret.query_op_arg.push_back(i);
        }
        ret.query_op_arg.push_back(")");
        return ret;
    }

    template<typename Derived>
    inline query_relation<std::vector<Derived>, std::tuple<>> model<Derived>::all() {
        query_relation<std::vector<Derived>, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));

        return ret;
    }

    template<typename Derived>
    template<Attribute... Attrs>
    inline query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<>> model<Derived>::select() {
        query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<Attrs...>());
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));
        return ret;
    }

    template<typename Derived>
    template<AttributeAggregator... Aggregators>
    inline query_relation<std::tuple<typename Aggregators::attribute_type...>, std::tuple<>> model<Derived>::select() {
        query_relation<std::tuple<typename Aggregators::attribute_type...>, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<Aggregators...>());
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));
        return ret;
    }
    template<typename Derived>
    template<Attribute Attr>
    inline query_relation<std::vector<Attr>, std::tuple<>> model<Derived>::pluck() {
        query_relation<std::vector<Attr>, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<Attr>());
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));
        return ret;
    }

    template<typename Derived>
    template<Attribute Attr>
    inline query_relation<bool, std::tuple<const Attr*>> model<Derived>::destroy(const Attr&& attr){
        return destroy(Attr::cmp == attr);
    }

    template<typename Derived>
    template<Tuple SrcBindAttrs>
    inline query_relation<bool, SrcBindAttrs> model<Derived>::destroy(query_condition<SrcBindAttrs>&& cond) {
        query_relation<bool, SrcBindAttrs> ret;

        ret.operation = query_operation::destroy;
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));
        ret.query_condition = std::move(cond.condition);
        ret.temporary_attrs = std::move(cond.temporary_attrs);
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);
        return ret;
    }

    template<typename Derived>
    template<Attribute Attr>
    inline query_relation<std::vector<Derived>, std::tuple<const Attr*>> model<Derived>::where(const Attr& attr) {
        return where(Attr::cmp == attr);
    }

    template<typename Derived>
    template<Tuple SrcBindAttrs>
    inline query_relation<std::vector<Derived>, SrcBindAttrs> model<Derived>::where(query_condition<SrcBindAttrs>&& cond) {
        query_relation<std::vector<Derived>, SrcBindAttrs> ret;

        ret.operation = query_operation::condition;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));
        ret.query_condition = std::move(cond.condition);
        ret.temporary_attrs = std::move(cond.temporary_attrs);
        detail::set_bind_attrs_ptr<0>(ret.bind_attrs, ret.temporary_attrs);

        return ret;
    }

    template<typename Derived>
    inline query_relation<std::vector<Derived>, std::tuple<>> model<Derived>::limit(const std::size_t lim) {
        query_relation<std::vector<Derived>, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));
        ret.query_options.push_back(concat_strings("LIMIT ", std::to_string(lim)));

        return ret;
    }

    template<typename Derived>
    template<Attribute Attr>
    inline query_relation<std::vector<Derived>, std::tuple<>> model<Derived>::order_by(const active_record::order order) {
        query_relation<std::vector<Derived>, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));

        ret.query_options.push_back(concat_strings(
            "ORDER BY ", detail::column_full_names_to_string<Attr>(),
            (order == active_record::order::asc ? " ASC" : " DESC" )
        ));

        return ret;
    }

    namespace detail {
        template<Model ReferModel>
        struct get_reference_attr{
            template<Attribute Attr>
            requires std::same_as<typename Attr::foreign_key_type::model_type, ReferModel>
            auto operator()([[maybe_unused]]std::tuple<Attr&>) {
                return Attr{};
            }
            template<Attribute Attr>
            auto operator()([[maybe_unused]]std::tuple<Attr&>) {
                return std::false_type{};
            }
            template<Attribute Head, Attribute... Tail>
            auto operator()([[maybe_unused]]std::tuple<Head&, Tail&...>) {
                using head_type = std::invoke_result_t<get_reference_attr<ReferModel>, std::tuple<Head&>>;
                if constexpr(std::is_same_v<head_type, std::false_type>) {
                    using tail_type = std::invoke_result_t<get_reference_attr<ReferModel>, std::tuple<Tail&...>>;
                    return tail_type{};
                }
                else {
                    return head_type{};
                }
            }
        };
    }

    template<typename Derived>
    template<typename ReferModel>
    requires std::derived_from<ReferModel, model<ReferModel>>
    inline query_relation<std::vector<Derived>, std::tuple<>> model<Derived>::join() {
        query_relation<std::vector<Derived>, std::tuple<>> ret;

        using ReferenceAttribute = std::invoke_result_t<detail::get_reference_attr<ReferModel>, decltype(Derived{}.attributes)>;

        static_assert(!std::is_same_v<ReferenceAttribute, std::false_type>, "Derived model does not have reference to given model");

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(concat_strings(
            "\"", Derived::table_name, "\" INNER JOIN \"",
            ReferenceAttribute::foreign_key_type::model_type::table_name,
            "\" ON ", detail::column_full_names_to_string<typename ReferenceAttribute::foreign_key_type>(),
            " = ", detail::column_full_names_to_string<ReferenceAttribute>()
        ));

        return ret;
    }

    template<typename Derived>
    template<typename ReferModel>
    requires std::derived_from<ReferModel, model<ReferModel>>
    inline query_relation<std::vector<Derived>, std::tuple<>> model<Derived>::left_join() {
        query_relation<std::vector<Derived>, std::tuple<>> ret;

        using ReferenceAttribute = std::invoke_result_t<detail::get_reference_attr<ReferModel>, decltype(Derived{}.attributes)>;

        static_assert(!std::is_same_v<ReferenceAttribute, std::false_type>, "Derived model does not have reference to given model");

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::model_column_full_names_to_string<Derived>());
        ret.query_table.push_back(concat_strings(
            "\"", Derived::table_name, "\" LEFT OUTER JOIN \"",
            ReferenceAttribute::foreign_key_type::model_type::table_name,
            "\" ON ", detail::column_full_names_to_string<typename ReferenceAttribute::foreign_key_type>(),
            " = ", detail::column_full_names_to_string<ReferenceAttribute>()
        ));

        return ret;
    }

    template<typename Derived>
    template<Attribute Attr>
    inline query_relation<std::unordered_map<Attr, std::tuple<>>, std::tuple<>> model<Derived>::group_by() {
        query_relation<std::unordered_map<Attr, std::tuple<>>, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(detail::column_full_names_to_string<Attr>());
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));
        ret.query_options.push_back(concat_strings("GROUP BY", detail::column_full_names_to_string<Attr>()));

        return ret;
    }


    template<typename Derived>
    inline query_relation<std::size_t, std::tuple<>> model<Derived>::count() {
        query_relation<std::size_t, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back("count(*)");
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));

        return ret;
    }

    template<typename Derived>
    template<Attribute Attr>
    requires requires{ typename Attr::sum; }
    inline query_relation<typename Attr::sum::attribute_type, std::tuple<>> model<Derived>::sum(){
        query_relation<typename Attr::sum::attribute_type, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(Attr::sum::column_full_name());
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));

        return ret;
    }

    template<typename Derived>
    template<Attribute Attr>
    requires requires{ typename Attr::avg; }
    inline query_relation<typename Attr::avg::attribute_type, std::tuple<>> model<Derived>::avg(){
        query_relation<typename Attr::avg::attribute_type, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(Attr::avg::column_full_name());
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));

        return ret;
    }

    template<typename Derived>
    template<Attribute Attr>
    requires requires{ typename Attr::max; }
    inline query_relation<typename Attr::max::attribute_type, std::tuple<>> model<Derived>::max(){
        query_relation<typename Attr::max::attribute_type, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(Attr::max::column_full_name());
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));

        return ret;
    }

    template<typename Derived>
    template<Attribute Attr>
    requires requires{ typename Attr::min; }
    inline query_relation<typename Attr::min::attribute_type, std::tuple<>> model<Derived>::min(){
        query_relation<typename Attr::min::attribute_type, std::tuple<>> ret;

        ret.operation = query_operation::select;
        ret.query_op_arg.push_back(Attr::min::column_full_name());
        ret.query_table.push_back(concat_strings("\"", Derived::table_name, "\""));

        return ret;
    }

    template<typename Derived>
    template<std::derived_from<adaptor> Adaptor>
    inline active_record::string model<Derived>::schema::to_sql(bool abort_if_exist) {
        const auto column_definitions = std::apply(
            []<typename... Attrs>(const Attrs&...){ return std::array<const active_record::string, sizeof...(Attrs)>{(Adaptor::template column_definition<Attrs>())...}; },
            Derived{}.attributes
        );

        active_record::string col_defs;
        active_record::string_view delimiter = "";
        for(const auto& col_def : column_definitions){
            col_defs += delimiter;
            col_defs += col_def;
            delimiter = ",";
        }

        return concat_strings(
            "CREATE TABLE ", abort_if_exist ? "" : "IF NOT EXISTS ",
            Derived::table_name, "(", col_defs, ");"
        );
    }
}