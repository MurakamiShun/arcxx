#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include <libpq-fe.h>
#include "postgresql/schema.hpp"
#include "postgresql/utils.hpp"

namespace arcxx {
    namespace PostgreSQL {
        /*
         * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         * DO NOT HARD CORD SENSITIVE INFORMATION
         * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         */
        struct endpoint {
            const arcxx::string server_name;
            const arcxx::string port = "5432";
            const arcxx::string db_name;
        };

        struct auth {
            const arcxx::string user;
            const arcxx::string password;
        };

        struct options {
            const arcxx::string option;
            const arcxx::string debug_of;
        };
    }

    class postgresql_connector : public connector {
    private:
        ::PGconn* conn = nullptr;
        std::optional<arcxx::string> error_msg = std::nullopt;

        template<typename Result, specialized_from<std::tuple> BindAttrs>
        PGresult* exec_sql(const query_relation<Result, BindAttrs>& query);

        template<typename ResultType>
        struct executer;
    public:
        postgresql_connector(const PostgreSQL::endpoint& endpoint_info, const std::optional<PostgreSQL::auth>& auth_info, const std::optional<PostgreSQL::options> option);
        postgresql_connector(const arcxx::string& info);
        postgresql_connector() = delete;
        postgresql_connector(const postgresql_connector&) = delete;
        postgresql_connector(postgresql_connector&&);
        ~postgresql_connector();

        bool has_error() const noexcept;
        const arcxx::string& error_message() const;

        static postgresql_connector open(const PostgreSQL::endpoint endpoint_info, const std::optional<PostgreSQL::auth> auth_info = std::nullopt, const std::optional<PostgreSQL::options> option = std::nullopt);
        static postgresql_connector open(const arcxx::string& connection_info);

        int protocol_version() const;
        int server_version() const;

        void close();

        static constexpr bool bindable = true;
        static arcxx::string bind_variable_str(const std::size_t idx, arcxx::string&& buff = {});

        template<is_model Mod>
        arcxx::expected<void, arcxx::string> create_table(decltype(abort_if_exists));
        template<is_model Mod>
        arcxx::expected<void, arcxx::string> create_table();
        template<is_model Mod>
        arcxx::expected<void, arcxx::string> drop_table();

        template<is_model Mod>
        bool exists_table();

        template<typename Result, specialized_from<std::tuple> BindAttrs>
        arcxx::expected<Result, arcxx::string> exec(const query_relation<Result, BindAttrs>& query);

        template<specialized_from<std::tuple> BindAttrs>
        arcxx::expected<void, arcxx::string> exec(const query_relation<void, BindAttrs>& query);

        arcxx::expected<void, arcxx::string> begin();
        arcxx::expected<void, arcxx::string> commit();
        arcxx::expected<void, arcxx::string> rollback();

        template<typename F>
        requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t()>>
        arcxx::expected<void, arcxx::string> transaction(F&& func);
        template<typename F>
        requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t(postgresql_connector&)>>
        arcxx::expected<void, arcxx::string> transaction(F&& func);

        template<is_attribute Attr>
        static arcxx::string column_definition();
    };

    namespace PostgreSQL {
        using connector = postgresql_connector;
    }
}

#include "postgresql/connector.ipp"