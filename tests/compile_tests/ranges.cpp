#include "../../include/arcxx/ranges.hpp"
#include "../../include/arcxx.hpp"

struct Test : public arcxx::model<Test>{
    static inline decltype(auto) table_name = "test_table";

    struct Int : public arcxx::attributes::integer<Test, Int>{
        using integer<Test, Int>::integer;
        static inline decltype(auto) column_name = "int_col";
        static constexpr auto constraints = primary_key & not_null & default_value(0);
    } int_col;

    struct String : public arcxx::attributes::string<Test, String>{
        using string<Test, String>::string;
        static inline decltype(auto) column_name = "string_col";
        static constexpr auto constraints = not_null & default_value("unknown");
    } str_col;

    struct Decimal : public arcxx::attributes::decimal<Test, Decimal>{
        using decimal<Test, Decimal>::decimal;
        static inline decltype(auto) column_name = "decimal_col";
        static constexpr auto constraints = not_null & default_value(0.0);
    } decimal_col;

    struct DateTime : public arcxx::attributes::datetime<Test, DateTime>{
        using datetime<Test, DateTime>::datetime;
        static inline decltype(auto) column_name = "datetime_col";
        static constexpr auto constraints = not_null;
    } datetime;
    struct Date : public arcxx::attributes::date<Test, Date>{
        using date<Test, Date>::date;
        static inline decltype(auto) column_name = "date_col";
        static constexpr auto constraints = not_null;
    } date;
};

int main(){
    using namespace arcxx;
    arcxx::sqlite3::connector conn{"test.sqlite3"};
    conn | Test::all() | arcxx::ranges::views::select<Test::Int, Test::Decimal>;
}