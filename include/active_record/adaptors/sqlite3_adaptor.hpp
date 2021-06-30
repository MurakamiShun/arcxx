#pragma once
#include "../adaptor.hpp"
#include "../utils.hpp"
#include "../query.hpp"
#include "../attribute.hpp"
#include <optional>
#if __has_include(<sqlite3.h>)
#include <sqlite3.h>

namespace active_record {
    namespace sqlite3 {
        namespace options {
            constexpr auto readonly   = SQLITE_OPEN_READONLY;
            constexpr auto readwrite  = SQLITE_OPEN_READWRITE;
            constexpr auto create     = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;
            constexpr auto uri        = SQLITE_OPEN_URI;
            constexpr auto memory     = SQLITE_OPEN_MEMORY;
            constexpr auto no_mutex   = SQLITE_OPEN_NOMUTEX;
            constexpr auto full_mutex = SQLITE_OPEN_FULLMUTEX;
        }

        enum class transaction{
            rollback,
            commit
        };

        template<typename T>
        requires std::derived_from<T, model<T>>
        static int callback(void* res, int col_number, char** col_texts, char** col_names){
            auto result = reinterpret_cast<T*>(res);
            for(int n = 0; n < col_number; ++n){
                result[col_names[n]].from_string((col_texts[n] == nullptr) ? "null" : col_texts[n]);
            }
            return SQLITE_OK;
        }
        template<Tuple T>
        static int callback(void* res, int col_number, char** col_texts, char** col_names){
            auto result = reinterpret_cast<T*>(res);
            auto attribute_string_convertors = std::apply(
                []<typename... Attrs>(Attrs&... attrs){ std::unordered_map<active_record::string_view, attribute_string_convertor>{{attrs.column_name, attrs.get_string_convertor()}...}; },
                *result
            );
            for(int n = 0; n < col_number; ++n){
                attribute_string_convertors[col_names[n]].from_string((col_texts[n] == nullptr) ? "null" : col_texts[n]);
            }
            return SQLITE_OK;
        }
        template<Container T>
        requires std::derived_from<typename T::value_type, model<typename T::value_type>>
        static int callback(void* res, int col_number, char** col_texts, char** col_names){
            auto result = reinterpret_cast<T*>(res);
            typename T::value_type val;
            auto attribute_string_convertors = val.get_attribute_string_convertors();
            for(int n = 0; n < col_number; ++n) {
                attribute_string_convertors[col_names[n]].from_string((col_texts[n] == nullptr) ? "null" : col_texts[n]);
            }
            result->push_back(val);
            return SQLITE_OK;
        }
        template<Container T>
        requires Tuple<typename T::value_type>
        static int callback(void* res, int col_number, char** col_texts, char** col_names){
            auto result = reinterpret_cast<T*>(res);
            typename T::value_type val;
            auto attribute_string_convertors = std::apply(
                []<typename... Attrs>(Attrs&... attrs){ std::unordered_map<active_record::string_view, attribute_string_convertor>{{attrs.column_name, attrs.get_string_convertor()}...}; },
                val
            );
            for(int n = 0; n < col_number; ++n){
                attribute_string_convertors[col_names[n]].from_string((col_texts[n] == nullptr) ? "null" : col_texts[n]);
            }
            result->push_back(val);
            return SQLITE_OK;
        }

        namespace {
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
        }

        template<Attribute T>
        requires std::same_as<typename T::value_type, active_record::string>
        active_record::string column_definition() {
            return active_record::string{ T::column_name }
                + " TEXT"
                + (T::has_constraint(T::unique) ? " UNIQUE" : "")
                + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
                + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
                + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                    active_record::string{ (T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "") } + " DEFAULT '"
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
        active_record::string column_definition() {
            return active_record::string{ T::column_name }
                + active_record::string{ " INTEGER" }
                + (T::has_constraint(T::unique) ? " UNIQUE" : "")
                + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
                + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
                + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                    active_record::string{ (T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "") } + " DEFAULT "
                    + std::to_string(T::get_constraint(typename T::constraint_default_value_impl{})->template target<typename T::constraint_default_value_impl>()->default_value)
                    : "")
                + reference_definition<T>();
        }
        
        template<Attribute T>
        requires std::floating_point<typename T::value_type>
        active_record::string column_definition() {
            return active_record::string{ T::column_name }
                + active_record::string{ " REAL" }
                + (T::has_constraint(T::unique) ? " UNIQUE" : "")
                + (T::has_constraint(T::primary_key) ? " PRIMARY KEY" : "")
                + (T::has_constraint(T::not_null) ? " NOT NULL" : "")
                + (T::has_constraint(typename T::constraint_default_value_impl{}) ?
                    active_record::string{ (T::has_constraint(T::not_null) ? " ON CONFLICT REPLACE" : "") } + " DEFAULT "
                    + std::to_string(T::get_constraint(typename T::constraint_default_value_impl{})->template target<typename T::constraint_default_value_impl>()->default_value)
                    : "")
                + reference_definition<T>();
        }
    }

    class sqlite3_adaptor : adaptor {
    private:
        ::sqlite3* db_obj;

        sqlite3_adaptor() = delete;
        sqlite3_adaptor(const active_record::string& file_name, const int flags){
            auto result = sqlite3_open_v2(file_name.c_str(), &db_obj, flags, nullptr);
            if(result != SQLITE_OK) error_msg = get_error_msg();
        }

        std::optional<active_record::string> error_msg = std::nullopt;

        std::optional<active_record::string> get_error_msg(const char* msg_ptr){
            if(msg_ptr == nullptr) return std::nullopt;
            else return active_record::string{ msg_ptr };
        }
        std::optional<active_record::string> get_error_msg(const int error_code, const char* msg_ptr){
            if(error_code != SQLITE_OK){
                if(msg_ptr != nullptr) return get_error_msg(msg_ptr);
                else return get_error_msg(sqlite3_errstr(error_code));
            }
            else{
                return std::nullopt;
            }
        }
        std::optional<active_record::string> get_error_msg(){
            return get_error_msg(sqlite3_errmsg(db_obj));
        }
    public:
        bool has_error() const noexcept { return static_cast<bool>(error_msg); }
        const active_record::string& error_message() const { return error_msg.value(); }
        
        static sqlite3_adaptor open(const active_record::string& file_name, const int flags = sqlite3::options::readwrite){
            return sqlite3_adaptor{ file_name, flags };
        }
        bool close() {
            return sqlite3_close(db_obj) == SQLITE_OK;
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

        static constexpr bool bindable = true;
        static active_record::string bind_variable_str(const size_t idx) {
            return active_record::string{ ":" } + std::to_string(idx);
        }

        template<typename T>
        [[nodiscard]] std::pair<std::optional<active_record::string>, T> exec(query_relation<T> query){
            T result;
            char* errmsg_ptr = nullptr;
            const auto result_code = sqlite3_exec(db_obj,
                query.to_sql().c_str(),
                &sqlite3::callback<T>,
                &result,
                &errmsg_ptr
            );
            const auto errmsg = get_error_msg(result_code, errmsg_ptr);
            error_msg = errmsg;
            if(errmsg_ptr != nullptr){
                sqlite3_free(errmsg_ptr);
            }
            return {errmsg, result};
        }
        std::optional<active_record::string> exec(query_relation<bool> query){
            char* errmsg_ptr = nullptr;
            auto result_code = sqlite3_exec(db_obj,
                query.to_sql().c_str(),
                nullptr,
                nullptr,
                &errmsg_ptr
            );
            const auto errmsg = get_error_msg(result_code, errmsg_ptr);
            error_msg = errmsg;
            if(errmsg_ptr != nullptr){
                sqlite3_free(errmsg_ptr);
            }
            return errmsg;
        }

        std::optional<active_record::string> begin(const active_record::string_view transaction_name = ""){
            return exec(raw_query<bool>(active_record::string{ "BEGIN TRANSACTION " } + active_record::string{ transaction_name }));
        }
        std::optional<active_record::string> commit(const active_record::string_view transaction_name = ""){
            return exec(raw_query<bool>(active_record::string{ "COMMIT TRANSACTION " } + active_record::string{ transaction_name }));
        }
        std::optional<active_record::string> rollback(const active_record::string_view transaction_name = ""){
            return exec(raw_query<bool>(active_record::string{ "ROLLBACK TRANSACTION " } + active_record::string{ transaction_name }));
        }

        template<std::convertible_to<std::function<sqlite3::transaction(void)>> F>
        std::pair<std::optional<active_record::string>, sqlite3::transaction> transaction(F& func) {
            if(const auto errmsg = begin(); errmsg) return { errmsg, sqlite3::transaction::rollback };
            const auto transaction_result = func();
            switch (transaction_result) {
            case sqlite3::transaction::commit:
                return { commit(), sqlite3::transaction::commit };
            default:
                return { rollback(), sqlite3::transaction::rollback };
            }
        }
        template<std::convertible_to<std::function<sqlite3::transaction(void)>> F>
        std::pair<std::optional<active_record::string>, sqlite3::transaction>  transaction(F&& func) {
            return transaction(func);
        }
        template<std::convertible_to<std::function<sqlite3::transaction(sqlite3_adaptor&)>> F>
        std::pair<std::optional<active_record::string>, sqlite3::transaction> transaction(F& func) {
            return transaction(static_cast<std::function<sqlite3::transaction()>>(std::bind(func, std::ref(*this))));
        }
        template<std::convertible_to<std::function<sqlite3::transaction(sqlite3_adaptor&)>> F>
        std::pair<std::optional<active_record::string>, sqlite3::transaction>  transaction(F&& func) {
            return transaction(func);
        }

        template<Attribute T>
        static active_record::string column_definition() {
            return sqlite3::column_definition<T>();
        }
    };

    inline namespace sqlite3_string_convertors{
        // boolean
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, bool>
        [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
            return static_cast<bool>(attr) ? (attr.value() ? "1" : "0") : "null";
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, bool>
        void from_string(Attr& attr, const active_record::string_view str){
            if(str != "null"){
                attr = ((str == "0") ? false : true);
            }
        }

        // integer
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::integral<typename Attr::value_type>
        [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
            return to_string<common_adaptor>(attr);
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::integral<typename Attr::value_type>
        void from_string(Attr& attr, const active_record::string_view str) {
            from_string<common_adaptor>(attr, str);
        }

        // decimal
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::floating_point<typename Attr::value_type>
        [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
            return to_string<common_adaptor>(attr);
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::floating_point<typename Attr::value_type>
        void from_string(Attr& attr, const active_record::string_view str){
            from_string<common_adaptor>(attr, str);
        }

        // string
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::string>
        [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
            return to_string<common_adaptor>(attr);
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::string>
        void from_string(Attr& attr, const active_record::string_view str) {
            from_string<common_adaptor>(attr, str);
        }

        // datetime
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::datetime>
        [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
            // ISO 8601 yyyyMMddTHHmmss (sqlite supports only utc)
            //return static_cast<bool>(attr) ? std::format("%FT%T", attr.value()) : "null";
            return "";
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::datetime>
        void from_string(Attr& attr, const active_record::string_view str){
            active_record::datetime dt;
            //std::chrono::parse("%fT%T", dt, str);
            attr = dt;
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        [[nodiscard]] constexpr active_record::string to_string(const Attr& attr) {
            active_record::string hex = "x'";
            char buf[4];
            for(const auto b : attr.value()){
                hex += active_record::string_view{buf, std::to_chars(buf, &buf[3], b, 16) + 1};
            }
            return static_cast<bool>(attr) ? hex + "'" : "null";
        }
        template<std::same_as<sqlite3_adaptor> Adaptor, Attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        void from_string(Attr& attr, const active_record::string_view str){
            attr = std::vector<std::byte>{};
            attr.value().reserve(str.size());
            std::copy(str.begin(), str.end(), attr.value().begin());
        }
    }
}
#else
#include <type_traits>
namespace active_record {

    template<typename LazyInstantiate>
    struct sqlite3_adaptor_impl{static_assert(SFINAE::value, "sqlite3.h is not found");};
    
    using sqlite3_adaptor = sqlite3_adaptor_impl<std::false_type>;
}
#endif