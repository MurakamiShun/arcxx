#pragma once
#include "../adaptor.hpp"
#include "../utils.hpp"
#include "../query.hpp"
#include "../attribute.hpp"
#include <optional>
#include <unordered_map>
#if __has_include(<sqlite3.h>)
#include <sqlite3.h>

namespace active_record {
    namespace sqlite3_options {
        constexpr auto readonly   = SQLITE_OPEN_READONLY;
        constexpr auto readwrite  = SQLITE_OPEN_READWRITE;
        constexpr auto create     = SQLITE_OPEN_CREATE;
        constexpr auto uri        = SQLITE_OPEN_URI;
        constexpr auto memory     = SQLITE_OPEN_MEMORY;
        constexpr auto no_mutex   = SQLITE_OPEN_NOMUTEX;
        constexpr auto full_mutex = SQLITE_OPEN_FULLMUTEX;
    }

    class sqlite3_adaptor : adaptor {
    private:
        sqlite3* db_obj;

        sqlite3_adaptor() = delete;
        sqlite3_adaptor(const active_record::string& file_name){
            auto result = sqlite3_open_v2(file_name.c_str(), &db_obj, 0, NULL);
            if(result != SQLITE_OK) error_msg.set(db_obj);
        }

        template<typename T>
        static int callback(void* res, int col_number, char** col_texts, char** col_names){
            auto result = reinterpret_cast<T*>(res);
            return SQLITE_OK;
        }

        template<Container T>
        requires std::derived_from<typename T::value_type, model<typename T::value_type>>
        static int callback(void* res, int col_number, char** col_texts, char** col_names){
            auto result = reinterpret_cast<T*>(res);
            typename T::value_type val;
            for(int n = 0; n < col_number; ++n){
                val[col_names[n]].from_string(col_texts[n]);
            }
            result.push_back(val);
            return SQLITE_OK;
        }
        template<Container T>
        requires Tuple<typename T::value_type>
        static int callback(void* res, int col_number, char** col_texts, char** col_names){
            auto result = reinterpret_cast<T*>(res);
            typename T::value_type val;
            auto tmp = std::apply(
                []<typename... Attrs>(Attrs&... attrs){ std::unordered_map<active_record::string, attribute_string_convertor*>{{attrs.column_name, &attrs}...}; },
                val
            );
            for(int n = 0; n < col_number; ++n){
                tmp[col_names[n]]->from_string(col_texts[n]);
            }
            result.push_back(val);
            return SQLITE_OK;
        }

        template<Attribute T>
        struct is_reference {
            template<typename S>
            static decltype(std::declval<typename S::foreign_key_type>(), std::true_type{}) check(S);
            static std::false_type check(...);
            static constexpr bool value = decltype(check(std::declval<T>()))::value;
        };
        
        template<Attribute T>
        static active_record::string reference_definition(){
            if constexpr(is_reference<T>::value){
                return active_record::string{ ",FOREIGN KEY(" }
                    + active_record::string{ T::column_name } + ") REFERENCES "
                    + active_record::string{ T::foreign_key_type::model_type::table_name } + "("
                    + active_record::string{ T::foreign_key_type::column_name } + ")";
            }
            else return "";
        }
    public:
        struct error_message{
            std::optional<active_record::string> msg = std::nullopt;
            void set(sqlite3* db) {
                auto msg_ptr = sqlite3_errmsg(db);
                if(msg_ptr != NULL) msg = msg_ptr;
                else msg = std::nullopt;
            }
            operator bool(){ return static_cast<bool>(msg); }
        } error_msg;
        static sqlite3_adaptor open(const active_record::string& file_name){
            return sqlite3_adaptor{ file_name };
        }
        ~sqlite3_adaptor(){
            sqlite3_close(db_obj);
        }

        active_record::string_view version(){
            return sqlite3_version;
        }
        int version_number(){
            return sqlite3_libversion_number();
        }

        template<typename T>
        [[nodiscard]] T&& exec(query_relation<T> query){
            std::vector<T> result;
            char* errmsg = nullptr;
            sqlite3_exec(db_obj,
                query.to_sql().c_str(),
                &callback,
                &result,
                &errmsg
            );
            if(errmsg != NULL){
                error_msg.set(db_obj);
                sqlite3_free(errmsg);
            }
            return std::move(result);
        }
        bool exec(query_relation<bool> query){
            char* errmsg = nullptr;
            sqlite3_exec(db_obj,
                query.to_sql().c_str(),
                nullptr,
                nullptr,
                &errmsg
            );
            if(errmsg != NULL){
                error_msg.set(db_obj);
                sqlite3_free(errmsg);
                return false;
            }
            return true;
        }

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
                    : "")
                + reference_definition<T>();
        }

        template<Attribute T>
        requires std::integral<typename T::value_type>
        static active_record::string column_definition() {
            return active_record::string{ T::column_name }
                + active_record::string{ " INTEGER" }
                + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
                + (T::has_constraint(T::unique) ? " UNIQUE" : "")
                + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
                + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                    active_record::string{" DEFAULT "}
                        + std::to_string(T::get_constraint(typename T::constraint_default_value_impl{})->template target<typename T::constraint_default_value_impl>()->default_value)
                    : "")
                + reference_definition<T>();
        }
        
        template<Attribute T>
        requires std::floating_point<typename T::value_type>
        static active_record::string column_definition() {
            return active_record::string{ T::column_name }
                + active_record::string{ " REAL" }
                + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
                + (T::has_constraint(T::unique) ? " UNIQUE" : "")
                + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
                + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                    active_record::string{" DEFAULT "}
                        + std::to_string(T::get_constraint(typename T::constraint_default_value_impl{})->template target<typename T::constraint_default_value_impl>()->default_value)
                    : "")
                + reference_definition<T>();
        }
    };
}
#else
#include <type_traits>
namespace active_record {
    template<typename SFINAE>
    struct sqlite3_adaptor_impl{static_assert(SFINAE::value, "sqlite3.h is not found");};
    
    using sqlite3_adaptor = sqlite3_adaptor_impl<std::false_type>;
}
#endif