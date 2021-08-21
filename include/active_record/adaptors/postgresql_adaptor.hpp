#pragma once
#include "../adaptor.hpp"
#include <postgresql/libpq-fe.h>
#include "postgresql/schema.hpp"
#if !(defined(_WIN32) || defined(_WIN64))
#include <byteswap.h>
#endif

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

        template<typename T>
        auto hton(const T h) -> decltype(h) {
            #if defined(_WIN32) || defined(_WIN64)
            if constexpr (sizeof(h) == sizeof(uint16_t)) return _byteswap_ushort(h);
            else if constexpr (sizeof(h) == sizeof(uint32_t)) return _byteswap_ulong(h);
            else if constexpr (sizeof(h) == sizeof(uint64_t)) return _byteswap_uint64(h);
            #else
            if constexpr (__BYTE_ORDER == __LITTLE_ENDIAN) {
                if constexpr (sizeof(h) == sizeof(uint16_t)) return bswap_16(h);
                else if constexpr (sizeof(h) == sizeof(uint32_t)) return bswap_32(h);
                else if constexpr (sizeof(h) == sizeof(uint64_t)) return bswap_64(h);
            }
            #endif
            return h;
        }
        template<typename T>
        auto ntoh(const T n) -> decltype(n) {
            #if defined(_WIN32) || defined(_WIN64)
            if constexpr (sizeof(n) == sizeof(uint16_t)) return _byteswap_ushort(n);
            else if constexpr (sizeof(n) == sizeof(uint32_t)) return _byteswap_ulong(n);
            else if constexpr (sizeof(n) == sizeof(uint64_t)) return _byteswap_uint64(n);
            #else
            if constexpr (__BYTE_ORDER == __LITTLE_ENDIAN) {
                if constexpr (sizeof(n) == sizeof(uint16_t)) return bswap_16(n);
                else if constexpr (sizeof(n) == sizeof(uint32_t)) return bswap_32(n);
                else if constexpr (sizeof(n) == sizeof(uint64_t)) return bswap_64(n);
            }
            #endif
            return n;
        }
    }

    class postgresql_adaptor : public adaptor {
    private:
        ::PGconn* conn = nullptr;
        std::optional<active_record::string> error_msg = std::nullopt;
        
        postgresql_adaptor() = delete;
        postgresql_adaptor(const PostgreSQL::endpoint& endpoint_info, const std::optional<PostgreSQL::auth>& auth_info, const std::optional<PostgreSQL::options> option) {
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
        postgresql_adaptor(const active_record::string& info){
            conn = PQconnectdb(info.c_str());
            if (PQstatus(conn) == CONNECTION_BAD){
                // handle error
            }
        }
    public:
        static postgresql_adaptor open(const PostgreSQL::endpoint endpoint_info, const std::optional<PostgreSQL::auth> auth_info = std::nullopt, const std::optional<PostgreSQL::options> option = std::nullopt){
            return postgresql_adaptor(endpoint_info, auth_info, option);
        }
        static postgresql_adaptor open(const active_record::string& connection_info){
            return postgresql_adaptor(connection_info);
        }

        int protocol_version() const {
            return PQprotocolVersion(conn);
        }
        int server_version() const {
            return PQprotocolVersion(conn);
        }

        ~postgresql_adaptor() {
            PQfinish(conn);
        }

        static constexpr bool bindable = true;
        static active_record::string bind_variable_str(const std::size_t idx) {
            return active_record::string{ "$" } + std::to_string(idx + 1);
        }

        template<Model Mod>
        std::optional<active_record::string> create_table(bool create_if_not_exist = false){
            const auto sql = Mod::schema::template to_sql<postgresql_adaptor>(create_if_not_exist);
            PGresult* result = PQexec(
                conn,
                sql.c_str()
            );
            if (PQresultStatus(result) == PGRES_TUPLES_OK) error_msg = std::nullopt;
            else error_msg = PQresultErrorMessage(result);
            
            if(result != nullptr) PQclear(result);
            
            return error_msg;
        }

        
        template<typename T, Tuple BindAttrs>
        auto exec(const query_relation<T, BindAttrs>&& query){
            return exec(query);
        }

        template<Tuple BindAttrs>
        std::optional<active_record::string> exec(const query_relation<bool, BindAttrs>& query){
            const auto sql = query.template to_sql<postgresql_adaptor>();

            PGresult* result = nullptr;
            if constexpr(std::tuple_size_v<BindAttrs> == 0){
                result = PQexec(
                    conn,
                    sql.c_str()
                );
            }
            else{
                const auto param_length = std::apply(
                    []<typename... Attrs>(const Attrs* const... attr_ptrs){
                        return std::array<int, sizeof...(Attrs)>{ static_cast<int>(attribute_size(*attr_ptrs))... };
                    },
                    query.bind_attrs
                );
                const auto param_format = std::apply(
                    []<typename... Attrs>(const Attrs* const... attr_ptrs){
                        const auto is_not_text_format = []<typename Attr>([[maybe_unused]]const Attr* const){
                            return static_cast<int>(
                                std::is_same_v<typename Attr::value_type, active_record::string> ||
                                std::is_same_v<typename Attr::value_type, active_record::datetime>
                            );
                        };
                        return std::array<int, sizeof...(Attrs)>{ is_not_text_format(attr_ptrs)... };
                    },
                    query.bind_attrs
                );

                const auto param_values = std::apply(
                    []<typename... Attrs>(const Attrs* const... attr_ptrs){
                        const auto get_value_ptr = []<typename Attr>(const Attr* const attr_ptr){
                            if (*attr_ptr) static_cast<const char* const>(&(attr_ptr->value()));
                            else return nullptr;
                        };
                        return std::array<const char* const, sizeof...(Attrs)>{ get_value_ptr(attr_ptrs)... };
                    },
                    query.bind_attrs
                );
                
                result = PQexecParams(
                    conn,
                    sql.c_str(),
                    std::tuple_size_v<BindAttrs>, // parameter count
                    NULL, // parameter types
                    param_values.data(), // parameter values
                    param_length.data(), // parameter length
                    param_format.data(), // parameter formats
                    1  // result formats(binary)
                );
            }
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

    namespace PostgreSQL {
        using adaptor = postgresql_adaptor;
    }
}