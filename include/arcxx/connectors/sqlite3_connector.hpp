#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include <sqlite3.h>
#include "sqlite3/schema.hpp"
#include "sqlite3/string_convertors.hpp"

namespace arcxx {
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

    class sqlite3_connector : public connector {
    private:
        ::sqlite3* db_obj = nullptr;
        std::optional<arcxx::string> error_msg = std::nullopt;

        std::optional<arcxx::string> get_error_msg(const char* msg_ptr) const;
        std::optional<arcxx::string> get_error_msg(const int result_code) const;
        std::optional<arcxx::string> get_error_msg() const;

        template<typename Result, specialized_from<std::tuple> BindAttrs>
        arcxx::expected<::sqlite3_stmt*, arcxx::string> make_stmt_and_bind(const query_relation<Result, BindAttrs>& query);

        template<typename ResultType>
        class executer;
    public:
        sqlite3_connector() = delete;
        sqlite3_connector(const sqlite3_connector&) = delete;
        sqlite3_connector(sqlite3_connector&&);
        sqlite3_connector(const arcxx::string& file_name, const int flags = arcxx::sqlite3::options::readwrite);
        ~sqlite3_connector();

        bool has_error() const noexcept;
        const arcxx::string& error_message() const;

        static sqlite3_connector open(const arcxx::string& file_name, const int flags = arcxx::sqlite3::options::readwrite);
        void close();

        static arcxx::string_view version();
        static int version_number();

        static constexpr bool bindable = true;
        static arcxx::string bind_variable_str(const std::size_t idx, arcxx::string&& buff = {});

        template<specialized_from<std::vector> Result, specialized_from<std::tuple> BindAttrs>
        [[nodiscard]] auto make_executer(const query_relation<Result, BindAttrs>& query) -> arcxx::expected<executer<typename Result::value_type>, arcxx::string>;
        template<specialized_from<std::unordered_map> Result, specialized_from<std::tuple> BindAttrs>
        [[nodiscard]] auto make_executer(const query_relation<Result, BindAttrs>& query) -> arcxx::expected<executer<std::pair<typename Result::key_type, typename Result::mapped_type>>, arcxx::string>;
        template<typename Result, specialized_from<std::tuple> BindAttrs>
        [[nodiscard]] auto make_executer(const query_relation<Result, BindAttrs>& query) -> arcxx::expected<executer<Result>, arcxx::string>;

        template<specialized_from<std::tuple> BindAttrs>
        arcxx::expected<void, arcxx::string> exec(const query_relation<void, BindAttrs>& query);
        template<typename Result, specialized_from<std::tuple> BindAttrs>
        [[nodiscard]] arcxx::expected<Result, arcxx::string> exec(const query_relation<Result, BindAttrs>& query);

        template<is_model Mod>
        arcxx::expected<void, arcxx::string> create_table(decltype(abort_if_exists));
        template<is_model Mod>
        arcxx::expected<void, arcxx::string> create_table();
        template<is_model Mod>
        arcxx::expected<void, arcxx::string> drop_table();

        template<is_model Mod>
        bool exists_table();

        arcxx::expected<void, arcxx::string> begin(const arcxx::string_view transaction_name = "");
        arcxx::expected<void, arcxx::string> commit(const arcxx::string_view transaction_name = "");
        arcxx::expected<void, arcxx::string> rollback(const arcxx::string_view transaction_name = "");

        template<typename F>
        requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t()>>
        arcxx::expected<void, arcxx::string> transaction(F&& func);
        template<typename F>
        requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t(sqlite3_connector&)>>
        arcxx::expected<void, arcxx::string> transaction(F&& func);

        template<is_attribute Attr>
        static arcxx::string column_definition();
    };

    namespace sqlite3 {
        using connector = sqlite3_connector;
    }
}

#include "sqlite3/executer.ipp"
#include "sqlite3/connector.ipp"