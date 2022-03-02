#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
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
        ::sqlite3* db_obj = nullptr;
        std::optional<active_record::string> error_msg = std::nullopt;

        sqlite3_adaptor(const active_record::string& file_name, const int flags);

        std::optional<active_record::string> get_error_msg(const char* msg_ptr) const;
        std::optional<active_record::string> get_error_msg(const int result_code) const;
        std::optional<active_record::string> get_error_msg() const;
    public:
        bool has_error() const noexcept;
        const active_record::string& error_message() const;

        static sqlite3_adaptor open(const active_record::string& file_name, const int flags = active_record::sqlite3::options::readwrite);
        void close();

        sqlite3_adaptor() = delete;
        sqlite3_adaptor(const sqlite3_adaptor&) = delete;
        sqlite3_adaptor(sqlite3_adaptor&& adpt);
        ~sqlite3_adaptor();

        static active_record::string_view version();
        static int version_number();

        static constexpr bool bindable = true;
        static active_record::string bind_variable_str(const std::size_t idx);

        template<typename Result, specialized_from<std::tuple> BindAttrs>
        [[nodiscard]] active_record::expected<Result, active_record::string> exec(const query_relation<Result, BindAttrs>& query);
        
        template<specialized_from<std::tuple> BindAttrs>
        active_record::expected<void, active_record::string> exec(const query_relation<void, BindAttrs>& query);

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
        requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t(sqlite3_adaptor&)>>
        active_record::expected<void, active_record::string> transaction(F&& func);

        template<is_attribute Attr>
        static active_record::string column_definition();
    };

    namespace sqlite3 {
        using adaptor = sqlite3_adaptor;
    }
}

#include "sqlite3/adaptor.ipp"