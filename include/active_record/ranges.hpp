#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include <ranges>
#include "adaptor.hpp"

namespace active_record::ranges{

    template<typename Derived>
    struct query_range_view_interface : public std::ranges::view_interface<Derived>{
        using derived_type = Derived;

        static_assert(requires(Derived d){ d.make_query_relation(); });
        static_assert(requires(Derived d){ d.db_adaptor(); });
        
        struct sentinel{};
        class iterator{
        public:
            using dbadaptor_type = std::remove_cvref_t<decltype(std::declval<Derived>().db_adaptor())>;
            using query_relation_type = std::remove_cvref_t<decltype(std::declval<Derived>().make_query_relation())>;
            using result_type = decltype(std::declval<Derived>().make_query_relation())::result_type;
            using value_type = std::conditional_t<
                specialized_from<result_type, std::vector> || specialized_from<result_type, std::unordered_map>,
                typename result_type::value_type, result_type
            >;

            iterator() = delete;
            iterator(dbadaptor_type& adpt, query_relation_type&& q) : db_adpt(adpt), query(std::move(q)){}
            iterator(dbadaptor_type& adpt, const query_relation_type& q): db_adpt(adpt), query(std::move(q)){}
            
            bool operator==(const sentinel);
            value_type operator*();
            iterator& operator++();
        private:
            dbadaptor_type& db_adpt;
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

    template<std::derived_from<adaptor> DBAdaptor, specialized_from<query_relation_common> QueryRelation>
    class db_adaptor_view : query_range_view_interface<db_adaptor_view<DBAdaptor, QueryRelation>>{
    private:
        QueryRelation query_relation;
        DBAdaptor& adaptor_ref;
    public:
        db_adaptor_view() = delete;
        db_adaptor_view(DBAdaptor& adpt, QueryRelation&& relation) : adaptor_ref(adpt), query_relation(std::move(relation)){}
        db_adaptor_view(DBAdaptor& adpt, const QueryRelation& relation) : adaptor_ref(adpt), query_relation(relation){}
        auto make_query_relation() && { return std::move(query_relation); }
        auto make_query_relation() const& { return query_relation; }
        DBAdaptor& db_adaptor() noexcept { return adaptor_ref; }
    };

    template<std::derived_from<adaptor> DBAdaptor, specialized_from<query_relation_common> QueryRelation>
    auto operator|(DBAdaptor& db_adpt, QueryRelation&& relation){
        return db_adaptor_view<DBAdaptor, std::remove_cvref_t<QueryRelation>>(db_adpt, std::forward<QueryRelation>(relation));
    }
}

#include "ranges/adaptor_pipe.ipp"
#include "ranges/select.ipp"
