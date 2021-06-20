#pragma once
#include <optional>
#include "../adaptor.hpp"
#include "../utils.hpp"
#include "../query.hpp"
#include "../attribute.hpp"
#if __has_include(<sqlite3.h>)
#include <sqlite3.h>

namespace active_record{
    class sqlite3_adaptor : adaptor {
    private:
        sqlite3* db_obj;

        sqlite3_adaptor() = delete;
        sqlite3_adaptor(const active_record::string& file_name){
            auto result = sqlite3_open(file_name.c_str(), &db_obj);
            error_msg.set(result);
        }

        static int callback(void* result, int argv, char** argc, char** column_names) {
            
        }
    public:
        struct error_message{
            std::optional<active_record::string> msg;
            void set(const int errcode, sqlite3* db) {
                if(errcode == SQLITE_OK) msg.reset();
                else msg = sqlite3_errmsg(db);
            }
            void set(const char* errmsg) {
                msg = errmsg;
            }
            operator bool(){ return msg; }
        } error_msg;
        static sqlite3_adaptor open(const active_record::string& file_name){
            return sqlite3_adaptor{ file_name };
        }
        ~sqlite3_adaptor(){
            auto result = sqlite3_close(db_obj);
            error_msg.set(result)
        }

        template<typename T>
        T exec(query_relation<T> query){
            std::vector<std::vector<active_record::string>> result;
            char* errmsg = nullptr;
            auto result = sqlite3_exec(db_obj,
                query.to_sql(),
                &callback,
                &result,
                &errmsg
            );
            if(errmsg != nullptr){
                sqlite3_free(errmsg);
            }
            error_msg.set(result);
        }

        template<Attribute T>
        requires same_as<T::value_type, active_record::string>
        active_record::string attribute_typename(const T&) {
            return active_record::string{ "TEXT" }
                + (T::has_constrain(T::not_null) ? " NOT NULL" : "")
                + (T::has_constrain(T::unique) ? " UNIQUE" : "")
                + (T::has_constrain(T::primary_key) ? " PRIMARY KEY" : "");
                + (T::has_constrain(T::constraint_default_value_impl) ?
                    active_record::string{" DEFAULT '"}
                        + active_record::sanitize(T::get_constraint(T::constraint_default_value_impl{}).target<T::constraint_default_value_impl>().default_value)
                        + "'"
                    : "");
                //+ (T::has_constrain(T::constraint_length_impl) ? " CHECK(" + std::to_string(T::get_constraint().target<constraint_length_impl>().length) + ")" : "");
        }

        template<Attribute T>
        requires std::integral<T::value_type>
        active_record::string attribute_typename(const T&) {
            return active_record::string{ "INTEGER" }
                + (T::has_constrain(T::not_null) ? " NOT NULL" : "")
                + (T::has_constrain(T::unique) ? " UNIQUE" : "")
                + (T::has_constrain(T::primary_key) ? " PRIMARY KEY" : "");
                //+ (T::has_constrain(T::constraint_length_impl) ? " LENGTH" + std::to_string(T::get_constraint().target<constraint_length_impl>().length) : "");
        }
    };
}

#endif