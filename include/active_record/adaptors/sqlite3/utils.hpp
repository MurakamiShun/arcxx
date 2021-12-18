#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
#include <sqlite3.h>
#include "active_record/query_impl/query_relation.hpp"
#include "active_record/model_impl/queries.hpp"

namespace active_record::sqlite3::detail {
    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::string>
    inline bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
        const auto type = sqlite3_column_type(stmt, static_cast<int>(idx));
        if(type == SQLITE_TEXT){
            auto text_ptr = sqlite3_column_text(stmt, static_cast<int>(idx));
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
    inline bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
        const auto type = sqlite3_column_type(stmt, static_cast<int>(idx));
        if(type == SQLITE_INTEGER){
            if constexpr(sizeof(typename Attr::value_type) == sizeof(int64_t)){
                attr = sqlite3_column_int64(stmt, static_cast<int>(idx));
            }
            else{
                attr = sqlite3_column_int(stmt, static_cast<int>(idx));
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
    inline bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
        const auto type = sqlite3_column_type(stmt, static_cast<int>(idx));
        if(type == SQLITE_FLOAT){
            attr = sqlite3_column_double(stmt, static_cast<int>(idx));
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
    inline bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
        const auto type = sqlite3_column_type(stmt, static_cast<int>(idx));
        if(type == SQLITE_BLOB){
            auto ptr = sqlite3_column_blob(stmt, static_cast<int>(idx));
            attr = std::vector<std::byte>{};
            const auto size = sqlite3_column_bytes(stmt, static_cast<int>(idx));
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
    inline bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, T& attr){
        const auto type = sqlite3_column_type(stmt, static_cast<int>(idx));
        if constexpr(std::is_floating_point_v<T>){
            if(type == SQLITE_FLOAT) {
                attr = sqlite3_column_double(stmt, static_cast<int>(idx));
                return true;
            }
        }
        else if constexpr(std::is_integral_v<T>){
            if(type == SQLITE_INTEGER) {
                if constexpr(sizeof(T) == sizeof(int64_t)){
                    attr = sqlite3_column_int64(stmt, static_cast<int>(idx));
                }
                else{
                    attr = sqlite3_column_int(stmt, static_cast<int>(idx));
                }
                return true;
            }
        }
        return false;
    }

    template<is_model T>
    [[nodiscard]] inline T extract_column_data(sqlite3_stmt* stmt){
        T ret;
        tuptup::indexed_apply_each(
            [stmt]<std::size_t N, typename Attr>(Attr& attr){ set_column_data(stmt, N, attr); },
            ret.attributes_as_tuple()
        );
        return ret;
    }
    template<specialized_from<std::tuple> T>
    [[nodiscard]] inline T extract_column_data(sqlite3_stmt* stmt){
        T ret;
        tuptup::indexed_apply_each(
            [stmt]<std::size_t N, typename Attr>(Attr& attr){ set_column_data(stmt, N, attr); },
            ret
        );
        return ret;
    }
    template<typename T>
    [[nodiscard]] inline T extract_column_data(sqlite3_stmt* stmt){
        T ret;
        set_column_data(stmt, 0, ret);
        return ret;
    }

    /*
     * variable binders
     */

    template<Attribute Attr>
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

    template<Attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    inline int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
        if(!attr) {
            return sqlite3_bind_null(stmt, static_cast<int>(index + 1));
        }
        else {
            return sqlite3_bind_double(stmt, static_cast<int>(index + 1), static_cast<double>(attr.value()));
        }
    }

    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::string>
    inline int bind_variable(sqlite3_stmt* stmt, const std::size_t index, const Attr& attr) {
        if(!attr) {
            return sqlite3_bind_null(stmt, static_cast<int>(index + 1));
        }
        else {
            // not copy text
            return sqlite3_bind_text(stmt, static_cast<int>(index + 1), attr.value().c_str(), static_cast<int>(attr.value().length()), SQLITE_STATIC);
        }
    }
    template<Attribute Attr>
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