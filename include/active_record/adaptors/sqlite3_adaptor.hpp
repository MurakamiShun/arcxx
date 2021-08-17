#pragma once
#include "../adaptor.hpp"
#include "../utils.hpp"
#include "../query.hpp"
#include "../attribute.hpp"
#include <sqlite3.h>
#include "sqlite3/utils.hpp"
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
                        [stmt]<typename... Attrs>(const Attrs&... attr_ptrs){ return (active_record::sqlite3::detail::bind_variable(stmt, attr_ptrs.first, *(attr_ptrs.second)) + ...);},
                        query.bind_attrs
                    );
                    if(result_code != SQLITE_OK) return result_code;
                }

                while(true){
                    const auto status = sqlite3_step(stmt);
                    if(status == SQLITE_ROW){
                        if constexpr (same_as_vector<T>) {
                            result.push_back(active_record::sqlite3::detail::extract_column_data<typename T::value_type>(stmt));
                        }
                        else if constexpr (same_as_unordered_map<T>) {
                            if constexpr (Tuple<typename T::mapped_type>){
                                using result_type = decltype(std::tuple_cat(std::declval<std::tuple<typename T::key_type>>(), std::declval<typename T::mapped_type>()));
                                auto result_column = active_record::sqlite3::detail::extract_column_data<result_type>(stmt);
                                result.insert(std::make_pair(
                                    std::get<0>(result_column),
                                    active_record::tuple_slice(result_column, active_record::make_index_sequence_between<1, std::tuple_size_v<result_type>>())
                                ));
                            }
                            else{
                                const auto result_column = active_record::sqlite3::detail::extract_column_data<std::tuple<typename T::key_type, typename T::mapped_type>>(stmt);
                                result.insert(std::make_pair(std::move(std::get<0>(result_column)), std::move(std::get<1>(result_column))));
                            }
                        }
                        else {
                            result = active_record::sqlite3::detail::extract_column_data<T>(stmt);
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
                    [stmt]<typename... Attrs>(const Attrs&... attr_ptrs){ return (active_record::sqlite3::detail::bind_variable(stmt, attr_ptrs.first, *(attr_ptrs.second)) + ...);},
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