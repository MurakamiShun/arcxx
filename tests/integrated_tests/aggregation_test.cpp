#include "user_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "sum query tests", "[model][select][aggregation][insert]") {
    SECTION("Integer attribute"){
        INFO(User::sum<User::ID>().to_sql());

        const auto total_result = User::sum<User::ID>().exec(conn);
        if(!total_result) FAIL(total_result.error());
        REQUIRE(total_result.value() == 45);
    }
    
    SECTION("Decimal attribute"){
        INFO(User::sum<User::Height>().to_sql());

        const auto total_result = User::sum<User::Height>().exec(conn);
        if(!total_result) FAIL(total_result.error());
        REQUIRE(total_result.value() == 1745.0);
    }
}

TEST_CASE_METHOD(UserModelTestsFixture, "avg query tests", "[model][select][aggregation][insert]") {
    SECTION("Integer attribute"){
        INFO(User::avg<User::ID>().to_sql());

        const auto avg_result = User::avg<User::ID>().exec(conn);
        if(!avg_result) FAIL(avg_result.error());
        REQUIRE(avg_result.value() == 4.5);
    }
    
    SECTION("Decimal attribute"){
        INFO(User::avg<User::Height>().to_sql());

        const auto height_avg_result = User::avg<User::Height>().exec(conn);
        if(!height_avg_result) FAIL(height_avg_result.error());
        REQUIRE(height_avg_result.value() == 174.5);
    }
}

TEST_CASE_METHOD(UserModelTestsFixture, "max query tests", "[model][select][aggregation][insert]") {
    SECTION("Integer attribute"){
        INFO(User::max<User::ID>().to_sql());

        const auto max_result = User::max<User::ID>().exec(conn);
        if(!max_result) FAIL(max_result.error());
        REQUIRE(max_result.value() == 9);
    }
    
    SECTION("Decimal attribute"){
        INFO(User::max<User::Height>().to_sql());

        const auto max_hegiht_result = User::max<User::Height>().exec(conn);
        if(!max_hegiht_result) FAIL(max_hegiht_result.error());
        REQUIRE(max_hegiht_result.value() == 179.0);
    }
}

TEST_CASE_METHOD(UserModelTestsFixture, "min query tests", "[model][select][aggregation][insert]") {
    SECTION("Integer attribute"){
        INFO(User::min<User::ID>().to_sql());

        const auto min_result = User::min<User::ID>().exec(conn);
        if(!min_result) FAIL(min_result.error());
        REQUIRE(min_result.value() == 0);
    }
    
    SECTION("Decimal attribute"){
        INFO(User::min<User::Height>().to_sql());

        const auto min_height_result = User::min<User::Height>().exec(conn);
        if(!min_height_result) FAIL(min_height_result.error());
        REQUIRE(min_height_result.value() == 170.0);
    }
}

TEST_CASE_METHOD(UserModelTestsFixture, "all aggregation query tests", "[model][select][aggregation][insert]") {
    SECTION("Integer attribute"){
        using ID = User::ID;
        INFO(( User::select<ID::sum, ID::avg, ID::max, ID::min>().to_sql() ));

        const auto aggregation_results = User::select<ID::sum, ID::avg, ID::max, ID::min>().exec(conn);
        if(!aggregation_results) FAIL(aggregation_results.error());
        REQUIRE(std::get<0>(aggregation_results.value()) == 45);
        REQUIRE(std::get<1>(aggregation_results.value()) == 4.5);
        REQUIRE(std::get<2>(aggregation_results.value()) == 9);
        REQUIRE(std::get<3>(aggregation_results.value()) == 0);
    }

    SECTION("Decimal attribute"){
        using Height = User::Height;
        INFO(( User::select<Height::sum, Height::avg, Height::max, Height::min>().to_sql() ));

        const auto aggregation_results = User::select<Height::sum, Height::avg, Height::max, Height::min>().exec(conn);
        if(!aggregation_results) FAIL(aggregation_results.error());
        REQUIRE(std::get<0>(aggregation_results.value()) == 1745.0);
        REQUIRE(std::get<1>(aggregation_results.value()) == 174.5);
        REQUIRE(std::get<2>(aggregation_results.value()) == 179.0);
        REQUIRE(std::get<3>(aggregation_results.value()) == 170.0);
    }
}

TEST_CASE_METHOD(UserModelTestsFixture, "all aggregation with where clause query tests", "[model][query_relation][select][aggregation][insert]") {
    SECTION("Integer attribute"){
        using ID = User::ID;
        INFO(( User::where(ID::between(0,5)).select<ID::sum, ID::avg, ID::max, ID::min>().to_sql() ));

        const auto aggregation_results = User::where(ID::between(0,5)).select<ID::sum, ID::avg, ID::max, ID::min>().exec(conn);
        if(!aggregation_results) FAIL(aggregation_results.error());

        const auto sum_result = User::where(ID::between(0,5)).sum<ID>().exec(conn);
        if(!sum_result) FAIL(sum_result.error());
        
        const auto avg_result = User::where(ID::between(0,5)).avg<ID>().exec(conn);
        if(!avg_result) FAIL(avg_result.error());
        
        const auto max_result = User::where(ID::between(0,5)).max<ID>().exec(conn);
        if(!max_result) FAIL(max_result.error());
        
        const auto min_result = User::where(ID::between(0,5)).min<ID>().exec(conn);
        if(!min_result) FAIL(min_result.error());

        REQUIRE(std::get<0>(aggregation_results.value()) == 15);
        REQUIRE(std::get<1>(aggregation_results.value()) == 2.5);
        REQUIRE(std::get<2>(aggregation_results.value()) == 5);
        REQUIRE(std::get<3>(aggregation_results.value()) == 0);
        REQUIRE(sum_result.value() == 15);
        REQUIRE(avg_result.value() == 2.5);
        REQUIRE(max_result.value() == 5);
        REQUIRE(min_result.value() == 0);
    }

    SECTION("Decimal attribute"){
        using Height = User::Height;
        INFO(( User::where(User::ID::between(0,5)).select<Height::sum, Height::avg, Height::max, Height::min>().to_sql() ));

        const auto aggregation_results = User::where(User::ID::between(0,5)).select<Height::sum, Height::avg, Height::max, Height::min>().exec(conn);
        if(!aggregation_results) FAIL(aggregation_results.error());

        const auto sum_result = User::where(User::ID::between(0,5)).sum<Height>().exec(conn);
        if(!sum_result) FAIL(sum_result.error());
        
        const auto avg_result = User::where(User::ID::between(0,5)).avg<Height>().exec(conn);
        if(!avg_result) FAIL(avg_result.error());
        
        const auto max_result = User::where(User::ID::between(0,5)).max<Height>().exec(conn);
        if(!max_result) FAIL(max_result.error());
        
        const auto min_result = User::where(User::ID::between(0,5)).min<Height>().exec(conn);
        if(!min_result) FAIL(min_result.error());

        REQUIRE(std::get<0>(aggregation_results.value()) == 1035.0);
        REQUIRE(std::get<1>(aggregation_results.value()) == 172.5);
        REQUIRE(std::get<2>(aggregation_results.value()) == 175.0);
        REQUIRE(std::get<3>(aggregation_results.value()) == 170.0);
        REQUIRE(sum_result.value() == 1035.0);
        REQUIRE(avg_result.value() == 172.5);
        REQUIRE(max_result.value() == 175.0);
        REQUIRE(min_result.value() == 170.0);
    }
}