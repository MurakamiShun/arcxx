#pragma once
#include "../../attribute.hpp"
#include "string_convertors.hpp"
#if !(defined(_WIN32) || defined(_WIN64))
#include <byteswap.h>
#endif

namespace active_record::PostgreSQL::detail {
    template<typename T>
    [[nodiscard]] auto hton(const T h) noexcept -> decltype(h) {
        #if defined(_WIN32) || defined(_WIN64)
        if constexpr (sizeof(h) == sizeof(uint16_t)) return _byteswap_ushort(h);
        else if constexpr (sizeof(h) == sizeof(uint32_t)) return _byteswap_ulong(h);
        else if constexpr (sizeof(h) == sizeof(uint64_t)) return _byteswap_uint64(h);
        #else
        if constexpr (__BYTE_ORDER == __LITTLE_ENDIAN) {
            if constexpr (sizeof(h) == sizeof(uint16_t)) return bswap_16(h);
            else if constexpr (sizeof(h) == sizeof(uint32_t)) return bswap_32(h);
            else if constexpr (sizeof(h) == sizeof(uint64_t)) return bswap_64(h);
        }
        #endif
        return h;
    }
    template<typename T>
    [[nodiscard]] auto ntoh(const T n) noexcept -> decltype(n) {
        #if defined(_WIN32) || defined(_WIN64)
        if constexpr (sizeof(n) == sizeof(uint16_t)) return _byteswap_ushort(n);
        else if constexpr (sizeof(n) == sizeof(uint32_t)) return _byteswap_ulong(n);
        else if constexpr (sizeof(n) == sizeof(uint64_t)) return _byteswap_uint64(n);
        #else
        if constexpr (__BYTE_ORDER == __LITTLE_ENDIAN) {
            if constexpr (sizeof(n) == sizeof(uint16_t)) return bswap_16(n);
            else if constexpr (sizeof(n) == sizeof(uint32_t)) return bswap_32(n);
            else if constexpr (sizeof(n) == sizeof(uint64_t)) return bswap_64(n);
        }
        #endif
        return n;
    }

    template<Attribute Attr>
    requires std::same_as<typename Attr::value_type, active_record::string>
    [[nodiscard]] auto get_value_ptr(const Attr* const attr_ptr, [[maybe_unused]]std::any&) {
        if (!(*attr_ptr)) return static_cast<const char*>(nullptr);
        return attr_ptr->value().c_str(); 
    }
    template<Attribute Attr>
    requires std::integral<typename Attr::value_type>
    [[nodiscard]] auto get_value_ptr(const Attr* const attr_ptr, [[maybe_unused]]std::any& tmp) {
        if (!(*attr_ptr)) return static_cast<const char*>(nullptr);
        const auto tmp_value = hton(attr_ptr->value());
        tmp = tmp_value;
        return reinterpret_cast<const char*>(std::any_cast<decltype(tmp_value)>(&tmp));
    }
    template<Attribute Attr>
    requires std::floating_point<typename Attr::value_type>
    [[nodiscard]] auto get_value_ptr(const Attr* const attr_ptr, [[maybe_unused]]std::any& tmp) {
        if (!(*attr_ptr)) return static_cast<const char*>(nullptr);
        const auto tmp_value = hton(attr_ptr->value());
        tmp = tmp_value;
        return reinterpret_cast<const char*>(std::any_cast<decltype(tmp_value)>(&tmp));
    }

    template<Attribute Attr>
    bool set_column_data(PGresult* res, int col, int field, Attr& attr) {
        if(PQgetisnull(res, col, field)){
            attr = std::nullopt;
            return true;
        }
        const char* const value_text = PQgetvalue(res, col, field);
        if (value_text == nullptr) return false;
        from_string<postgresql_adaptor>(attr, active_record::string_view{ value_text });
        return true;
    }
    template<typename T>
    requires (!Attribute<T>)
    bool set_column_data(PGresult* res, int col, int field, T& result) {
        const char* const text_ptr = PQgetvalue(res, col, field);
        if (text_ptr == nullptr) return false;
        const auto value_text = active_record::string{ text_ptr };
        std::from_chars(&(value_text.front()), &(value_text.back()), result);
        return true;
    }

    template<Model T>
    [[nodiscard]] T extract_column_data(PGresult* res, int col) {
        T ret;
        indexed_apply(
            [res, col]<typename... Attrs>(Attrs... attrs){
                return (set_column_data(res, col, attrs.first, attrs.second) && ...);
            },
            ret.attributes
        );
        return ret;
    }
    template<Tuple T>
    [[nodiscard]] T extract_column_data(PGresult* res, int col) {
        T ret;
        indexed_apply(
            [res, col]<typename... Attrs>(Attrs... attrs){
                return (set_column_data(res, col, attrs.first, attrs.second) && ...);
            },
            ret
        );
        return ret;
    }
    template<typename T>
    [[nodiscard]] T extract_column_data(PGresult* res, int col) {
        T ret;
        set_column_data(res, col, 0, ret);
        return ret;
    }
}