#pragma once
#include "../model.hpp"
#include "../query.hpp"
#include "../attribute.hpp"
#include "../adaptor.hpp"
#include "../utils.hpp"
#include "query_condition.hpp"

namespace active_record {
    template<Tuple BindAttrs>
    struct query_relation_common {
    public:
        using str_or_bind = std::variant<active_record::string, std::size_t>;
    private:
        template<std::derived_from<adaptor> Adaptor>
        struct sob_to_string_impl {
            struct visitor_impl {
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
                    }()){/*
                    buff.reserve(std::transform_reduce(
                        attr_strings.begin(), attr_strings.end(), static_cast<std::size_t>(0),
                        [](auto acc, const auto len){ return acc += len; },
                        [](const auto& str){ return str.length(); }
                    ));*/
                }

                void operator()(const active_record::string& str) {
                    buff += str;
                }
                void operator()(const std::size_t idx) {
                    buff += attr_strings[idx];
                }
            } visitor;

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
    public:        
        query_operation operation;
        std::vector<str_or_bind> query_op_arg;
        std::vector<str_or_bind> query_table;
        std::vector<str_or_bind> query_condition;
        std::vector<str_or_bind> query_options; // order, limit
        
        BindAttrs bind_attrs;
        std::vector<std::any> temporary_attrs;

        [[nodiscard]] static constexpr std::size_t bind_attrs_count() noexcept {
            return std::tuple_size_v<BindAttrs>;
        }

        query_relation_common(){}
        query_relation_common(const BindAttrs attrs) : bind_attrs(attrs){  }

        template<std::derived_from<adaptor> Adaptor = common_adaptor>
        [[nodiscard]] const active_record::string to_sql() const {
            sob_to_string_impl<Adaptor> convertor{ bind_attrs };
            if (operation == query_operation::select) {
                return concat_strings("SELECT ", convertor.to_string(query_op_arg),
                    " FROM ", convertor.to_string(query_table),
                    query_condition.empty() ? "" : concat_strings(" WHERE ", convertor.to_string(query_condition)),
                    " ", convertor.to_string(query_options), ";"
                );
            }
            else if (operation == query_operation::insert) {
                return concat_strings("INSERT INTO ", convertor.to_string(query_table),
                    " VALUES ", convertor.to_string(query_op_arg), ";"
                );
            }
            else if (operation == query_operation::destroy) {
                return concat_strings("DELETE FROM ", convertor.to_string(query_table),
                    query_condition.empty() ? "" : concat_strings(" WHERE ", convertor.to_string(query_condition)), ";"
                );
            }
            else if (operation == query_operation::update) {
                return concat_strings("UPDATE ", convertor.to_string(query_table),
                    " SET ", convertor.to_string(query_op_arg),
                    query_condition.empty() ? "": concat_strings(" WHERE ", convertor.to_string(query_condition)), ";"
                );
            }
            else if (operation == query_operation::condition) {
                return concat_strings("SELECT ", convertor.to_string(query_op_arg),
                    " FROM ", convertor.to_string(query_table),
                    " WHERE ", convertor.to_string(query_condition),
                    convertor.to_string(query_options), ";"
                );
            }
            else {
                return concat_strings(convertor.to_string(query_op_arg), ";");
            }
        }
    };

    template<Tuple BindAttrs>
    struct query_relation<bool, BindAttrs> : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        template<std::derived_from<adaptor> Adaptor>
        auto exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }
    };

    template<typename Result, Tuple BindAttrs>
    struct query_relation : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        template<std::derived_from<adaptor> Adaptor>
        [[nodiscard]] auto exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, active_record::tuple_cat_t<BindAttrs, std::tuple<const Attr*>>> where(const Attr&&) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, active_record::tuple_cat_t<BindAttrs, std::tuple<const Attr*>>> where(const Attr&&) const &;

        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_relation<Result, active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) &&;
        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_relation<Result, active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const&;
    };

    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::vector<typename Result::value_type>>
    struct query_relation<Result, BindAttrs> : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        template<std::derived_from<adaptor> Adaptor>
        [[nodiscard]] auto exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }
        
        template<Attribute... Attrs>
        [[nodiscard]] query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> select() &&;
        template<Attribute... Attrs>
        [[nodiscard]] query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> select() const &;

        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::tuple<typename Attrs::attribute_type...>, BindAttrs> select() &&;
        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::tuple<typename Attrs::attribute_type...>, BindAttrs> select() const &;

        template<Attribute Attr>
        [[nodiscard]] query_relation<std::vector<Attr>, BindAttrs> pluck() &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<std::vector<Attr>, BindAttrs> pluck() const &;

        template<Attribute... Attrs>
        requires Model<typename Result::value_type>
        [[nodiscard]] query_relation<bool, active_record::tuple_cat_t<BindAttrs, std::tuple<const Attrs*...>>> update(const Attrs...);

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, active_record::tuple_cat_t<BindAttrs, std::tuple<const Attr*>>> where(const Attr&) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, active_record::tuple_cat_t<BindAttrs, std::tuple<const Attr*>>> where(const Attr&) const &;

        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_relation<Result, active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) &&;
        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_relation<Result, active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const&;

        [[nodiscard]] query_relation<Result, BindAttrs>& limit(const std::size_t) &&;
        [[nodiscard]] query_relation<Result, BindAttrs> limit(const std::size_t) const &;

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs>& order_by(const active_record::order = active_record::order::asc) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs> order_by(const active_record::order = active_record::order::asc) const &;

        [[nodiscard]] query_relation<std::size_t, BindAttrs> count() &&;
        [[nodiscard]] query_relation<std::size_t, BindAttrs> count() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<typename Attr::sum::attribute_type, BindAttrs> sum() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<typename Attr::sum::attribute_type, BindAttrs> sum() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<typename Attr::avg::attribute_type, BindAttrs> avg() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<typename Attr::avg::attribute_type, BindAttrs> avg() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<typename Attr::max::attribute_type, BindAttrs> max() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<typename Attr::max::attribute_type, BindAttrs> max() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<typename Attr::min::attribute_type, BindAttrs> min() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<typename Attr::min::attribute_type, BindAttrs> min() const&;
    };


    template<typename Result, Tuple BindAttrs>
    requires std::same_as<Result, std::unordered_map<typename Result::key_type, typename Result::mapped_type>>
    struct query_relation<Result, BindAttrs> : public query_relation_common<BindAttrs> {
        using mapped_type = typename Result::mapped_type;

        using query_relation_common<BindAttrs>::query_relation_common;
        
        template<std::derived_from<adaptor> Adaptor>
        [[nodiscard]] auto exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }
        
        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> select() &&;
        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> select() const &;

        template<AttributeAggregator Attr>
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> pluck() &&;
        template<AttributeAggregator Attr>
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::attribute_type>, BindAttrs> pluck() const &;

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, active_record::tuple_cat_t<BindAttrs, std::tuple<const Attr*>>> where(const Attr&) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, active_record::tuple_cat_t<BindAttrs, std::tuple<const Attr*>>> where(const Attr&) const &;

        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_relation<Result, active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) &&;
        template<Tuple SrcBindAttrs>
        [[nodiscard]] query_relation<Result, active_record::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const&;

        [[nodiscard]] query_relation<Result, BindAttrs>& limit(const std::size_t) &&;
        [[nodiscard]] query_relation<Result, BindAttrs> limit(const std::size_t) const &;

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs>& order_by(const active_record::order = active_record::order::asc) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs> order_by(const active_record::order = active_record::order::asc) const &;

        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> count() &&;
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, std::size_t>, BindAttrs> count() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::sum::attribute_type>, BindAttrs> sum() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::sum::attribute_type>, BindAttrs> sum() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::avg::attribute_type>, BindAttrs> avg() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::avg::attribute_type>, BindAttrs> avg() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::max::attribute_type>, BindAttrs> max() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::max::attribute_type>, BindAttrs> max() const&;

        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::min::attribute_type>, BindAttrs> min() &&;
        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<std::unordered_map<typename Result::key_type, typename Attr::min::attribute_type>, BindAttrs> min() const&;
    };
}