#pragma once
/*
 * Copyright 2021 akisute514
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <sqlite3.h>
#include "../../query.hpp"
#include "../../attribute.hpp"

namespace active_record::sqlite3::detail {
    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::string>
    bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
        const auto type = sqlite3_column_type(stmt, idx);
        if(type == SQLITE_TEXT){
            auto text_ptr = sqlite3_column_text(stmt, idx);
            attr = active_record::string{ reinterpret_cast<const char*>(text_ptr) };
            return true;
        }
        else if(type == SQLITE_NULL){
            attr = std::nullopt;
            return true;
        }
        else{
            return false;
        }
    }
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type>
    bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
        const auto type = sqlite3_column_type(stmt, idx);
        if(type == SQLITE_INTEGER){
            if constexpr(sizeof(typename Attr::value_type) == sizeof(int64_t)){
                attr = sqlite3_column_int64(stmt, idx);
            }
            else{
                attr = sqlite3_column_int(stmt, idx);
            }
            return true;
        }
        else if(type == SQLITE_NULL){
            attr = std::nullopt;
            return true;
        }
        else{
            return false;
        }
    }
    template<Attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
        const auto type = sqlite3_column_type(stmt, idx);
        if(type == SQLITE_FLOAT){
            attr = sqlite3_column_double(stmt, idx);
            return true;
        }
        else if(type == SQLITE_NULL){
            attr = std::nullopt;
            return true;
        }
        else{
            return false;
        }
    }
    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
    bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
        const auto type = sqlite3_column_type(stmt, idx);
        if(type == SQLITE_BLOB){
            void* ptr = sqlite3_column_blob(stmt, idx);
            attr = std::vector<std::byte>{};
            const auto size = sqlite3_column_bytes(stmt, idx);
            attr.value().resize(size);
            std::copy_n(attr.value().begin(), ptr, size);
            return true;
        }
        else if(type == SQLITE_NULL){
            attr = std::nullopt;
            return true;
        }
        else{
            return false;
        }
    }
    template<typename T>
    requires (!Attribute<T>)
    bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, T& attr){
        const auto type = sqlite3_column_type(stmt, idx);
        if constexpr(std::is_floating_point_v<T>){
            if(type == SQLITE_FLOAT) {
                attr = sqlite3_column_double(stmt, idx);
                return true;
            }
        }
        else if constexpr(std::is_integral_v<T>){
            if(type == SQLITE_INTEGER) {
                if constexpr(sizeof(T) == sizeof(int64_t)){
                    attr = sqlite3_column_int64(stmt, idx);
                }
                else{
                    attr = sqlite3_column_int(stmt, idx);
                }
                return true;
            }
        }
        return false;
    }

    template<Model T>
    [[nodiscard]] T extract_column_data(sqlite3_stmt* stmt){
        T ret;
        tuptup::indexed_apply_each(
            [stmt]<std::size_t N, typename Attr>(Attr& attr){ set_column_data(stmt, N, attr); },
            ret.attributes_as_tuple()
        );
        return ret;
    }
    template<specialized_from<std::tuple> T>
    [[nodiscard]] T extract_column_data(sqlite3_stmt* stmt){
        T ret;
        tuptup::indexed_apply_each(
            [stmt]<std::size_t N, typename Attr>(Attr& attr){ set_column_data(stmt, N, attr); },
            ret
        );
        return ret;
    }
    template<typename T>
    [[nodiscard]] T extract_column_data(sqlite3_stmt* stmt){
        T ret;
        set_column_data(stmt, 0, ret);
        return ret;
    }

    /*
     * variable binders
     */

    template<Attribute Attr>
    requires std::integral<typename Attr::value_type>
    int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
        if(!attr) {
            return sqlite3_bind_null(stmt, index + 1);
        }
        else {
            if constexpr (sizeof(typename Attr::value_type) == sizeof(int64_t)) {
                return sqlite3_bind_int64(stmt, index + 1, attr.value());
            }
            else {
                return sqlite3_bind_int(stmt, index + 1, attr.value());
            }
        }
    }

    template<Attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
        if(!attr) {
            return sqlite3_bind_null(stmt, index + 1);
        }
        else {
            return sqlite3_bind_double(stmt, index + 1, static_cast<double>(attr.value()));
        }
    }

    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::string>
    int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
        if(!attr) {
            return sqlite3_bind_null(stmt, index + 1);
        }
        else {
            // not copy text
            return sqlite3_bind_text(stmt, index + 1, attr.value().c_str(), attr.value().length(), SQLITE_STATIC);
        }
    }
    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
    int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
        if(!attr) {
            return sqlite3_bind_null(stmt, index + 1);
        }
        else {
            // not copy text
            return sqlite3_bind_blob(stmt, index + 1, attr.value().data(), attr.value().size(), SQLITE_STATIC);
        }
    }
}