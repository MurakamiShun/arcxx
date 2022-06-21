#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */

namespace arcxx::ranges{
    template<query_range_view ViewSource, specialized_from<query_condition> QueryCondition>
    class where_view : public query_range_view_interface<where_view<ViewSource, QueryCondition>>{
    private:
        ViewSource view_source;
        QueryCondition condition_query;
    public:
        using result_type = typename ViewSource::result_type;
        where_view() = delete;
        where_view(auto&& v_source, auto&& cond_query)
        : view_source(std::forward<decltype(v_source)>(v_source)),
          condition_query(std::forward<decltype(cond_query)>(cond_query)){
        }

        auto make_query_relation() const& {
            return view_source.make_query_relation().where(condition_query);
        }
        auto make_query_relation() &&{
            return std::move(view_source).make_query_relation().where(std::move(condition_query));
        }

        auto& get_connector() const noexcept {
            return view_source.get_connector();
        }
    };

    namespace adaptor{
        template<specialized_from<query_condition> QueryCondition>
        requires std::same_as<QueryCondition, std::remove_cvref_t<QueryCondition>>
        class where_adaptor : public query_range_adaptor_interface<where_adaptor<QueryCondition>>{
        private:
            QueryCondition condition_query;
        public:
            where_adaptor(auto&& cond_query) : condition_query(std::forward<decltype(cond_query)>(cond_query)){}
            template<query_range_view RV>
            auto operator()(RV&& view) && {
                return where_view<std::remove_cvref_t<RV>, QueryCondition>{
                    std::forward<RV>(view),
                    std::move(condition_query)
                };
            }
            template<query_range_view RV>
            auto operator()(RV&& view) const& {
                return where_view<std::remove_cvref_t<RV>, QueryCondition>{
                    std::forward<RV>(view),
                    condition_query
                };
            }
        };

        namespace detail{
            struct where_adaptor_closure{
                template<specialized_from<query_condition> QueryCondition>
                auto operator()(QueryCondition&& cond) const {
                    return where_adaptor<std::remove_cvref_t<QueryCondition>>{
                        std::forward<QueryCondition>(cond)
                    };
                }
            };
        }
    }

    namespace views{
        inline constexpr auto where = adaptor::detail::where_adaptor_closure{};
    }
}