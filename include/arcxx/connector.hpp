#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "utils.hpp"

namespace arcxx{
    struct connector {};

    template<typename T>
    concept is_connector = std::derived_from<T, connector>;

    namespace transaction {
        namespace detail{
            struct commit_or_rollback_t {
                const std::optional<arcxx::string> rollback_reason;
                bool should_rollback() const noexcept {
                    return rollback_reason.has_value();
                }
            };
        }
        inline auto rollback(const arcxx::string_view reason){
            return detail::commit_or_rollback_t{ std::make_optional<arcxx::string>(reason) };
        }
        inline const detail::commit_or_rollback_t commit{ std::nullopt };
    }

    // abort if a given table exists when called connector::create_table.
    constexpr struct {} abort_if_exists;

    struct common_connector;
}