#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include <iostream>
namespace active_record {
    inline sqlite3_adaptor::sqlite3_adaptor(const active_record::string& file_name, const int flags){
        auto result = sqlite3_open_v2(file_name.c_str(), &db_obj, flags, nullptr);
        if(result != SQLITE_OK) error_msg = get_error_msg(result);
    }

    inline std::optional<active_record::string> sqlite3_adaptor::get_error_msg(const char* msg_ptr) const {
        if(msg_ptr == nullptr) return std::nullopt;
        else return std::make_optional<active_record::string>(msg_ptr);
    }
    inline std::optional<active_record::string> sqlite3_adaptor::get_error_msg(const int result_code) const {
        if(result_code == SQLITE_OK) return std::nullopt;
        else return get_error_msg();
    }
    inline std::optional<active_record::string> sqlite3_adaptor::get_error_msg() const {
        return get_error_msg(sqlite3_errmsg(db_obj));
    }

    inline bool sqlite3_adaptor::has_error() const noexcept {
        return static_cast<bool>(error_msg);
    }
    inline const active_record::string& sqlite3_adaptor::error_message() const {
        return error_msg.value();
    }

    inline sqlite3_adaptor sqlite3_adaptor::open(const active_record::string& file_name, const int flags){
        return sqlite3_adaptor{ file_name, flags };
    }
    inline void sqlite3_adaptor::close() {
        if (db_obj != nullptr){
            sqlite3_close(db_obj);
            db_obj = nullptr;
        }
    }

    inline sqlite3_adaptor::sqlite3_adaptor(sqlite3_adaptor&& adpt){
        this->db_obj = adpt.db_obj;
        adpt.db_obj = nullptr;

        this->error_msg = std::move(adpt.error_msg);
    }

    inline sqlite3_adaptor::~sqlite3_adaptor(){
        this->close();
    }

    inline active_record::string_view sqlite3_adaptor::version(){
        return sqlite3_version;
    }
    inline int sqlite3_adaptor::version_number(){
        return sqlite3_libversion_number();
    }
    inline active_record::string sqlite3_adaptor::bind_variable_str(const std::size_t idx) {
        // variable number must be between ?1 and ?250000
        return concat_strings("?", std::to_string(idx + 1));
    }

    namespace detail{
        template<typename Result>
        inline void set_result(Result& result, sqlite3_stmt* stmt){
            result = active_record::sqlite3::detail::extract_column_data<Result>(stmt);
        }
        template<specialized_from<std::vector> Result>
        inline void set_result(Result& result, sqlite3_stmt* stmt){
            result.push_back(active_record::sqlite3::detail::extract_column_data<typename Result::value_type>(stmt));
        }
        template<specialized_from<std::unordered_map> Result>
        inline void set_result(Result& result, sqlite3_stmt* stmt){
            if constexpr (specialized_from<typename Result::mapped_type, std::tuple>){
                using result_type = tuptup::tuple_cat_t<
                    std::tuple<typename Result::key_type>,
                    std::conditional_t<specialized_from<typename Result::mapped_type, std::tuple>, typename Result::mapped_type, std::tuple<>>
                >;
                auto result_column = active_record::sqlite3::detail::extract_column_data<result_type>(stmt);
                result.insert(std::make_pair(
                    std::get<0>(result_column),
                    tuptup::tuple_slice<tuptup::make_index_range<1, std::tuple_size_v<result_type>>>(result_column)
                ));
            }
            else{
                using result_type = std::tuple<typename Result::key_type, typename Result::mapped_type>;
                auto result_column = active_record::sqlite3::detail::extract_column_data<result_type>(stmt);
                result.insert(std::make_pair(std::move(std::get<0>(result_column)), std::move(std::get<1>(result_column))));
            }
        }
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline active_record::expected<Result, active_record::string> sqlite3_adaptor::exec(const query_relation<Result, BindAttrs>& query){
        Result result;
        const auto result_code = [this, &query, &result](){
            const auto sql = query.template to_sql<sqlite3_adaptor>();
            sqlite3_stmt* stmt = nullptr;
            auto result_code = sqlite3_prepare_v2(
                db_obj,
                sql.c_str(),
                static_cast<int>(sql.size()),
                &stmt,
                nullptr
            );
            if(result_code != SQLITE_OK) return result_code;

            if constexpr(query.bind_attrs_count() != 0) {
                result_code = SQLITE_OK;
                tuptup::indexed_apply_each(
                    [stmt, &result_code]<std::size_t N, typename Attr>(const Attr& attr){
                        const auto res = active_record::sqlite3::detail::bind_variable(stmt, N, attr);
                        if(res != SQLITE_OK) result_code = res;
                    },
                    query.bind_attrs
                );
                if(result_code != SQLITE_OK) return result_code;
            }

            while(true){
                const auto status = sqlite3_step(stmt);
                if(status == SQLITE_ROW){
                    detail::set_result(result, stmt);
                }
                else if (status == SQLITE_DONE) break;
                else return status;
            }
            result_code = sqlite3_finalize(stmt);
            return result_code;
        }();

        error_msg = get_error_msg(result_code);
        if(error_msg){
            return active_record::make_unexpected(error_msg.value());
        }
        else{
            return result;
        }
    }

    template<specialized_from<std::tuple> BindAttrs>
    inline active_record::expected<void, active_record::string> sqlite3_adaptor::exec(const query_relation<void, BindAttrs>& query){
        const auto result_code = [this, &query](){
            const auto sql = query.template to_sql<sqlite3_adaptor>();
            sqlite3_stmt* stmt = nullptr;
            auto result_code = sqlite3_prepare_v2(
                db_obj,
                sql.c_str(),
                static_cast<int>(sql.size()),
                &stmt,
                nullptr
            );
            if(result_code != SQLITE_OK) return result_code;

            if constexpr(query.bind_attrs_count() != 0) {
                result_code = SQLITE_OK;
                tuptup::indexed_apply_each(
                    [stmt, &result_code]<std::size_t N, typename Attr>(const Attr& attr){
                        const auto res = active_record::sqlite3::detail::bind_variable(stmt, N, attr);
                        if (res != SQLITE_OK) result_code = res;
                    },
                    query.bind_attrs
                );
            }
            if(result_code != SQLITE_OK) return result_code;

            result_code = sqlite3_step(stmt);
            if(result_code != SQLITE_DONE) return result_code;

            result_code = sqlite3_finalize(stmt);
            return result_code;
        }();
        error_msg = get_error_msg(result_code);
        if(error_msg){
            return active_record::make_unexpected(error_msg.value());
        }
        else{
            return active_record::expected<void, active_record::string>{};
        }
    }

    template<is_model Mod>
    inline active_record::expected<void, active_record::string> sqlite3_adaptor::create_table(decltype(abort_if_exists)){
        return exec(raw_query<void>(Mod::schema::template to_sql<sqlite3_adaptor>(abort_if_exists)));
    }
    template<is_model Mod>
    inline active_record::expected<void, active_record::string> sqlite3_adaptor::create_table(){
        return exec(raw_query<void>(Mod::schema::template to_sql<sqlite3_adaptor>()));
    }

    template<is_model Mod>
    inline active_record::expected<void, active_record::string> sqlite3_adaptor::drop_table(){
        return exec(raw_query<void>("DROP TABLE ", Mod::table_name, ";"));
    }

    template<is_model Mod>
    inline bool sqlite3_adaptor::exists_table(){
        const auto result = exec(raw_query<int>("SELECT COUNT(*) FROM sqlite_master WHERE type = \"table\" AND name = \"", Mod::table_name, "\";"));
        return result.value();
    }

    inline active_record::expected<void, active_record::string> sqlite3_adaptor::begin(const active_record::string_view transaction_name){
        return exec(raw_query<void>("BEGIN TRANSACTION ", transaction_name, ";"));
    }
    inline active_record::expected<void, active_record::string> sqlite3_adaptor::commit(const active_record::string_view transaction_name){
        return exec(raw_query<void>("COMMIT TRANSACTION ", transaction_name, ";"));
    }
    inline active_record::expected<void, active_record::string> sqlite3_adaptor::rollback(const active_record::string_view transaction_name){
        return exec(raw_query<void>("ROLLBACK TRANSACTION ", transaction_name, ";"));
    }

    template<typename F>
    requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t()>>
    inline active_record::expected<void, active_record::string> sqlite3_adaptor::transaction(F&& func) {
        if(auto begin_result = begin(); !begin_result) {
            return active_record::make_unexpected(std::move(begin_result).error());
        }
        auto trans_result = func();
        if(trans_result.should_rollback()) {
            if(auto rollback_result = rollback(); !rollback_result) {
                return active_record::make_unexpected(std::move(rollback_result).error());
            }
            else {
                return active_record::make_unexpected(std::move(trans_result).rollback_reason.value());
            }
        }
        else { // commit
            if(auto commit_result = commit(); !commit_result){
                return active_record::make_unexpected(std::move(commit_result).error());
            }
            return active_record::expected<void, active_record::string>{};
        }
    }
    template<typename F>
    requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t(sqlite3_adaptor&)>>
    inline active_record::expected<void, active_record::string> sqlite3_adaptor::transaction(F&& func) {
        return transaction([this, &func](){ return func(*this); });
    }

    template<is_attribute Attr>
    inline active_record::string sqlite3_adaptor::column_definition() {
        return active_record::sqlite3::column_definition<Attr>();
    }
}