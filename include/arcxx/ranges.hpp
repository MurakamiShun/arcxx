#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include <ranges>
#include "connector.hpp"

namespace arcxx::ranges{

    template<typename Derived>
    struct query_range_view_interface : public std::ranges::view_interface<Derived>{
        using derived_type = Derived;

        static_assert(requires(Derived d){ d.make_query_relation(); });
        static_assert(requires(Derived d){ d.get_connector(); });
        
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
    struct query_range_adaptor_interface;

    template<typename T>
    concept query_range_view = std::derived_from<std::remove_cvref_t<T>, query_range_view_interface<typename T::derived_type>>;
    template<typename T>
    concept query_range_adaptor = std::derived_from<std::remove_cvref_t<T>, query_range_adaptor_interface<typename T::derived_type>>;

    template<query_range_adaptor... RAs>
    struct query_range_adaptor_pipe;

    template<typename Derived>
    struct query_range_adaptor_interface{
        using derived_type = Derived;
        template<specialized_from<query_range_adaptor_pipe> RAP>
        auto operator()(RAP&& range_adaptor_pipe) const&;
        template<specialized_from<query_range_adaptor_pipe> RAP>
        auto operator()(RAP&& range_adaptor_pipe) &&;

        static_assert(requires(Derived d){ d([]{ struct A : query_range_view_interface<A>{}a{};return a; }()); });
    };

    template<is_connector Connector, specialized_from<query_relation_common> QueryRelation>
    class connector_view : query_range_view_interface<connector_view<Connector, QueryRelation>>{
    private:
        QueryRelation query_relation;
        Connector& connector_ref;
    public:
        connector_view() = delete;
        connector_view(Connector& conn, QueryRelation&& relation) : connector_ref(conn), query_relation(std::move(relation)){}
        connector_view(Connector& conn, const QueryRelation& relation) : connector_ref(conn), query_relation(relation){}
        auto make_query_relation() && { return std::move(query_relation); }
        auto make_query_relation() const& { return query_relation; }
        Connector& get_connector() noexcept { return connector_ref; }
    };

    template<is_connector Connector, specialized_from<query_relation_common> QueryRelation>
    auto operator|(Connector& conn, QueryRelation&& relation){
        return connector_view<Connector, std::remove_cvref_t<QueryRelation>>(conn, std::forward<QueryRelation>(relation));
    }
}

#include "ranges/adaptor_pipe.ipp"
#include "ranges/select.ipp"
