#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
namespace arcxx::ranges::adaptor{
    template<query_range_adaptor... RAs>
    struct query_range_adaptor_pipe : std::tuple<RAs...>, query_range_adaptor_interface<query_range_adaptor_pipe<RAs...>>{
        static constexpr std::size_t adaptor_size = sizeof...(RAs);

        query_range_adaptor_pipe() = delete;
        template<std::convertible_to<RAs>... Args>
        constexpr explicit query_range_adaptor_pipe(Args&&... args) : std::tuple<RAs...>{std::forward<Args>(args)...}{}

        // view | adaptor_pipe -> view
        template<query_range_view RV>
        constexpr auto operator()(RV&& view) const& {
            return [this, &view]<std::size_t... Is>(std::index_sequence<Is...>){
                return (std::forward<RV>(view) | ... | std::get<Is>(*this));
            }(std::make_index_sequence<adaptor_size>{});
        }
        template<query_range_view RV>
        constexpr auto operator()(RV&& view) && {
            return [this, &view]<std::size_t... Is>(std::index_sequence<Is...>){
                return (std::forward<RV>(view) | ... | std::move(std::get<Is>(*this)));
            }(std::make_index_sequence<adaptor_size>{});
        }
    };

    // view | adaptor_pipe -> view
    template<query_range_view RV, specialized_from<query_range_adaptor_pipe> RAP>
    constexpr auto operator|(RV&& range_view, RAP&& adaptor_pipe){
        return std::forward<RAP>(adaptor_pipe)(std::forward<RV>(range_view));
    }
    // adaptor_pipe | adaptor -> adaptor_pipe
    template<specialized_from<query_range_adaptor_pipe> RAP, query_range_adaptor RA>
    constexpr auto operator|(RAP&& adaptor_pipe, RA&& adaptor){
        return [&adaptor_pipe, &adaptor]<std::size_t... Is>(std::index_sequence<Is...>){
            if constexpr(std::is_rvalue_reference_v<RAP>){
                return query_range_adaptor_pipe{ std::move(std::get<Is>(adaptor_pipe))..., std::forward<RA>(adaptor)};
            }
            else{
                return query_range_adaptor_pipe{ std::get<Is>(adaptor_pipe)..., std::forward<RA>(adaptor)};
            }
        }(std::make_index_sequence<RAP::adaptor_size>{});
    }
    // adaptor | adaptor_pipe -> adaptor_pipe
    template<query_range_adaptor RA, specialized_from<query_range_adaptor_pipe> RAP>
    constexpr auto operator|(RA&& adaptor, RAP&& adaptor_pipe){
        return [&adaptor_pipe, &adaptor]<std::size_t... Is>(std::index_sequence<Is...>){
            if constexpr(std::is_rvalue_reference_v<RAP>){
                return query_range_adaptor_pipe{ std::forward<RA>(adaptor), std::move(std::get<Is>(adaptor_pipe))...};
            }
            else{
                return query_range_adaptor_pipe{ std::forward<RA>(adaptor), std::get<Is>(adaptor_pipe)...};
            }
        }(std::make_index_sequence<RAP::adaptor_size>{});
    }
    // adaptor | adaptor -> adaptor_pipe
    template<query_range_adaptor RA1, query_range_adaptor RA2>
    constexpr auto operator|(RA1&& adaptor1, RA2&& adaptor2){
        return query_range_adaptor_pipe{ std::forward<RA1>(adaptor1), std::forward<RA2>(adaptor2) };
    }
}