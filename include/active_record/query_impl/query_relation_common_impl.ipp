#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
namespace active_record {
    template<typename Result, typename... Args>
    requires ((is_attribute<Args> || std::convertible_to<Args, active_record::string_view>) && ...)
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
                else return active_record::string{ arg };
            }(args)...
        };
        ret.bind_attrs = tuptup::tuple_filter<is_attribute_type<_1>>(std::make_tuple(std::forward<Args>(args)...));

        return ret;
    }

    template<specialized_from<std::tuple> BindAttrs>
    template<std::derived_from<adaptor> Adaptor>
    [[nodiscard]] const active_record::string query_relation_common<BindAttrs>::to_sql() const {
        sob_to_string_impl<Adaptor> convertor{ bind_attrs };
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
    template<std::derived_from<adaptor> Adaptor>
    struct query_relation_common<BindAttrs>::sob_to_string_impl {
        const BindAttrs& bind_attrs;
        const std::array<std::function<active_record::string()>, std::tuple_size_v<BindAttrs>> to_string_func;

        sob_to_string_impl(const BindAttrs& attrs) :
            bind_attrs(attrs),
            to_string_func([&attrs]<std::size_t... I>(std::index_sequence<I...>){
                if constexpr(Adaptor::bindable){
                    return std::array<std::function<active_record::string()>, std::tuple_size_v<BindAttrs>>{};
                }
                else{
                    return std::array<std::function<active_record::string()>, std::tuple_size_v<BindAttrs>>{
                        [&attr = std::get<I>(attrs)]{ return active_record::to_string<Adaptor>(attr); }...
                    };
                }
            }(std::make_index_sequence<std::tuple_size_v<BindAttrs>>{})){
        }

        active_record::string to_string(const std::vector<str_or_bind>& sobs) const {
            active_record::string buff;
            buff.reserve(std::bit_ceil(sobs.size()*2));

            if constexpr (Adaptor::bindable){
                for(const auto& sob : sobs) {
                    visit_by_lambda(sob,
                        [&buff](const active_record::string& str) { buff += str; },
                        [&buff](const std::size_t idx) { buff += Adaptor::bind_variable_str(idx); }
                    );
                }
            }
            else{
                for(const auto& sob : sobs) {
                    visit_by_lambda(sob,
                        [&buff](const active_record::string& str) { buff += str; },
                        [&buff, this](const std::size_t idx) { buff += this->to_string_func[idx](); }
                    );
                }
            }
            return buff;
        }
    };
}