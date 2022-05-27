#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */

namespace active_record::ranges{
    template<query_range_view ViewSource, typename... SelectedColumns>
    struct select_view : public query_range_view_interface<select_view<ViewSource, SelectedColumns...>>{
        using result_type = std::tuple<SelectedColumns...>;
        ViewSource source = ViewSource{};

        constexpr select_view() = delete;
        constexpr select_view(ViewSource&& vs) : source(std::move(vs)){};
        constexpr select_view(const ViewSource& vs) : source(vs){};

        auto make_query_relation() const {
            return source.make_query_relation().template select<SelectedColumns...>();
        }

        decltype(auto) db_adaptor() const {
            return source.db_adaptor();
        }
    };

    namespace views{
        // range adaptor factory
        namespace detail{
            template<typename... Columns>
            struct select_adaptor : query_range_adaptor_interface<select_adaptor<Columns...>>{
                template<query_range_view RV>
                constexpr auto operator()(RV&& view) const {
                    return select_view<std::remove_cvref_t<RV>, Columns...>{ std::forward<RV>(view) };
                }
            };

            struct select_adaptor_closure{
                template<typename... Columns>
                requires (sizeof...(Columns) > 0)
                constexpr auto operator()() const noexcept {
                    return select_adaptor<Columns...>{};
                }
                template<query_range_view RV, typename... Columns>
                requires (sizeof...(Columns) > 0)
                constexpr auto operator()(RV&& view) const {
                    return select_view<std::remove_cvref_t<RV>, std::tuple<Columns...>>{ std::forward<RV>(view) };
                }
            };

            template<query_range_view RV, typename... Columns>
            constexpr auto operator|(RV&& view, const select_adaptor<Columns...>){
                return select_view<std::remove_cvref_t<RV>, std::tuple<Columns...>>{ std::forward<RV>(view) };
            }
        }
        inline constexpr auto select = detail::select_adaptor_closure{};
    }
}