#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
namespace arcxx::ranges{
    template<query_range_adaptor... RAs>
    struct query_range_adaptor_pipe : std::tuple<RAs...>, query_range_adaptor_interface<query_range_adaptor_pipe<RAs...>>{
        query_range_adaptor_pipe() = delete;
        template<std::convertible_to<RAs>... Args>
        constexpr explicit query_range_adaptor_pipe(Args&&... args) : std::tuple<RAs...>{std::forward<Args>(args)...}{}
        template<query_range_view RV>
        constexpr auto operator()(RV&& view) const& {
            return [this, &view]<std::size_t... Is>(std::index_sequence<Is...>){
                return (std::forward<RV>(view) | ... | std::get<Is>(*this));
            }(std::make_index_sequence<sizeof...(RAs)>{});
        }
        template<query_range_view RV>
        constexpr auto operator()(RV&& view) && {
            return [this, &view]<std::size_t... Is>(std::index_sequence<Is...>){
                return (std::forward<RV>(view) | ... | std::move(std::get<Is>(*this)));
            }(std::make_index_sequence<sizeof...(RAs)>{});
        }

        template<query_range_adaptor RA>
        constexpr auto operator()(RA&& adaptor) const& {
            return [this, &adaptor]<std::size_t... Is>(std::index_sequence<Is...>){
                return (std::forward<RA>(adaptor) | ... | std::get<Is>(*this));
            }(std::make_index_sequence<sizeof...(RAs)>{});
        }
        template<query_range_adaptor RA>
        constexpr auto operator()(RA&& adaptor) && {
            return [this, &adaptor]<std::size_t... Is>(std::index_sequence<Is...>){
                return (std::forward<RA>(adaptor) | ... | std::move(std::get<Is>(*this)));
            }(std::make_index_sequence<sizeof...(RAs)>{});
        }
    };
    
    template<query_range_view RV, specialized_from<query_range_adaptor_pipe> RAP>
    constexpr auto operator|(RV&& range_view, RAP&& adaptor_pipe){
        return std::forward<RAP>(adaptor_pipe)(std::forward<RV>(range_view));
    }

    template<specialized_from<query_range_adaptor_pipe> RAP, query_range_adaptor RA>
    constexpr auto operator|(RAP&& adaptor_pipe, RA&& adaptor){
        return std::forward<RA>(adaptor)(std::forward<RAP>(adaptor_pipe));
    }
    template<query_range_adaptor RA, specialized_from<query_range_adaptor_pipe> RAP>
    constexpr auto operator|(RA&& adaptor, RAP&& adaptor_pipe){
        return std::forward<RA>(adaptor_pipe)(std::forward<RAP>(adaptor));
    }

    template<typename Derived>
    template<specialized_from<query_range_adaptor_pipe> RAP>
    auto query_range_adaptor_interface<Derived>::operator()(RAP&& range_adaptor_pipe) const& {
        return [this, &range_adaptor_pipe]<std::size_t... Is>(std::index_sequence<Is...>){
            return query_range_adaptor_pipe<std::tuple_element_t<Is, std::remove_cvref_t<RAP>>..., Derived>{
                std::get<Is>(std::forward<RAP>(range_adaptor_pipe))...,
                reinterpret_cast<Derived>(*this)
            };
        }(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<RAP>>>{});
    }

    template<typename Derived>
    template<specialized_from<query_range_adaptor_pipe> RAP>
    auto query_range_adaptor_interface<Derived>::operator()(RAP&& range_adaptor_pipe) && {
        return [this, &range_adaptor_pipe]<std::size_t... Is>(std::index_sequence<Is...>){
            return query_range_adaptor_pipe<std::tuple_element_t<Is, std::remove_cvref_t<RAP>>..., Derived>{
                std::get<Is>(std::forward<RAP>(range_adaptor_pipe))...,
                std::move(reinterpret_cast<Derived>(*this))
            };
        }(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<RAP>>>{});
    }
}