#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
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
    }

    class sqlite3_connector : public connector {
    private:
        ::sqlite3* db_obj = nullptr;
        std::optional<active_record::string> error_msg = std::nullopt;

        std::optional<active_record::string> get_error_msg(const char* msg_ptr) const;
        std::optional<active_record::string> get_error_msg(const int result_code) const;
        std::optional<active_record::string> get_error_msg() const;

        template<typename Result, specialized_from<std::tuple> BindAttrs>
        active_record::expected<::sqlite3_stmt*, active_record::string> make_stmt_and_bind(const query_relation<Result, BindAttrs>& query);

        template<typename ResultType>
        class executer;
    public:
        sqlite3_connector() = delete;
        sqlite3_connector(const sqlite3_connector&) = delete;
        sqlite3_connector(sqlite3_connector&&);
        sqlite3_connector(const active_record::string& file_name, const int flags = active_record::sqlite3::options::readwrite);
        ~sqlite3_connector();

        bool has_error() const noexcept;
        const active_record::string& error_message() const;

        static sqlite3_connector open(const active_record::string& file_name, const int flags = active_record::sqlite3::options::readwrite);
        void close();

        static active_record::string_view version();
        static int version_number();

        static constexpr bool bindable = true;
        static active_record::string bind_variable_str(const std::size_t idx, active_record::string&& buff = {});

        template<specialized_from<std::vector> Result, specialized_from<std::tuple> BindAttrs>
        [[nodiscard]] auto make_executer(const query_relation<Result, BindAttrs>& query) -> active_record::expected<executer<typename Result::value_type>, active_record::string>;
        template<specialized_from<std::unordered_map> Result, specialized_from<std::tuple> BindAttrs>
        [[nodiscard]] auto make_executer(const query_relation<Result, BindAttrs>& query) -> active_record::expected<executer<std::pair<typename Result::key_type, typename Result::mapped_type>>, active_record::string>;
        template<typename Result, specialized_from<std::tuple> BindAttrs>
        [[nodiscard]] auto make_executer(const query_relation<Result, BindAttrs>& query) -> active_record::expected<executer<Result>, active_record::string>;

        template<specialized_from<std::tuple> BindAttrs>
        active_record::expected<void, active_record::string> exec(const query_relation<void, BindAttrs>& query);
        template<typename Result, specialized_from<std::tuple> BindAttrs>
        [[nodiscard]] active_record::expected<Result, active_record::string> exec(const query_relation<Result, BindAttrs>& query);

        template<is_model Mod>
        active_record::expected<void, active_record::string> create_table(decltype(abort_if_exists));
        template<is_model Mod>
        active_record::expected<void, active_record::string> create_table();
        template<is_model Mod>
        active_record::expected<void, active_record::string> drop_table();

        template<is_model Mod>
        bool exists_table();

        active_record::expected<void, active_record::string> begin(const active_record::string_view transaction_name = "");
        active_record::expected<void, active_record::string> commit(const active_record::string_view transaction_name = "");
        active_record::expected<void, active_record::string> rollback(const active_record::string_view transaction_name = "");

        template<typename F>
        requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t()>>
        active_record::expected<void, active_record::string> transaction(F&& func);
        template<typename F>
        requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t(sqlite3_connector&)>>
        active_record::expected<void, active_record::string> transaction(F&& func);

        template<is_attribute Attr>
        static active_record::string column_definition();
    };

    namespace sqlite3 {
        using connector = sqlite3_connector;
    }
}

#include "sqlite3/executer.ipp"
#include "sqlite3/connector.ipp"