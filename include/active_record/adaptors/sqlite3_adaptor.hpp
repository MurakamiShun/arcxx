#pragma once
#include "../adaptor.hpp"
#include "../utils.hpp"
#include "../query.hpp"
#include "../attribute.hpp"
#include <optional>
#include <sqlite3.h>
#include "sqlite3/schema.hpp"
#include "sqlite3/string_convertors.hpp"

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

        template<Attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::string>
        bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
            const auto type = sqlite3_column_type(stmt, idx);
            if(type == SQLITE_TEXT){
                auto text_ptr = sqlite3_column_text(stmt, idx);
                attr = active_record::string{ reinterpret_cast<const char*>(text_ptr) };
                return true;
            }
            else if(type == SQLITE_NULL){
                attr = std::nullopt;
                return true;
            }
            else{
                return false;
            }
        }
        template<Attribute Attr>
        requires std::integral<typename Attr::value_type>
        bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
            const auto type = sqlite3_column_type(stmt, idx);
            if(type == SQLITE_INTEGER){
                if constexpr(sizeof(typename Attr::value_type) == sizeof(int64_t)){
                    attr = sqlite3_column_int64(stmt, idx);
                }
                else{
                    attr = sqlite3_column_int(stmt, idx);
                }
                return true;
            }
            else if(type == SQLITE_NULL){
                attr = std::nullopt;
                return true;
            }
            else{
                return false;
            }
        }
        template<Attribute Attr>
        requires std::floating_point<typename Attr::value_type>
        bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
            const auto type = sqlite3_column_type(stmt, idx);
            if(type == SQLITE_FLOAT){
                attr = sqlite3_column_double(stmt, idx);
                return true;
            }
            else if(type == SQLITE_NULL){
                attr = std::nullopt;
                return true;
            }
            else{
                return false;
            }
        }
        template<Attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
            const auto type = sqlite3_column_type(stmt, idx);
            if(type == SQLITE_BLOB){
                auto ptr = sqlite3_column_blob(stmt, idx);
                attr = std::vector<std::byte>{};
                const auto size = sqlite3_column_bytes(stmt, idx);
                attr.value().resize(size);
                std::copy_n(attr.value().begin(), ptr, size);
                sqlite3_free(ptr);
                return true;
            }
            else if(type == SQLITE_NULL){
                attr = std::nullopt;
                return true;
            }
            else{
                return false;
            }
        }
        template<typename T>
        requires (!Attribute<T>)
        bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, T& attr){
            const auto type = sqlite3_column_type(stmt, idx);
            if constexpr(std::is_floating_point_v<T>){
                if(type == SQLITE_FLOAT) {
                    attr = sqlite3_column_double(stmt, idx);
                    return true;
                }
            }
            else if constexpr(std::is_integral_v<T>){
                if(type == SQLITE_INTEGER) {
                    if constexpr(sizeof(T) == sizeof(int64_t)){
                        attr = sqlite3_column_int64(stmt, idx);
                    }
                    else{
                        attr = sqlite3_column_int(stmt, idx);
                    }
                    return true;
                }
            }
            return false;
        }

        template<typename T>
        requires std::derived_from<T, model<T>>
        T extract_column_data(sqlite3_stmt* stmt){
            T ret;
            indexed_apply(
                [stmt]<typename... Attrs>(Attrs... attrs){
                    return (set_column_data(stmt, attrs.first, attrs.second) && ...);
                },
                ret.attributes
            );
            return ret;
        }
        template<Tuple T>
        T extract_column_data(sqlite3_stmt* stmt){
            T ret;
            indexed_apply(
                [stmt]<typename... Attrs>(Attrs... attrs){
                    return (set_column_data(stmt, attrs.first, attrs.second) && ...);
                },
                ret
            );
            return ret;
        }
        template<typename T>
        T extract_column_data(sqlite3_stmt* stmt){
            T ret;
            set_column_data(stmt, 0, ret);
            return ret;
        }

        /* 
         * variable binders 
         */

        template<Attribute Attr>
        requires std::integral<typename Attr::value_type>
        int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
            if(!attr) {
                return sqlite3_bind_null(stmt, index + 1);
            }
            else {
                if constexpr (sizeof(typename Attr::value_type) == sizeof(int64_t)) {
                    return sqlite3_bind_int64(stmt, index + 1, attr.value());
                }
                else {
                    return sqlite3_bind_int(stmt, index + 1, attr.value());
                }
            }
        }

        template<Attribute Attr>
        requires std::floating_point<typename Attr::value_type>
        int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
            if(!attr) {
                return sqlite3_bind_null(stmt, index + 1);
            }
            else {
                return sqlite3_bind_double(stmt, index + 1, static_cast<double>(attr.value()));
            }
        }

        template<Attribute Attr>
        requires std::same_as<typename Attr::value_type, active_record::string>
        int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
            if(!attr) {
                return sqlite3_bind_null(stmt, index + 1);
            }
            else {
                // not copy text
                return sqlite3_bind_text(stmt, index + 1, attr.value().c_str(), attr.value().length(), SQLITE_STATIC);
            }
        }
        template<Attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
            if(!attr) {
                return sqlite3_bind_null(stmt, index + 1);
            }
            else {
                // not copy text
                return sqlite3_bind_blob(stmt, index + 1, attr.value().data(), attr.value().size(), SQLITE_STATIC);
            }
        }
    }

    class sqlite3_adaptor : public adaptor {
    private:
        ::sqlite3* db_obj;
        std::optional<active_record::string> error_msg = std::nullopt;

        sqlite3_adaptor() = delete;
        sqlite3_adaptor(const active_record::string& file_name, const int flags){
            auto result = sqlite3_open_v2(file_name.c_str(), &db_obj, flags, nullptr);
            if(result != SQLITE_OK) error_msg = get_error_msg(result);
        }

        std::optional<active_record::string> get_error_msg(const char* msg_ptr){
            if(msg_ptr == nullptr) return std::nullopt;
            else return active_record::string{ msg_ptr };
        }
        std::optional<active_record::string> get_error_msg(const int result_code){
            if(result_code != SQLITE_OK) return get_error_msg();
            else return std::nullopt;
        }
        std::optional<active_record::string> get_error_msg(){
            return get_error_msg(sqlite3_errmsg(db_obj));
        }
    public:
        bool has_error() const noexcept { return static_cast<bool>(error_msg); }
        const active_record::string& error_message() const { return error_msg.value(); }
        
        static sqlite3_adaptor open(const active_record::string& file_name, const int flags = active_record::sqlite3::options::readwrite){
            return sqlite3_adaptor{ file_name, flags };
        }
        bool close() {
            return sqlite3_close(db_obj) == SQLITE_OK;
        }
        ~sqlite3_adaptor(){
            sqlite3_close(db_obj);
        }

        static active_record::string_view version(){
            return sqlite3_version;
        }
        static int version_number(){
            return sqlite3_libversion_number();
        }

        static constexpr bool bindable = true;
        static active_record::string bind_variable_str(const std::size_t idx) {
            // variable number must be between ?1 and ?250000
            return active_record::string{ "?" } + std::to_string(idx + 1);
        }

        template<typename T, Tuple BindAttrs>
        [[nodiscard]] std::pair<std::optional<active_record::string>, T> exec(const query_relation<T, BindAttrs>& query){
            T result;
            const auto result_code = [this, &query, &result](){
                const auto sql = query.template to_sql<sqlite3_adaptor>();
                sqlite3_stmt* stmt = nullptr;
                auto result_code = sqlite3_prepare_v2(
                    db_obj,
                    sql.c_str(),
                    sql.size(),
                    &stmt,
                    nullptr
                );
                if(result_code != SQLITE_OK) return result_code;
                
                if constexpr(query.bind_attrs_count() != 0) {
                    result_code = indexed_apply(
                        [stmt]<typename... Attrs>(const Attrs&... attr_ptrs){ return (active_record::sqlite3::bind_variable(stmt, attr_ptrs.first, *(attr_ptrs.second)) + ...);},
                        query.bind_attrs
                    );
                    if(result_code != SQLITE_OK) return result_code;
                }

                while(true){
                    const auto status = sqlite3_step(stmt);
                    if(status == SQLITE_ROW){
                        if constexpr (Container<T>) {
                            result.push_back(active_record::sqlite3::extract_column_data<typename T::value_type>(stmt));
                        }
                        else {
                            result = active_record::sqlite3::extract_column_data<T>(stmt);
                        }
                    }
                    else if (status == SQLITE_DONE) break;
                    else return status;
                }
                result_code = sqlite3_finalize(stmt);
                return result_code;
            }();

            return {error_msg = get_error_msg(result_code), result};
        }
        template<Tuple BindAttrs>
        std::optional<active_record::string> exec(const query_relation<bool, BindAttrs>& query){
            const auto sql = query.template to_sql<sqlite3_adaptor>();
            sqlite3_stmt* stmt = nullptr;
            auto result_code = sqlite3_prepare_v2(
                db_obj,
                sql.c_str(),
                sql.size(),
                &stmt,
                nullptr
            );
            if(result_code != SQLITE_OK) return error_msg = get_error_msg(result_code);
            
            if constexpr(query.bind_attrs_count() != 0) {
                result_code = indexed_apply(
                    [stmt]<typename... Attrs>(const Attrs&... attr_ptrs){ return (active_record::sqlite3::bind_variable(stmt, attr_ptrs.first, *(attr_ptrs.second)) + ...);},
                    query.bind_attrs
                );
            }
            if(result_code != SQLITE_OK) return error_msg = get_error_msg(result_code);

            result_code = sqlite3_step(stmt);
            if(result_code != SQLITE_DONE) return error_msg = get_error_msg(result_code);

            result_code = sqlite3_finalize(stmt);
            return error_msg = get_error_msg(result_code);
        }
        template<typename T, Tuple BindAttrs>
        auto exec(const query_relation<T, BindAttrs>&& query){
            return exec(query);
        }

        template<Model Mod>
        std::optional<active_record::string> create_table(bool create_if_not_exist = false){
            char* errmsg_ptr = nullptr;
            auto result_code = sqlite3_exec(db_obj,
                Mod::schema::template to_sql<sqlite3_adaptor>(create_if_not_exist).c_str(),
                nullptr,
                nullptr,
                &errmsg_ptr
            );
            if(errmsg_ptr != nullptr){
                error_msg = get_error_msg(errmsg_ptr);
                sqlite3_free(errmsg_ptr);
                return error_msg;
            }
            return error_msg = get_error_msg(result_code);
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
            return active_record::sqlite3::column_definition<T>();
        }
    };

    namespace sqlite3 {
        using adaptor = sqlite3_adaptor;
    }
}