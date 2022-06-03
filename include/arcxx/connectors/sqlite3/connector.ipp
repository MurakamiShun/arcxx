#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
namespace arcxx {
    namespace sqlite3::detail{
        template<is_attribute Attr>
        requires std::integral<typename Attr::value_type>
        inline int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
            if(!attr) {
                return sqlite3_bind_null(stmt, static_cast<int>(index + 1));
            }
            else {
                if constexpr (sizeof(typename Attr::value_type) == sizeof(int64_t)) {
                    return sqlite3_bind_int64(stmt, static_cast<int>(index + 1), attr.value());
                }
                else {
                    return sqlite3_bind_int(stmt, static_cast<int>(index + 1), attr.value());
                }
            }
        }

        template<is_attribute Attr>
        requires std::floating_point<typename Attr::value_type>
        inline int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
            if(!attr) {
                return sqlite3_bind_null(stmt, static_cast<int>(index + 1));
            }
            else {
                return sqlite3_bind_double(stmt, static_cast<int>(index + 1), static_cast<double>(attr.value()));
            }
        }

        template<is_attribute Attr>
        requires std::same_as<typename Attr::value_type, arcxx::string>
        inline int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
            if(!attr) {
                return sqlite3_bind_null(stmt, static_cast<int>(index + 1));
            }
            else {
                // not copy text
                return sqlite3_bind_text(stmt, static_cast<int>(index + 1), attr.value().c_str(), static_cast<int>(attr.value().length()), SQLITE_STATIC);
            }
        }
        template<is_attribute Attr>
        requires regarded_as_clock<typename Attr::value_type>
        inline int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
            if(!attr) {
                return sqlite3_bind_null(stmt, static_cast<int>(index + 1));
            }
            else {
                const arcxx::string time_str = to_string<sqlite3_connector>(attr);
                // copy text
                return sqlite3_bind_text(stmt, static_cast<int>(index + 1), time_str.c_str(), static_cast<int>(time_str.length()), SQLITE_TRANSIENT);
            }
        }
        template<is_attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        inline int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
            if(!attr) {
                return sqlite3_bind_null(stmt, static_cast<int>(index + 1));
            }
            else {
                // not copy text
                return sqlite3_bind_blob(stmt, static_cast<int>(index + 1), attr.value().data(), static_cast<int>(attr.value().size()), SQLITE_STATIC);
            }
        }
    }

    inline sqlite3_connector::sqlite3_connector(const arcxx::string& file_name, const int flags){
        auto result = sqlite3_open_v2(file_name.c_str(), &db_obj, flags, nullptr);
        if(result != SQLITE_OK) error_msg = get_error_msg(result);
    }

    inline std::optional<arcxx::string> sqlite3_connector::get_error_msg(const char* msg_ptr) const {
        if(msg_ptr == nullptr) return std::nullopt;
        else return std::make_optional<arcxx::string>(msg_ptr);
    }
    inline std::optional<arcxx::string> sqlite3_connector::get_error_msg(const int result_code) const {
        if(result_code == SQLITE_OK) return std::nullopt;
        else return get_error_msg();
    }
    inline std::optional<arcxx::string> sqlite3_connector::get_error_msg() const {
        return get_error_msg(sqlite3_errmsg(db_obj));
    }

    inline bool sqlite3_connector::has_error() const noexcept {
        return static_cast<bool>(error_msg);
    }
    inline const arcxx::string& sqlite3_connector::error_message() const {
        return error_msg.value();
    }

    inline sqlite3_connector sqlite3_connector::open(const arcxx::string& file_name, const int flags){
        return sqlite3_connector{ file_name, flags };
    }
    inline void sqlite3_connector::close() {
        if (db_obj != nullptr){
            sqlite3_close(db_obj);
            db_obj = nullptr;
        }
    }

    inline sqlite3_connector::sqlite3_connector(sqlite3_connector&& conn){
        this->db_obj = conn.db_obj;
        conn.db_obj = nullptr;

        this->error_msg = std::move(conn.error_msg);
    }

    inline sqlite3_connector::~sqlite3_connector(){
        this->close();
    }

    inline arcxx::string_view sqlite3_connector::version(){
        return sqlite3_version;
    }
    inline int sqlite3_connector::version_number(){
        return sqlite3_libversion_number();
    }
    inline arcxx::string sqlite3_connector::bind_variable_str(const std::size_t idx, arcxx::string&& buff) {
        // variable number must be between ?1 and ?250000
        std::array<arcxx::string::value_type, 8> char_buff{0};
        std::to_chars(&(*char_buff.begin()), &(*char_buff.end()), idx+1);
        buff.reserve(buff.size() + 8);
        buff += "?";
        buff += char_buff.data();
        return std::move(buff);
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline arcxx::expected<::sqlite3_stmt*, arcxx::string> sqlite3_connector::make_stmt_and_bind(const query_relation<Result, BindAttrs>& query){
        const auto sql = query.template to_sql<sqlite3_connector>();
        ::sqlite3_stmt* stmt = nullptr;
        auto result_code = sqlite3_prepare_v2(
            db_obj,
            sql.c_str(),
            static_cast<int>(sql.size()),
            &stmt,
            nullptr
        );
        if(result_code != SQLITE_OK){
            return arcxx::make_unexpected(get_error_msg(result_code).value());
        }

        if constexpr(query.bind_attrs_count() != 0) {
            result_code = SQLITE_OK;
            tuptup::indexed_apply_each(
                [stmt, &result_code]<std::size_t N, typename Attr>(const Attr& attr){
                    const auto res = arcxx::sqlite3::detail::bind_variable(stmt, N, attr);
                    if(res != SQLITE_OK) result_code = res;
                },
                query.bind_attrs
            );
            if(result_code != SQLITE_OK){
                return arcxx::make_unexpected(get_error_msg(result_code).value());
            }
        }
        return stmt;
    }
    template<specialized_from<std::vector> Result, specialized_from<std::tuple> BindAttrs>
    inline auto sqlite3_connector::make_executer(const query_relation<Result, BindAttrs>& query) -> arcxx::expected<executer<typename Result::value_type>, arcxx::string>{
        auto stmt_result = make_stmt_and_bind(query);
        if(!stmt_result){
            error_msg = stmt_result.error();
            return arcxx::make_unexpected(std::move(stmt_result.error()));
        }
        else return executer<typename Result::value_type>(stmt_result.value());
    }
    template<specialized_from<std::unordered_map> Result, specialized_from<std::tuple> BindAttrs>
    inline auto sqlite3_connector::make_executer(const query_relation<Result, BindAttrs>& query) -> arcxx::expected<executer<std::pair<typename Result::key_type, typename Result::mapped_type>>, arcxx::string>{
        auto stmt_result = make_stmt_and_bind(query);
        if(!stmt_result){
            error_msg = stmt_result.error();
            return arcxx::make_unexpected(std::move(stmt_result.error()));
        }
        else return executer<std::pair<typename Result::key_type, typename Result::mapped_type>>(stmt_result.value());
    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline auto sqlite3_connector::make_executer(const query_relation<Result, BindAttrs>& query) -> arcxx::expected<executer<Result>, arcxx::string>{
        auto stmt_result = make_stmt_and_bind(query);
        if(!stmt_result){
            error_msg = stmt_result.error();
            return arcxx::make_unexpected(std::move(stmt_result.error()));
        }
        else{
            ::sqlite3_stmt* stmt = stmt_result.value();
            return executer<Result>(stmt);
        }
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline arcxx::expected<Result, arcxx::string> sqlite3_connector::exec(const query_relation<Result, BindAttrs>& query){
        auto make_executer_result = make_executer(query);
        if(!make_executer_result){
            error_msg = make_executer_result.error();
            return arcxx::make_unexpected(std::move(make_executer_result.error()));
        }

        Result result{};
        for(auto exec_result : make_executer_result.value()){
            if(!exec_result) return arcxx::make_unexpected(exec_result.error());
            else{
                if constexpr(specialized_from<Result, std::vector>){
                    result.push_back(exec_result.value().get());
                }
                else if constexpr(specialized_from<Result, std::unordered_map>){
                    result.insert(exec_result.value().get());
                }
                else{
                    result = exec_result.value().get();
                }
            }
        }
        return result;
    }

    template<specialized_from<std::tuple> BindAttrs>
    inline arcxx::expected<void, arcxx::string> sqlite3_connector::exec(const query_relation<void, BindAttrs>& query){
        auto make_executer_result = make_executer(query);
        if(!make_executer_result){
            error_msg = make_executer_result.error();
            return arcxx::make_unexpected(std::move(make_executer_result.error()));
        }

        if(auto exec_result = make_executer_result.value().execute(); !exec_result){
            error_msg = exec_result.error();
            return arcxx::make_unexpected(std::move(exec_result.error()));
        }
        else return arcxx::expected<void, arcxx::string>{};
    }

    template<is_model Mod>
    inline arcxx::expected<void, arcxx::string> sqlite3_connector::create_table(decltype(abort_if_exists)){
        return exec(raw_query<void>(Mod::schema::template to_sql<sqlite3_connector>(abort_if_exists)));
    }
    template<is_model Mod>
    inline arcxx::expected<void, arcxx::string> sqlite3_connector::create_table(){
        return exec(raw_query<void>(Mod::schema::template to_sql<sqlite3_connector>()));
    }

    template<is_model Mod>
    inline arcxx::expected<void, arcxx::string> sqlite3_connector::drop_table(){
        return exec(raw_query<void>("DROP TABLE ", Mod::table_name, ";"));
    }

    template<is_model Mod>
    inline bool sqlite3_connector::exists_table(){
        const auto result = exec(raw_query<int>("SELECT COUNT(*) FROM sqlite_master WHERE type = \"table\" AND name = \"", Mod::table_name, "\";"));
        return result.value();
    }

    inline arcxx::expected<void, arcxx::string> sqlite3_connector::begin(const arcxx::string_view transaction_name){
        return exec(raw_query<void>("BEGIN TRANSACTION ", transaction_name, ";"));
    }
    inline arcxx::expected<void, arcxx::string> sqlite3_connector::commit(const arcxx::string_view transaction_name){
        return exec(raw_query<void>("COMMIT TRANSACTION ", transaction_name, ";"));
    }
    inline arcxx::expected<void, arcxx::string> sqlite3_connector::rollback(const arcxx::string_view transaction_name){
        return exec(raw_query<void>("ROLLBACK TRANSACTION ", transaction_name, ";"));
    }

    template<typename F>
    requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t()>>
    inline arcxx::expected<void, arcxx::string> sqlite3_connector::transaction(F&& func) {
        if(auto begin_result = begin(); !begin_result) {
            return arcxx::make_unexpected(std::move(begin_result).error());
        }
        auto trans_result = func();
        if(trans_result.should_rollback()) {
            if(auto rollback_result = rollback(); !rollback_result) {
                return arcxx::make_unexpected(std::move(rollback_result).error());
            }
            else {
                return arcxx::make_unexpected(std::move(trans_result).rollback_reason.value());
            }
        }
        else { // commit
            if(auto commit_result = commit(); !commit_result){
                return arcxx::make_unexpected(std::move(commit_result).error());
            }
            return arcxx::expected<void, arcxx::string>{};
        }
    }
    template<typename F>
    requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t(sqlite3_connector&)>>
    inline arcxx::expected<void, arcxx::string> sqlite3_connector::transaction(F&& func) {
        return transaction([this, &func](){ return func(*this); });
    }

    template<is_attribute Attr>
    inline arcxx::string sqlite3_connector::column_definition() {
        return arcxx::sqlite3::column_definition<Attr>();
    }
}