#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
namespace arcxx {
    inline postgresql_connector::postgresql_connector(const PostgreSQL::endpoint& endpoint_info, const std::optional<PostgreSQL::auth>& auth_info, const std::optional<PostgreSQL::options> option) {
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
            error_msg = PQerrorMessage(conn);
        }
    }
    inline postgresql_connector::postgresql_connector(const arcxx::string& info){
        conn = PQconnectdb(info.c_str());
        if (PQstatus(conn) == CONNECTION_BAD){
            // handle error
            error_msg = PQerrorMessage(conn);
        }
    }

    inline postgresql_connector::postgresql_connector(postgresql_connector&& src){
        conn = src.conn;
        src.conn = nullptr;

        this->error_msg = std::move(src.error_msg);
    }

    inline postgresql_connector::~postgresql_connector() {
        this->close();
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline PGresult* postgresql_connector::exec_sql(const query_relation<Result, BindAttrs>& query) {
        const auto sql = query.template to_sql<postgresql_connector>();

        PGresult* result = nullptr;
        if constexpr(query.bind_attrs_count() == 0){
            result = PQexec(
                conn,
                sql.c_str()
            );
        }
        else{
            const auto param_length = std::apply(
                []<typename... Attrs>(const Attrs&... attrs){
                    return std::array<int, sizeof...(Attrs)>{ static_cast<int>(PostgreSQL::detail::attribute_size(attrs))... };
                },
                query.bind_attrs
            );
            const auto param_format = std::apply(
                []<typename... Attrs>(const Attrs&... attrs){
                    const auto is_not_text_format = []<typename Attr>([[maybe_unused]]const Attr&){
                        return static_cast<int>(
                            !(std::is_same_v<typename Attr::value_type, arcxx::string> ||
                            regarded_as_clock<typename Attr::value_type>) ||
                            std::floating_point<typename Attr::value_type>
                        );
                    };
                    return std::array<int, sizeof...(Attrs)>{ is_not_text_format(attrs)... };
                },
                query.bind_attrs
            );

            std::array<std::any, query.bind_attrs_count()> temporary_values;
            const auto param_values = std::apply(
                []<typename... Ptrs>(const Ptrs... ptrs){ return std::array<const char* const, sizeof...(Ptrs)>{ptrs...}; },
                tuptup::indexed_apply_each(
                    [&temporary_values]<std::size_t N, typename Attr>(const Attr& attr){
                        return PostgreSQL::detail::get_value_ptr(attr, temporary_values[N]);
                    },
                    query.bind_attrs
                )
            );

            result = PQexecParams(
                conn,
                sql.c_str(),
                std::tuple_size_v<BindAttrs>, // parameter count
                NULL, // parameter types
                param_values.data(), // parameter values
                param_length.data(), // parameter length
                param_format.data(), // parameter formats
                0  // result formats(text)
            );
        }
        return result;
    }
    inline bool postgresql_connector::has_error() const noexcept {
        return static_cast<bool>(error_msg);
    }
    inline const arcxx::string& postgresql_connector::error_message() const {
        return error_msg.value();
    }

    inline postgresql_connector postgresql_connector::open(const PostgreSQL::endpoint endpoint_info, const std::optional<PostgreSQL::auth> auth_info, const std::optional<PostgreSQL::options> option){
        return postgresql_connector(endpoint_info, auth_info, option);
    }
    inline postgresql_connector postgresql_connector::open(const arcxx::string& connection_info){
        return postgresql_connector(connection_info);
    }

    inline int postgresql_connector::protocol_version() const {
        return PQprotocolVersion(conn);
    }
    inline int postgresql_connector::server_version() const {
        return PQserverVersion(conn);
    }

    inline void postgresql_connector::close() {
        if (conn != nullptr){
            PQfinish(conn);
            conn = nullptr;
        }
    }
    inline arcxx::string postgresql_connector::bind_variable_str(const std::size_t idx, arcxx::string&& buff) {
        std::array<arcxx::string::value_type, 8> char_buff{0};
        std::to_chars(std::to_address(char_buff.begin()), std::to_address(char_buff.end()), idx+1);
        buff.reserve(buff.size() + 8);
        buff += "$";
        buff += char_buff.data();
        return std::move(buff);
    }

    template<specialized_from<std::vector> Result, specialized_from<std::tuple> BindAttrs>
    inline auto postgresql_connector::make_executer(const query_relation<Result, BindAttrs>& query) -> arcxx::expected<executer<typename Result::value_type>, arcxx::string>{
        return 
    }
    template<specialized_from<std::vector> Result, specialized_from<std::tuple> BindAttrs>
    inline auto postgresql_connector::make_executer(query_relation<Result, BindAttrs>&& query) -> arcxx::expected<executer<typename Result::value_type>, arcxx::string>{

    }

    template<specialized_from<std::unordered_map> Result, specialized_from<std::tuple> BindAttrs>
    inline auto postgresql_connector::make_executer(const query_relation<Result, BindAttrs>& query) -> arcxx::expected<executer<std::pair<typename Result::key_type, typename Result::mapped_type>>, arcxx::string>{

    }
    template<specialized_from<std::unordered_map> Result, specialized_from<std::tuple> BindAttrs>
    inline auto postgresql_connector::make_executer(query_relation<Result, BindAttrs>&& query) -> arcxx::expected<executer<std::pair<typename Result::key_type, typename Result::mapped_type>>, arcxx::string>{

    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline auto postgresql_connector::make_executer(const query_relation<Result, BindAttrs>& query) -> arcxx::expected<executer<Result>, arcxx::string>{

    }
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline auto postgresql_connector::make_executer(query_relation<Result, BindAttrs>&& query) -> arcxx::expected<executer<Result>, arcxx::string>{

    }

    template<is_model Mod>
    inline arcxx::expected<void, arcxx::string> postgresql_connector::create_table(decltype(abort_if_exists)){
        return exec(raw_query<void>(Mod::schema::template to_sql<postgresql_connector>(abort_if_exists)));
    }
    template<is_model Mod>
    inline arcxx::expected<void, arcxx::string> postgresql_connector::create_table(){
        return exec(raw_query<void>(Mod::schema::template to_sql<postgresql_connector>()));
    }

    template<is_model Mod>
    inline arcxx::expected<void, arcxx::string> postgresql_connector::drop_table(){
        return exec(raw_query<void>("DROP TABLE ", Mod::table_name,";"));
    }

    template<is_model Mod>
    inline bool postgresql_connector::exists_table(){
        const auto result = exec(raw_query<int>("SELECT COUNT(*) FROM information_schema.tables ",
            "WHERE table_name = '", Mod::table_name, "';"
        ));
        return result.value();
    }

    namespace detail{
        template<typename Result>
        inline void set_result(Result& ret, PGresult* result, const auto col){
            ret = arcxx::PostgreSQL::detail::extract_column_data<Result>(result, col);
        }
        template<specialized_from<std::vector> Result>
        inline void set_result(Result& ret, PGresult* result, const auto col){
            ret.push_back(PostgreSQL::detail::extract_column_data<typename Result::value_type>(result, col));
        }
        template<specialized_from<std::unordered_map> Result>
        inline void set_result(Result& ret, PGresult* result, const auto col){
            if constexpr (specialized_from<typename Result::mapped_type, std::tuple>){
                using result_type = tuptup::tuple_cat_t<std::tuple<typename Result::key_type>, typename Result::mapped_type>;
                auto result_column = arcxx::PostgreSQL::detail::extract_column_data<result_type>(result, col);
                ret.insert(std::make_pair(
                    std::get<0>(result_column),
                    tuptup::tuple_slice<tuptup::make_index_range<1, std::tuple_size_v<result_type>>>(result_column)
                ));
            }
            else{
                auto result_column = arcxx::PostgreSQL::detail::extract_column_data<std::tuple<typename Result::key_type, typename Result::mapped_type>>(result, col);
                ret.insert(std::make_pair(std::move(std::get<0>(result_column)), std::move(std::get<1>(result_column))));
            }
        }
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline arcxx::expected<Result, arcxx::string> postgresql_connector::exec(const query_relation<Result, BindAttrs>& query){
        PGresult* result = this->exec_sql(query);

        Result ret{};
        // error handling
        if (PQresultStatus(result) != PGRES_TUPLES_OK) {
            error_msg = PQresultErrorMessage(result);
            if(result != nullptr) PQclear(result);
            return arcxx::make_unexpected(error_msg.value());
        }

        auto col_count = PQntuples(result);
        for(decltype(col_count) col = 0; col < col_count; ++col){
            detail::set_result(ret, result, col);
        }

        if(result != nullptr) PQclear(result);
        error_msg = std::nullopt;
        return ret;
    }

    template<specialized_from<std::tuple> BindAttrs>
    inline arcxx::expected<void, arcxx::string> postgresql_connector::exec(const query_relation<void, BindAttrs>& query){
        PGresult* result = this->exec_sql(query);

        if (const auto stat = PQresultStatus(result); stat == PGRES_COMMAND_OK || stat == PGRES_NONFATAL_ERROR){
            error_msg = std::nullopt;
        }
        else error_msg = PQresultErrorMessage(result);

        if(result != nullptr) PQclear(result);
        
        if(error_msg){
            return arcxx::make_unexpected(error_msg.value());
        }
        else{
            return {};
        }
    }

    inline arcxx::expected<void, arcxx::string> postgresql_connector::begin(){
        return exec(raw_query<void>("BEGIN"));
    }
    inline arcxx::expected<void, arcxx::string> postgresql_connector::commit(){
        return exec(raw_query<void>("COMMIT"));
    }
    inline arcxx::expected<void, arcxx::string> postgresql_connector::rollback(){
        return exec(raw_query<void>("ROLLBACK"));
    }

    template<typename F>
    requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t()>>
    inline arcxx::expected<void, arcxx::string> postgresql_connector::transaction(F&& func) {
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
            return arcxx::expected<void, arcxx::string>{};;
        }
    }

    template<typename F>
    requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t(postgresql_connector&)>>
    inline arcxx::expected<void, arcxx::string> postgresql_connector::transaction(F&& func) {
        return transaction([this, &func](){ return func(*this); });
    }

    template<is_attribute Attr>
    inline arcxx::string postgresql_connector::column_definition() {
        return arcxx::PostgreSQL::column_definition<Attr>();
    }
}