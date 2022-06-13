#include <arcxx.hpp>

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
};

struct Have_a_Test : public arcxx::model<Have_a_Test>{
    static inline decltype(auto) table_name = "belongs_to_test";
    struct TestID : public arcxx::attributes::foreign_key<Have_a_Test, TestID, Test::Int>{
        using foreign_key<Have_a_Test, TestID, Test::Int>::foreign_key;
        static inline decltype(auto) column_name = "test_id";
        static constexpr auto constraints = not_null & default_value(0);
    } test_id;
};

int main(){
    std::size_t discard_val = 0;
    auto query_test = [&discard_val](auto&& query_relation){
        discard_val += query_relation.to_sql().length();
    };

    // left value
    const auto query = Test::count();

    query_test(query.where(Test::Int{0}));
    query_test(query.where(Test::String{"unknown"}));
    query_test(query.where(Test::Decimal{0.0}));

    query_test(query.where(Test::Int::cmp == 0));
    query_test(query.where(Test::Int::cmp != 0));
    query_test(query.where(Test::Int::cmp < 0));
    query_test(query.where(Test::Int::cmp > 0));
    query_test(query.where(Test::Int::cmp <= 0));
    query_test(query.where(Test::Int::cmp >= 0));

    query_test(query.where(Test::String::cmp == "unknown"));
    query_test(query.where(Test::String::cmp != "unknown"));

    query_test(query.where(Test::Decimal::cmp == 0.0));
    query_test(query.where(Test::Decimal::cmp != 0.0));
    query_test(query.where(Test::Decimal::cmp < 0.0));
    query_test(query.where(Test::Decimal::cmp > 0.0));
    query_test(query.where(Test::Decimal::cmp <= 0.0));
    query_test(query.where(Test::Decimal::cmp >= 0.0));

    // right value
    query_test(Test::count().where(Test::Int{0}));
    query_test(Test::count().where(Test::String{"unknown"}));
    query_test(Test::count().where(Test::Decimal{0.0}));

    query_test(Test::count().where(Test::Int::cmp == 0));
    query_test(Test::count().where(Test::Int::cmp != 0));
    query_test(Test::count().where(Test::Int::cmp < 0));
    query_test(Test::count().where(Test::Int::cmp > 0));
    query_test(Test::count().where(Test::Int::cmp <= 0));
    query_test(Test::count().where(Test::Int::cmp >= 0));

    query_test(Test::count().where(Test::String::cmp == "unknown"));
    query_test(Test::count().where(Test::String::cmp != "unknown"));

    query_test(Test::count().where(Test::Decimal::cmp == 0.0));
    query_test(Test::count().where(Test::Decimal::cmp != 0.0));
    query_test(Test::count().where(Test::Decimal::cmp < 0.0));
    query_test(Test::count().where(Test::Decimal::cmp > 0.0));
    query_test(Test::count().where(Test::Decimal::cmp <= 0.0));
    query_test(Test::count().where(Test::Decimal::cmp >= 0.0));
}