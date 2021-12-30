#pragma once

namespace active_record {
    inline postgresql_adaptor::postgresql_adaptor(const PostgreSQL::endpoint& endpoint_info, const std::optional<PostgreSQL::auth>& auth_info, const std::optional<PostgreSQL::options> option) {
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
    inline postgresql_adaptor::postgresql_adaptor(const active_record::string& info){
        conn = PQconnectdb(info.c_str());
        if (PQstatus(conn) == CONNECTION_BAD){
            // handle error
            error_msg = PQerrorMessage(conn);
        }
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline PGresult* postgresql_adaptor::exec_sql(const query_relation<Result, BindAttrs>& query) {
        const auto sql = query.template to_sql<postgresql_adaptor>();

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
                    return std::array<int, sizeof...(Attrs)>{ static_cast<int>(attribute_size(attrs))... };
                },
                query.bind_attrs
            );
            const auto param_format = std::apply(
                []<typename... Attrs>(const Attrs&... attrs){
                    const auto is_not_text_format = []<typename Attr>([[maybe_unused]]const Attr&){
                        return static_cast<int>(
                            !(std::is_same_v<typename Attr::value_type, active_record::string> ||
                            std::is_same_v<typename Attr::value_type, active_record::datetime>) ||
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
    inline bool postgresql_adaptor::has_error() const noexcept {
        return static_cast<bool>(error_msg);
    }
    inline const active_record::string& postgresql_adaptor::error_message() const {
        return error_msg.value();
    }

    inline postgresql_adaptor postgresql_adaptor::open(const PostgreSQL::endpoint endpoint_info, const std::optional<PostgreSQL::auth> auth_info, const std::optional<PostgreSQL::options> option){
        return postgresql_adaptor(endpoint_info, auth_info, option);
    }
    inline postgresql_adaptor postgresql_adaptor::open(const active_record::string& connection_info){
        return postgresql_adaptor(connection_info);
    }

    inline int postgresql_adaptor::protocol_version() const {
        return PQprotocolVersion(conn);
    }
    inline int postgresql_adaptor::server_version() const {
        return PQserverVersion(conn);
    }

    inline postgresql_adaptor::~postgresql_adaptor() {
        this->close();
    }

    inline void postgresql_adaptor::close() {
        if (conn != nullptr){
            PQfinish(conn);
            conn = nullptr;
        }
    }
    inline active_record::string postgresql_adaptor::bind_variable_str(const std::size_t idx) {
        return concat_strings("$", std::to_string(idx + 1));
    }

    template<is_model Mod>
    inline std::optional<active_record::string> postgresql_adaptor::create_table(bool abort_if_exist){
        return exec(raw_query<bool>(Mod::schema::template to_sql<postgresql_adaptor>(abort_if_exist)));
    }

    template<is_model Mod>
    inline std::optional<active_record::string> postgresql_adaptor::drop_table(){
        return exec(raw_query<bool>("DROP TABLE ", Mod::table_name,";"));
    }

    template<typename T, specialized_from<std::tuple> BindAttrs>
    inline auto postgresql_adaptor::exec(const query_relation<T, BindAttrs>&& query){
        return exec(query);
    }

    template<specialized_from<std::tuple> BindAttrs>
    inline auto postgresql_adaptor::exec(const query_relation<bool, BindAttrs>& query){
        PGresult* result = this->exec_sql(query);

        if (const auto stat = PQresultStatus(result); stat == PGRES_COMMAND_OK || stat == PGRES_NONFATAL_ERROR){
            error_msg = std::nullopt;
        }
        else error_msg = PQresultErrorMessage(result);

        if(result != nullptr) PQclear(result);
        return error_msg;
    }

    template<typename Result, specialized_from<std::tuple> BindAttrs>
    inline auto postgresql_adaptor::exec(const query_relation<Result, BindAttrs>& query){
        PGresult* result = this->exec_sql(query);

        Result ret;
        // error handling
        if (PQresultStatus(result) != PGRES_TUPLES_OK) {
            error_msg = PQresultErrorMessage(result);
            if(result != nullptr) PQclear(result);
            return std::make_pair(error_msg, ret);
        }

        auto col_count = PQntuples(result);
        for(decltype(col_count) col = 0; col < col_count; ++col){
            if constexpr(specialized_from<Result, std::vector>) {
                ret.push_back(PostgreSQL::detail::extract_column_data<typename Result::value_type>(result, col));
            }
            else if constexpr(specialized_from<Result, std::unordered_map>){
                if constexpr (specialized_from<typename Result::mapped_type, std::tuple>){
                    using result_type = tuptup::tuple_cat_t<std::tuple<typename Result::key_type>, typename Result::mapped_type>;
                    auto result_column = active_record::PostgreSQL::detail::extract_column_data<result_type>(result, col);
                    ret.insert(std::make_pair(
                        std::get<0>(result_column),
                        tuptup::tuple_slice<tuptup::make_index_range<1, std::tuple_size_v<result_type>>>(result_column)
                    ));
                }
                else{
                    auto result_column = active_record::PostgreSQL::detail::extract_column_data<std::tuple<typename Result::key_type, typename Result::mapped_type>>(result, col);
                    ret.insert(std::make_pair(std::move(std::get<0>(result_column)), std::move(std::get<1>(result_column))));
                }
            }
            else {
                ret = active_record::PostgreSQL::detail::extract_column_data<Result>(result, col);
            }
        }

        if(result != nullptr) PQclear(result);
        error_msg = std::nullopt;
        return std::make_pair(error_msg, ret);
    }

    inline std::optional<active_record::string> postgresql_adaptor::begin(){
        return exec(raw_query<bool>("BEGIN"));
    }
    inline std::optional<active_record::string> postgresql_adaptor::commit(){
        return exec(raw_query<bool>("COMMIT"));
    }
    inline std::optional<active_record::string> postgresql_adaptor::rollback(){
        return exec(raw_query<bool>("ROLLBACK"));
    }

    template<std::convertible_to<std::function<active_record::transaction(void)>> F>
    inline std::pair<std::optional<active_record::string>, active_record::transaction> postgresql_adaptor::transaction(F& func) {
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
    inline std::pair<std::optional<active_record::string>, active_record::transaction> postgresql_adaptor::transaction(F&& func) {
        return transaction(func);
    }
    template<std::convertible_to<std::function<active_record::transaction(postgresql_adaptor&)>> F>
    inline std::pair<std::optional<active_record::string>, active_record::transaction> postgresql_adaptor::transaction(F& func) {
        return transaction(static_cast<std::function<active_record::transaction()>>(std::bind(func, std::ref(*this))));
    }
    template<std::convertible_to<std::function<active_record::transaction(postgresql_adaptor&)>> F>
    inline std::pair<std::optional<active_record::string>, active_record::transaction> postgresql_adaptor::transaction(F&& func) {
        return transaction(func);
    }

    template<is_attribute Attr>
    inline active_record::string postgresql_adaptor::column_definition() {
        return active_record::PostgreSQL::column_definition<Attr>();
    }
}