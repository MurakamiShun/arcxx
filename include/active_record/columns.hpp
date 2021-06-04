#pragma once
#include "database.hpp"
#include "utils.hpp"

namespace active_record{
    class database::table::column {
    private:
        template<typename SubClass, typename Table, typename Column, typename Type>
        struct attribute {
        protected:
            std::optional<Type> data;
            using validator = std::function<bool(const Type&)>;
            template<typename T>
            struct has_column_name {
            private:
                template<typename S>
                static decltype(S::column_name, std::true_type()) check(S);
                static std::false_type check(...);
            public:
                static constexpr bool value = decltype(check(std::declval<T>()))::value;
            };
            
            template<typename T>
            struct has_validators {
            private:
                template<typename S>
                static decltype(S::validators, std::true_type()) check(S);
                static std::false_type check(...);
            public:
                static constexpr bool value = decltype(check(std::declval<T>()))::value;
            };

            static_assert(has_column_name<Column>::value, "table doesn't have column_name. Declare like \"constexpr static auto column_name = \"id\"\"");
        public:
            const static std::pair<std::string_view, std::string_view> column_full_name = { Table::table_name, Column::column_name };
            attribute() {}
            attribute(std::optional<Type>& default_value) : data(default_value) {}
            virtual void from_string(const database::string_view str) = 0;
            virtual database::string to_string() const = 0;
            bool is_valid() const{
                if constexpr(has_validations<Column>::value){
                    for(const auto& val : Column::validators){
                        if(!val(data)) return false;
                    }
                }
                return true;
            }
            Column& operator=(const std::optional<Type> dst){
                data = dst;
                return *dynamic_cast<Column*>(this);
            }
            Column& operator=(const std::optional<const Type> dst){
                data = dst;
                return *dynamic_cast<Column*>(this);
            }
            explicit operator bool() const noexcept{ return static_cast<bool>(data); }
            const Type& value() const& { return data.value(); }
            Type& value() & { return data.value(); }
            const Type&& value() const&& { return std::move(data.value()); }
            Type&& value() && { return std::move(data.value()); }
            Type* operator->() { return data::operator->(); }
            const Type* operator->() const { return data::operator->(); }
        }
    public:
        static auto unspecified = std::nullopt;
        template<typename Table, typename Column>
        struct string : attribute<string, Table, Column, database::string>{
            string& operator=(const std::optional<database::string_view> dst){
                data = dst;
                return *this;
            }
            string& operator=(const std::optional<const database::string_view> dst){
                data = dst;
                return *this;
            }
        };

        template<typename Table, typename Column>
        struct integer : attribute<integer, Table, Column, int32_t>{};

        template<typename Table, typename Column>
        struct decimal : attribute<decimal, Table, Column, float>{};
        
        template<typename Table, typename Column>
        struct boolean : attribute<boolean, Table, Column, bool>{};

        template<typename Table, typename Column>
        struct datetime : attribute<datetime, Table, Column, database::datetime>{};

        template<typename Table, typename Column>
        struct binary : attribute<binary, Table, Column, std::vector<byte>> {};
    };
}