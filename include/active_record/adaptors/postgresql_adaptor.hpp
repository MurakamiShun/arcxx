#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include <libpq-fe.h>
#include "postgresql/schema.hpp"
#include "postgresql/utils.hpp"

namespace active_record {
    namespace PostgreSQL {
        /*
         * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         * DO NOT HARD CORD SENSITIVE INFORMATION
         * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         */
        struct endpoint {
            const active_record::string server_name;
            const active_record::string port = "5432";
            const active_record::string db_name;
        };

        struct auth {
            const active_record::string user;
            const active_record::string password;
        };

        struct options {
            const active_record::string option;
            const active_record::string debug_of;
        };
    }

    class postgresql_adaptor : public adaptor {
    private:
        ::PGconn* conn = nullptr;
        std::optional<active_record::string> error_msg = std::nullopt;

        postgresql_adaptor() = delete;
        postgresql_adaptor(const PostgreSQL::endpoint& endpoint_info, const std::optional<PostgreSQL::auth>& auth_info, const std::optional<PostgreSQL::options> option);
        postgresql_adaptor(const active_record::string& info);

        template<typename Result, specialized_from<std::tuple> BindAttrs>
        PGresult* exec_sql(const query_relation<Result, BindAttrs>& query);

    public:
        bool has_error() const noexcept;
        const active_record::string& error_message() const;

        static postgresql_adaptor open(const PostgreSQL::endpoint endpoint_info, const std::optional<PostgreSQL::auth> auth_info = std::nullopt, const std::optional<PostgreSQL::options> option = std::nullopt);
        static postgresql_adaptor open(const active_record::string& connection_info);

        int protocol_version() const;
        int server_version() const;

        ~postgresql_adaptor();

        void close();

        static constexpr bool bindable = true;
        static active_record::string bind_variable_str(const std::size_t idx);

        template<is_model Mod>
        std::optional<active_record::string> create_table(bool abort_if_exist = true);
        template<is_model Mod>
        std::optional<active_record::string> drop_table();

        template<typename T, specialized_from<std::tuple> BindAttrs>
        auto exec(const query_relation<T, BindAttrs>&& query);

        template<specialized_from<std::tuple> BindAttrs>
        auto exec(const query_relation<bool, BindAttrs>& query);

        template<typename Result, specialized_from<std::tuple> BindAttrs>
        auto exec(const query_relation<Result, BindAttrs>& query);

        std::optional<active_record::string> begin();
        std::optional<active_record::string> commit();
        std::optional<active_record::string> rollback();

        template<std::convertible_to<std::function<active_record::transaction(void)>> F>
        std::pair<std::optional<active_record::string>, active_record::transaction> transaction(F& func);
        template<std::convertible_to<std::function<active_record::transaction(void)>> F>
        std::pair<std::optional<active_record::string>, active_record::transaction> transaction(F&& func);
        template<std::convertible_to<std::function<active_record::transaction(postgresql_adaptor&)>> F>
        std::pair<std::optional<active_record::string>, active_record::transaction> transaction(F& func);
        template<std::convertible_to<std::function<active_record::transaction(postgresql_adaptor&)>> F>
        std::pair<std::optional<active_record::string>, active_record::transaction> transaction(F&& func);

        template<is_attribute Attr>
        static active_record::string column_definition();
    };

    namespace PostgreSQL {
        using adaptor = postgresql_adaptor;
    }
}

#include "postgresql/adaptor.ipp"