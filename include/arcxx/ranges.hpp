#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */

#include "../arcxx.hpp"

namespace arcxx::ranges{
    template<typename Derived>
    struct query_range_view_interface{
        using derived_type = Derived;
        
        struct sentinel{};
        class iterator{
        public:
            using connector_type = std::remove_cvref_t<decltype(std::declval<Derived>().get_connector())>;
            using query_relation_type = std::remove_cvref_t<decltype(std::declval<Derived>().make_query_relation())>;
            using result_type = typename decltype(std::declval<Derived>().make_query_relation())::result_type;
            using value_type = std::conditional_t<
                specialized_from<result_type, std::vector> || specialized_from<result_type, std::unordered_map>,
                typename result_type::value_type, result_type
            >;

            iterator() = delete;
            iterator(connector_type& conn, query_relation_type&& q) : connector_ref(conn), query(std::move(q)){}
            iterator(connector_type& conn, const query_relation_type& q): connector_ref(conn), query(std::move(q)){}
            
            bool operator==(const sentinel);
            value_type operator*();
            iterator& operator++();
        private:
            connector_type& connector_ref;
            query_relation_type query;
            value_type result_buffer;
        };

        auto begin();
        constexpr sentinel end() const noexcept { return sentinel{}; }
    };

    template<typename Derived>
    struct query_range_adaptor_interface{
        using derived_type = Derived;
    };

    template<typename T>
    concept query_range_view = std::derived_from<std::remove_cvref_t<T>, query_range_view_interface<typename std::remove_cvref_t<T>::derived_type>>;
    template<typename T>
    concept query_range_adaptor = std::derived_from<std::remove_cvref_t<T>, query_range_adaptor_interface<typename std::remove_cvref_t<T>::derived_type>>;

    // view | adaptor -> adaptor(view) -> view
    template<query_range_view RV, query_range_adaptor RA>
    auto operator|(RV&& view, RA&& adaptor){
        return std::forward<RA>(adaptor)(std::forward<RV>(view));
    }
}

#include "ranges/adaptor_pipe.ipp"
#include "ranges/query_relation_view.ipp"
#include "ranges/select.ipp"
#include "ranges/where.ipp"
