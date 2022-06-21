#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */

namespace arcxx{
    namespace ranges{
        template<is_connector Connector, specialized_from<query_relation> QueryRelation>
        class query_relation_view : public query_range_view_interface<query_relation_view<Connector, QueryRelation>>{
        private:
            QueryRelation query_relation;
            Connector& connector_ref;
        public:
            using result_type = typename QueryRelation::result_type;
            query_relation_view() = delete;
            query_relation_view(Connector& conn, auto&& relation)
            : connector_ref(conn),
              query_relation(std::forward<decltype(relation)>(relation)){
            }
            auto make_query_relation() && { return std::move(query_relation); }
            auto make_query_relation() const& { return query_relation; }
            Connector& get_connector() const noexcept { return connector_ref; }
        };
        namespace adaptor{
            template<typename QueryRelation>
            requires (specialized_from<QueryRelation, query_relation> && std::same_as<std::remove_cvref_t<QueryRelation>, QueryRelation>)
            class query_relation_adaptor : public query_range_adaptor_interface<query_relation_adaptor<QueryRelation>>{
            private:
                QueryRelation query;
            public:
                query_relation_adaptor(const QueryRelation& q) : query(q){}
                query_relation_adaptor(QueryRelation&& q) : query(std::move(q)){}
                template<is_connector Connector>
                constexpr auto operator()(Connector& conn) const& {
                    return query_relation_view<Connector, QueryRelation>{ conn, query };
                }
                template<is_connector Connector>
                constexpr auto operator()(Connector& conn) && {
                    return query_relation_view<Connector, QueryRelation>{ conn, std::move(query) };
                }
            };
        }
    }
    // connector | query_relation -> query_relation_view
    template<is_connector Connector, specialized_from<query_relation> QueryRelation>
    auto operator|(Connector& conn, QueryRelation&& query){
        return ranges::query_relation_view<std::remove_cvref_t<Connector>, std::remove_cvref_t<QueryRelation>>{conn, std::forward<QueryRelation>(query)};
    }

    // query_relation | adaptor -> adaptor_pipe
    template<specialized_from<query_relation> QueryRelation, ranges::query_range_adaptor QRA>
    auto operator|(QueryRelation&& query, QRA&& query_range_adpt){
        return ranges::adaptor::query_range_adaptor_pipe{
            ranges::adaptor::query_relation_adaptor<std::remove_cvref_t<QueryRelation>>{ std::forward<QueryRelation>(query) },
            std::forward<QRA>(query_range_adpt)
        };
    }
    // query_relation | adaptor_pipe -> adaptor_pipe
    template<specialized_from<query_relation> QueryRelation, specialized_from<ranges::adaptor::query_range_adaptor_pipe> QRAP>
    auto operator|(QueryRelation&& query, QRAP&& query_range_adpt_pipe){
        return std::forward<QRAP>(query_range_adpt_pipe)(ranges::adaptor::query_relation_adaptor<std::remove_cvref_t<QueryRelation>>(std::forward<QueryRelation>(query)));
    }
}