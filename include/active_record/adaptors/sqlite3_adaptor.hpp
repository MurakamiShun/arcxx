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
            error_msg.set(result, db_obj);
        }

        static int callback(void* result, int argv, char** argc, char** column_names) {
            return SQLITE_OK;
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
            operator bool(){ return static_cast<bool>(msg); }
        } error_msg;
        static sqlite3_adaptor open(const active_record::string& file_name){
            return sqlite3_adaptor{ file_name };
        }
        ~sqlite3_adaptor(){
            sqlite3_close(db_obj);
        }
        /*
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
            error_msg.set(result, db_obj);
        }
        */

        template<Attribute T>
        requires std::same_as<typename T::value_type, active_record::string>
        static active_record::string column_definition() {
            return active_record::string{ T::column_name }
                + " TEXT"
                + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
                + (T::has_constraint(T::unique) ? " UNIQUE" : "")
                + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
                + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                    active_record::string{" DEFAULT '"}
                        + active_record::sanitize(T::get_constraint(typename T::constraint_default_value_impl{})->template target<typename T::constraint_default_value_impl>()->default_value) + "'"
                    : "")
                + (T::has_constraint(typename T::constraint_length_impl{}) ?
                    active_record::string{ " CHECK(length(" } + active_record::string{ T::column_name }
                        + ")<=" + std::to_string(T::get_constraint(typename T::constraint_length_impl{})->template target<typename T::constraint_length_impl>()->length) + ")"
                    : "");
        }

        template<Attribute T>
        requires std::integral<typename T::value_type>
        static active_record::string column_definition() {
            return active_record::string{ T::column_name }
                + active_record::string{ " INTEGER" }
                + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
                + (T::has_constraint(T::unique) ? " UNIQUE" : "")
                + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "");
                //+ (T::has_constrain(T::constraint_length_impl) ? " LENGTH" + std::to_string(T::get_constraint().target<constraint_length_impl>().length) : "");
        }
        
        /*
        // reference_to
        template<typename T>
        requires std::integral<typename T::value_type>
        static active_record::string column_definition() {
            return active_record::string{ "INTEGER" }
                + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
                + (T::has_constraint(T::unique) ? " UNIQUE" : "")
                + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "");
                //+ (T::has_constrain(T::constraint_length_impl) ? " LENGTH" + std::to_string(T::get_constraint().target<constraint_length_impl>().length) : "");
        }
        */
    };
}

#endif