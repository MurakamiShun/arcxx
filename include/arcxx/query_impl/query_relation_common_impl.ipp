#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
namespace arcxx {
    template<typename Result, typename... Args>
    requires ((is_attribute<Args> || std::convertible_to<Args, arcxx::string_view>) && ...)
    auto raw_query(Args&&... args){
        using namespace tuptup::type_placeholders;
        using bind_attrs_t = tuptup::tuple_filter_t<is_attribute_type<_1>, std::tuple<std::remove_cvref_t<Args>...>>;
        
        query_relation<Result, bind_attrs_t> ret{ query_operation::unspecified };
        std::size_t index = 0;
        ret.op_args.reserve(sizeof...(Args));
        ret.op_args = {
            [&index]<typename Arg>(const Arg& arg) -> typename std::type_identity_t<decltype(ret)>::str_or_bind {
                if constexpr (is_attribute<Arg>) {
                    ++index;
                    return index;
                }
                else return arcxx::string{ arg };
            }(args)...
        };
        ret.bind_attrs = tuptup::tuple_filter<is_attribute_type<_1>>(std::make_tuple(std::forward<Args>(args)...));

        return ret;
    }

    template<specialized_from<std::tuple> BindAttrs>
    template<is_connector Connector>
    [[nodiscard]] const arcxx::string query_relation_common<BindAttrs>::to_sql() const {
        sob_to_string_impl<Connector> convertor{ bind_attrs };
        if (operation == query_operation::select) {
            return concat_strings("SELECT ", convertor.to_string(op_args),
                " FROM ", convertor.to_string(tables),
                conditions.empty() ? "" : concat_strings(" WHERE ", convertor.to_string(conditions)),
                " ", convertor.to_string(options), ";"
            );
        }
        else if (operation == query_operation::insert) {
            return concat_strings("INSERT INTO ", convertor.to_string(tables),
                " VALUES ", convertor.to_string(op_args), ";"
            );
        }
        else if (operation == query_operation::destroy) {
            return concat_strings("DELETE FROM ", convertor.to_string(tables),
                conditions.empty() ? "" : concat_strings(" WHERE ", convertor.to_string(conditions)), ";"
            );
        }
        else if (operation == query_operation::update) {
            return concat_strings("UPDATE ", convertor.to_string(tables),
                " SET ", convertor.to_string(op_args),
                conditions.empty() ? "": concat_strings(" WHERE ", convertor.to_string(conditions)), ";"
            );
        }
        else if (operation == query_operation::condition) {
            return concat_strings("SELECT ", convertor.to_string(op_args),
                " FROM ", convertor.to_string(tables),
                " WHERE ", convertor.to_string(conditions),
                convertor.to_string(options), ";"
            );
        }
        else {
            return concat_strings(convertor.to_string(op_args), ";");
        }
    }

    template<specialized_from<std::tuple> BindAttrs>
    template<is_connector Connector>
    struct query_relation_common<BindAttrs>::sob_to_string_impl {
        const BindAttrs& bind_attrs;
        const std::array<std::function<arcxx::string(arcxx::string&&)>, std::tuple_size_v<BindAttrs>> to_string_func;

        sob_to_string_impl(const BindAttrs& attrs) :
            bind_attrs(attrs),
            to_string_func([&attrs]<std::size_t... I>(std::index_sequence<I...>){
                if constexpr(Connector::bindable){
                    return std::array<std::function<arcxx::string(arcxx::string&&)>, std::tuple_size_v<BindAttrs>>{};
                }
                else{
                    const auto lambda_generator = [&attrs]<std::size_t N>(std::integral_constant<std::size_t, N>) -> std::function<arcxx::string(arcxx::string&&)>{
                        return [&attrs](arcxx::string&& buff){
                            return arcxx::to_string<Connector>(std::get<N>(attrs), std::move(buff));
                        };
                    };
                    return std::array<std::function<arcxx::string(arcxx::string&&)>, std::tuple_size_v<BindAttrs>>{
                        lambda_generator(std::integral_constant<std::size_t, I>{})...
                    };
                }
            }(std::make_index_sequence<std::tuple_size_v<BindAttrs>>{})){
        }

        arcxx::string to_string(const std::vector<str_or_bind>& sobs) const {
            arcxx::string buff;
            buff.reserve(std::bit_ceil(sobs.size()*2));

            if constexpr (Connector::bindable){
                for(const auto& sob : sobs) {
                    visit_by_lambda(sob,
                        [&buff](const arcxx::string& str) { buff += str; },
                        [&buff](const std::size_t idx) { buff += Connector::bind_variable_str(idx); }
                    );
                }
            }
            else{
                for(const auto& sob : sobs) {
                    visit_by_lambda(sob,
                        [&buff](const arcxx::string& str) { buff += str; },
                        [&buff, this](const std::size_t idx) { buff = this->to_string_func[idx](std::move(buff)); }
                    );
                }
            }
            return buff;
        }
    };
}