//#include <active_record.hpp>
#include "../../include/active_record.hpp"

struct Test : public active_record::model<Test>{
    static inline decltype(auto) table_name = "test_table";

    struct Int : public active_record::attributes::integer<Test, Int>{
        using integer<Test, Int>::integer;
        static inline decltype(auto) column_name = "int_col";
        static constexpr auto constraints = primary_key & not_null & default_value(0);
    } int_col;

    struct String : public active_record::attributes::string<Test, String>{
        using string<Test, String>::string;
        static inline decltype(auto) column_name = "string_col";
        static constexpr auto constraints = not_null & default_value("unknown");
    } str_col;

    struct Decimal : public active_record::attributes::decimal<Test, Decimal>{
        using decimal<Test, Decimal>::decimal;
        static inline decltype(auto) column_name = "decimal_col";
        static constexpr auto constraints = not_null & default_value(0.0);
    } decimal_col;

    struct DateTime : public active_record::attributes::datetime<Test, DateTime>{
        using datetime<Test, DateTime>::datetime;
        static inline decltype(auto) column_name = "datetime_col";
        static constexpr auto constraints = not_null;
    } datetime;
    struct Date : public active_record::attributes::date<Test, Date>{
        using date<Test, Date>::date;
        static inline decltype(auto) column_name = "date_col";
        static constexpr auto constraints = not_null;
    } date;
};

struct Have_a_Test : public active_record::model<Have_a_Test>{
    static inline decltype(auto) table_name = "belongs_to_test";
    struct TestID : public active_record::attributes::foreign_key<Have_a_Test, TestID, Test::Int>{
        using foreign_key<Have_a_Test, TestID, Test::Int>::foreign_key;
        static inline decltype(auto) column_name = "test_id";
        static constexpr auto constraints = not_null & default_value(0);
    } test_id;
};

int main(){
    using namespace std::chrono;
    using namespace std::chrono_literals;
    std::size_t discard_val = 0;
    auto query_test = [&discard_val](auto&& query_relation){
        discard_val += query_relation.to_sql().length();
    };
    Test test;
    query_test(Test::insert(test));
    query_test(Test::insert(std::move(test)));

    query_test(Test::all());
    query_test(Test::select<Test::Int, Test::String, Test::Decimal, Test::DateTime, Test::Date>());
    
    query_test(Test::select<Test::Int::count, Test::Int::sum, Test::Int::avg, Test::Int::max, Test::Int::min>());
    query_test(Test::select<Test::String::count>());
    query_test(Test::select<Test::Decimal::count, Test::Decimal::sum, Test::Decimal::avg, Test::Int::max, Test::Decimal::min>());
    query_test(Test::select<Test::DateTime::count, Test::DateTime::min, Test::DateTime::max>());
    query_test(Test::select<Test::Date::count, Test::Date::min, Test::Date::max>());
    
    query_test(Test::pluck<Test::Int>());
    query_test(Test::pluck<Test::String>());
    query_test(Test::pluck<Test::Decimal>());
    query_test(Test::pluck<Test::DateTime>());
    query_test(Test::pluck<Test::Date>());
    query_test(Test::pluck<Test::Int::count>());
    query_test(Test::pluck<Test::String::count>());
    query_test(Test::pluck<Test::Decimal::count>());

    query_test(Test::destroy(Test::Int{0}));
    query_test(Test::destroy(Test::String{"unknown"}));
    query_test(Test::destroy(Test::Decimal{0.0}));
    query_test(Test::destroy(Test::DateTime{sys_days(2022y/November/1d) + 1min}));
    query_test(Test::destroy(Test::Date{2022y/March/1d}));
    
    query_test(Test::destroy(Test::Int::cmp == 0));
    query_test(Test::destroy(Test::String::cmp == "unknown"));
    query_test(Test::destroy(Test::Decimal::cmp == 0.0));
    query_test(Test::destroy(Test::DateTime::cmp == sys_days(2022y/November/1d) + 1min));
    query_test(Test::destroy(Test::Date::cmp == 2020y/December/last));

    query_test(Test::where(Test::Int{0}));
    query_test(Test::where(Test::String{"unknown"}));
    query_test(Test::where(Test::Decimal{0.0}));
    query_test(Test::where(Test::DateTime::cmp == time_point_cast<seconds>(system_clock::now()) - 12h));
    query_test(Test::where(Test::Date::cmp == time_point_cast<days>(system_clock::now())));

    query_test(Test::where(Test::Int::cmp == 0));
    query_test(Test::where(Test::Int::cmp != 0));
    query_test(Test::where(Test::Int::cmp < 0));
    query_test(Test::where(Test::Int::cmp > 0));
    query_test(Test::where(Test::Int::cmp <= 0));
    query_test(Test::where(Test::Int::cmp >= 0));

    query_test(Test::where(Test::String::cmp == "unknown"));
    query_test(Test::where(Test::String::cmp != "unknown"));

    query_test(Test::where(Test::Decimal::cmp == 0.0));
    query_test(Test::where(Test::Decimal::cmp != 0.0));
    query_test(Test::where(Test::Decimal::cmp < 0.0));
    query_test(Test::where(Test::Decimal::cmp > 0.0));
    query_test(Test::where(Test::Decimal::cmp <= 0.0));
    query_test(Test::where(Test::Decimal::cmp >= 0.0));

    const auto current_datetime = time_point_cast<seconds>(system_clock::now());
    query_test(Test::where(Test::DateTime::cmp == current_datetime));
    query_test(Test::where(Test::DateTime::cmp != current_datetime));
    query_test(Test::where(Test::DateTime::cmp < current_datetime));
    query_test(Test::where(Test::DateTime::cmp > current_datetime));
    query_test(Test::where(Test::DateTime::cmp <= current_datetime));
    query_test(Test::where(Test::DateTime::cmp >= current_datetime));

    query_test(Test::limit(100));

    query_test(Test::order_by<Test::Int>()); // asc
    query_test(Test::order_by<Test::Int>(active_record::order::desc));
    query_test(Test::order_by<Test::String>()); // asc
    query_test(Test::order_by<Test::Decimal>()); // asc
    query_test(Test::order_by<Test::DateTime>()); // asc
    query_test(Test::order_by<Test::Date>()); // asc

    query_test(Have_a_Test::join<Test>());
    query_test(Have_a_Test::left_join<Test>());
    
    query_test(Test::group_by<Test::Int>());
    query_test(Test::group_by<Test::String>());
    query_test(Test::group_by<Test::Decimal>());
    query_test(Test::group_by<Test::DateTime>());
    query_test(Test::group_by<Test::Date>());
    
    query_test(Test::count());
    query_test(Test::sum<Test::Int>());
    query_test(Test::avg<Test::Int>());
    query_test(Test::max<Test::Int>());
    query_test(Test::min<Test::Int>());

    query_test(Test::sum<Test::Decimal>());
    query_test(Test::avg<Test::Decimal>());
    query_test(Test::max<Test::Decimal>());
    query_test(Test::min<Test::Decimal>());
    
    query_test(Test::max<Test::DateTime>());
    query_test(Test::min<Test::DateTime>());

    query_test(Test::max<Test::Date>());
    query_test(Test::min<Test::Date>());
}