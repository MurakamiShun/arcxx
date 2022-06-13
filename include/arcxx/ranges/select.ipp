#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */

namespace arcxx::ranges{
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

        decltype(auto) get_connector() const {
            return source.get_connector();
        }
    };

    namespace adaptor{
        template<typename... Columns>
        struct select_adaptor : public query_range_adaptor_interface<select_adaptor<Columns...>>{
            template<query_range_view RV>
            auto operator()(RV&& view) const {
                return select_view<std::remove_cvref_t<RV>, Columns...>{ std::forward<RV>(view) };
            }
        };
    }

    namespace views{
        template<typename... Columns>
        requires (is_attribute<Columns> && ...) || (is_attribute_aggregator<Columns> && ...)
        inline constexpr auto select = adaptor::select_adaptor<Columns...>{};
    }
}