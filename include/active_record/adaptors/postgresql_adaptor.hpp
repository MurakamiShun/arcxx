#pragma once
#include "../adaptor.hpp"
#if __has_include(<postgresql/libpq-fe.h>)
#include <postgresql/libpq-fe.h>
#include "postgresql/schema.hpp"

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
    class pg_adaptor : public adaptor {
    private:
        ::PGconn* conn = nullptr;
        std::optional<active_record::string> error_msg = std::nullopt;
        
        pg_adaptor() = delete;
        pg_adaptor(const PostgreSQL::endpoint& endpoint_info, const std::optional<PostgreSQL::auth>& auth_info, const std::optional<PostgreSQL::options> option) {
            conn = PQsetdbLogin(
                endpoint_info.server_name.c_str(),
                endpoint_info.port.c_str(),
                option ? option.value().option.c_str() : NULL,
                option ? option.value().debug_of.c_str() : NULL,
                endpoint_info.db_name.c_str(),
                auth_info ? auth_info.value().user.c_str() : NULL,
                auth_info ? auth_info.value().password.c_str() : NULL
            );
            if (PQstatus(conn) == CONNECTION_BAD){
                // handle error
            }
        }
    public:
        static pg_adaptor open(const PostgreSQL::endpoint endpoint_info, const std::optional<PostgreSQL::auth> auth_info = std::nullopt, const std::optional<PostgreSQL::options> option = std::nullopt){
            return pg_adaptor(endpoint_info, auth_info, option);
        }

        int protocol_version() const {
            return PQprotocolVersion(conn);
        }
        int server_version() const {
            return PQprotocolVersion(conn);
        }

        ~pg_adaptor() {
            PQfinish(conn);
        }

        static constexpr bool bindable = true;
        static active_record::string bind_variable_str(const std::size_t idx) {
            return active_record::string{ "$" } + std::to_string(idx + 1);
        }

        template<typename T, Tuple BindAttrs>
        auto exec(const query_relation<T, BindAttrs>&& query){
            return exec(query);
        }

        template<Model Mod>
        std::optional<active_record::string> create_table(bool create_if_not_exist = false){
            PGresult* result = PQexec(
                conn,
                Mod::schema::template to_sql<pg_adaptor>(create_if_not_exist).c_str()
            );
            if (PQresultStatus(result) == PGRES_TUPLES_OK) error_msg = std::nullopt;
            else error_msg = PQresultErrorMessage(result);

            if(result != nullptr) PQclear(result);
            
            return error_msg;
        }

        template<Tuple BindAttrs>
        std::optional<active_record::string> exec(const query_relation<bool, BindAttrs>& query){
            const auto sql = query.template to_sql<pg_adaptor>();
            PGresult* result = PQexec(
                conn,
                sql.c_str()
            );
            if (PQresultStatus(result) == PGRES_TUPLES_OK) error_msg = std::nullopt;
            else error_msg = PQresultErrorMessage(result);
            
            if(result != nullptr) PQclear(result);
            
            return error_msg;
        }

        
        std::optional<active_record::string> begin(){
            return exec(raw_query<bool>(active_record::string{ "BEGIN;" }));
        }
        std::optional<active_record::string> commit(){
            return exec(raw_query<bool>(active_record::string{ "COMMIT;" }));
        }
        std::optional<active_record::string> rollback(){
            return exec(raw_query<bool>(active_record::string{ "ROLLBACK;" }));
        }

        template<std::convertible_to<std::function<active_record::transaction(void)>> F>
        std::pair<std::optional<active_record::string>, active_record::transaction> transaction(F& func) {
            if(const auto errmsg = begin(); errmsg) return { errmsg, active_record::transaction::rollback };
            const auto transaction_result = func();
            switch (transaction_result) {
            case active_record::transaction::commit:
                return { commit(), active_record::transaction::commit };
            default:
                return { rollback(), active_record::transaction::rollback };
            }
        }
        template<std::convertible_to<std::function<active_record::transaction(void)>> F>
        std::pair<std::optional<active_record::string>, active_record::transaction>  transaction(F&& func) {
            return transaction(func);
        }
        template<std::convertible_to<std::function<active_record::transaction(sqlite3_adaptor&)>> F>
        std::pair<std::optional<active_record::string>, active_record::transaction> transaction(F& func) {
            return transaction(static_cast<std::function<active_record::transaction()>>(std::bind(func, std::ref(*this))));
        }
        template<std::convertible_to<std::function<active_record::transaction(sqlite3_adaptor&)>> F>
        std::pair<std::optional<active_record::string>, active_record::transaction>  transaction(F&& func) {
            return transaction(func);
        }
        

        template<Attribute T>
        static active_record::string column_definition() {
            return active_record::PostgreSQL::column_definition<T>();
        }
    };
}

#endif