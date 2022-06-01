#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "utils.hpp"

namespace active_record{
    struct connector {};

    template<typename T>
    concept is_connector = std::derived_from<T, connector>;

    namespace transaction {
        namespace detail{
            struct commit_or_rollback_t {
                const std::optional<active_record::string> rollback_reason;
                bool should_rollback() const noexcept {
                    return rollback_reason.has_value();
                }
            };
        }
        inline auto rollback(const active_record::string_view reason){
            return detail::commit_or_rollback_t{ std::make_optional<active_record::string>(reason) };
        }
        inline const detail::commit_or_rollback_t commit{ std::nullopt };
    }

    // abort if a given table exists when called connector::create_table.
    constexpr struct {} abort_if_exists;

    struct common_connector;
}