//#include <active_record.hpp>
#include "../../include/active_record.hpp"

struct Test : public active_record::model<Test>{
    static inline decltype(auto) table_name = "test_table";

    struct Int : public active_record::attributes::integer<Test, Int>{
        using integer<Test, Int>::integer;
        static inline decltype(auto) column_name = "int_col";
        static inline const auto constraints = { primary_key, not_null, default_value(0) };
    } int_col;

    struct String : public active_record::attributes::string<Test, String>{
        using string<Test, String>::string;
        static inline decltype(auto) column_name = "string_col";
        static inline const auto constraints = { not_null, default_value("unknown") };
    } str_col;

    struct Decimal : public active_record::attributes::decimal<Test, Decimal>{
        using decimal<Test, Decimal>::decimal;
        static inline decltype(auto) column_name = "decimal_col";
        static inline const auto constraints = { not_null, default_value(0.0) };
    } decimal_col;
};

struct Have_a_Test : public active_record::model<Have_a_Test>{
    static inline decltype(auto) table_name = "belongs_to_test";
    struct TestID : public active_record::attributes::foreign_key<Have_a_Test, TestID, Test::Int>{
        using foreign_key<Have_a_Test, TestID, Test::Int>::foreign_key;
        static inline decltype(auto) column_name = "test_id";
        static inline const auto constraints = { not_null, default_value(0.0) };
    } test_id;
};

int main(){
    std::size_t discard_val = 0;
    auto query_test = [&discard_val](auto&& query_relation){
        discard_val += query_relation.to_sql().length();
    };
    // left value
    const auto query = Test::where(Test::Int::cmp > 0);

    query_test(query.select<Test::Int, Test::String, Test::Decimal>());
    
    query_test(query.select<Test::Int::count, Test::Int::sum, Test::Int::avg, Test::Int::max, Test::Int::min>());
    query_test(query.select<Test::String::count>());
    query_test(query.select<Test::Decimal::count, Test::Decimal::sum, Test::Decimal::avg, Test::Int::max, Test::Decimal::min>());
    
    query_test(query.pluck<Test::Int>());
    query_test(query.pluck<Test::String>());
    query_test(query.pluck<Test::Decimal>());
    query_test(query.pluck<Test::Int::count>());
    query_test(query.pluck<Test::String::count>());
    query_test(query.pluck<Test::Decimal::count>());

    query_test(query.update(Test::Int{0}, Test::String{"unknown"}, Test::Decimal{0.0}));

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

    query_test(query.limit(100));

    query_test(query.order_by<Test::Int>()); // asc
    query_test(query.order_by<Test::Int>(active_record::order::desc));
    query_test(query.order_by<Test::String>()); // asc
    query_test(query.order_by<Test::Decimal>()); // asc
    
    query_test(query.count());
    query_test(query.sum<Test::Int>());
    query_test(query.avg<Test::Int>());
    query_test(query.max<Test::Int>());
    query_test(query.min<Test::Int>());

    query_test(query.sum<Test::Decimal>());
    query_test(query.avg<Test::Decimal>());
    query_test(query.max<Test::Decimal>());
    query_test(query.min<Test::Decimal>());

    // right value
    query_test(Test::where(Test::Int::cmp > 0).select<Test::Int, Test::String, Test::Decimal>());
    
    query_test(Test::where(Test::Int::cmp > 0).select<Test::Int::count, Test::Int::sum, Test::Int::avg, Test::Int::max, Test::Int::min>());
    query_test(Test::where(Test::Int::cmp > 0).select<Test::String::count>());
    query_test(Test::where(Test::Int::cmp > 0).select<Test::Decimal::count, Test::Decimal::sum, Test::Decimal::avg, Test::Int::max, Test::Decimal::min>());
    
    query_test(Test::where(Test::Int::cmp > 0).pluck<Test::Int>());
    query_test(Test::where(Test::Int::cmp > 0).pluck<Test::String>());
    query_test(Test::where(Test::Int::cmp > 0).pluck<Test::Decimal>());
    query_test(Test::where(Test::Int::cmp > 0).pluck<Test::Int::count>());
    query_test(Test::where(Test::Int::cmp > 0).pluck<Test::String::count>());
    query_test(Test::where(Test::Int::cmp > 0).pluck<Test::Decimal::count>());

    query_test(Test::where(Test::Int::cmp > 0).update(Test::Int{0}, Test::String{"unknown"}, Test::Decimal{0.0}));

    query_test(Test::where(Test::Int::cmp > 0).where(Test::Int{0}));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::String{"unknown"}));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::Decimal{0.0}));

    query_test(Test::where(Test::Int::cmp > 0).where(Test::Int::cmp == 0));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::Int::cmp != 0));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::Int::cmp < 0));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::Int::cmp > 0));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::Int::cmp <= 0));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::Int::cmp >= 0));

    query_test(Test::where(Test::Int::cmp > 0).where(Test::String::cmp == "unknown"));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::String::cmp != "unknown"));

    query_test(Test::where(Test::Int::cmp > 0).where(Test::Decimal::cmp == 0.0));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::Decimal::cmp != 0.0));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::Decimal::cmp < 0.0));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::Decimal::cmp > 0.0));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::Decimal::cmp <= 0.0));
    query_test(Test::where(Test::Int::cmp > 0).where(Test::Decimal::cmp >= 0.0));

    query_test(Test::where(Test::Int::cmp > 0).limit(100));

    query_test(Test::where(Test::Int::cmp > 0).order_by<Test::Int>()); // asc
    query_test(Test::where(Test::Int::cmp > 0).order_by<Test::Int>(active_record::order::desc));
    query_test(Test::where(Test::Int::cmp > 0).order_by<Test::String>()); // asc
    query_test(Test::where(Test::Int::cmp > 0).order_by<Test::Decimal>()); // asc
    
    query_test(Test::where(Test::Int::cmp > 0).count());
    query_test(Test::where(Test::Int::cmp > 0).sum<Test::Int>());
    query_test(Test::where(Test::Int::cmp > 0).avg<Test::Int>());
    query_test(Test::where(Test::Int::cmp > 0).max<Test::Int>());
    query_test(Test::where(Test::Int::cmp > 0).min<Test::Int>());

    query_test(Test::where(Test::Int::cmp > 0).sum<Test::Decimal>());
    query_test(Test::where(Test::Int::cmp > 0).avg<Test::Decimal>());
    query_test(Test::where(Test::Int::cmp > 0).max<Test::Decimal>());
    query_test(Test::where(Test::Int::cmp > 0).min<Test::Decimal>());
}