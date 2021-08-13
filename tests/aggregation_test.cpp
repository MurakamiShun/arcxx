#include "user_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "sum query tests", "[model][select][aggregation][insert]") {
    SECTION("Integer attribute"){
        INFO(User::sum<User::ID>().to_sql());

        const auto [error, total] = User::sum<User::ID>().exec(conn);
        if(error) FAIL(error.value());
        REQUIRE(total.value() == 45);
    }
    
    SECTION("Decimal attribute"){
        INFO(User::sum<User::Height>().to_sql());

        const auto [error, total] = User::sum<User::Height>().exec(conn);
        if(error) FAIL(error.value());
        REQUIRE(total.value() == 1745.0);
    }
}

TEST_CASE_METHOD(UserModelTestsFixture, "avg query tests", "[model][select][aggregation][insert]") {
    SECTION("Integer attribute"){
        INFO(User::avg<User::ID>().to_sql());

        const auto [error, avg] = User::avg<User::ID>().exec(conn);
        if(error) FAIL(error.value());
        REQUIRE(avg.value() == 4.5);
    }
    
    SECTION("Decimal attribute"){
        INFO(User::avg<User::Height>().to_sql());

        const auto [error, avg] = User::avg<User::Height>().exec(conn);
        if(error) FAIL(error.value());
        REQUIRE(avg.value() == 174.5);
    }
}

TEST_CASE_METHOD(UserModelTestsFixture, "max query tests", "[model][select][aggregation][insert]") {
    SECTION("Integer attribute"){
        INFO(User::max<User::ID>().to_sql());

        const auto [error, max] = User::max<User::ID>().exec(conn);
        if(error) FAIL(error.value());
        REQUIRE(max.value() == 9);
    }
    
    SECTION("Decimal attribute"){
        INFO(User::max<User::Height>().to_sql());

        const auto [error, max] = User::max<User::Height>().exec(conn);
        if(error) FAIL(error.value());
        REQUIRE(max.value() == 179.0);
    }
}

TEST_CASE_METHOD(UserModelTestsFixture, "min query tests", "[model][select][aggregation][insert]") {
    SECTION("Integer attribute"){
        INFO(User::min<User::ID>().to_sql());

        const auto [error, min] = User::min<User::ID>().exec(conn);
        if(error) FAIL(error.value());
        REQUIRE(min.value() == 0);
    }
    
    SECTION("Decimal attribute"){
        INFO(User::min<User::Height>().to_sql());

        const auto [error, min] = User::min<User::Height>().exec(conn);
        if(error) FAIL(error.value());
        REQUIRE(min.value() == 170.0);
    }
}

TEST_CASE_METHOD(UserModelTestsFixture, "all aggregation query tests", "[model][select][aggregation][insert]") {
    SECTION("Integer attribute"){
        using ID = User::ID;
        INFO(( User::select<ID::sum, ID::avg, ID::max, ID::min>().to_sql() ));

        const auto [error, ag] = User::select<ID::sum, ID::avg, ID::max, ID::min>().exec(conn);
        if(error) FAIL(error.value());
        REQUIRE(std::get<0>(ag).value() == 45);
        REQUIRE(std::get<1>(ag).value() == 4.5);
        REQUIRE(std::get<2>(ag).value() == 9);
        REQUIRE(std::get<3>(ag).value() == 0);
    }

    SECTION("Decimal attribute"){
        using Height = User::Height;
        INFO(( User::select<Height::sum, Height::avg, Height::max, Height::min>().to_sql() ));

        const auto [error, ag] = User::select<Height::sum, Height::avg, Height::max, Height::min>().exec(conn);
        if(error) FAIL(error.value());
        REQUIRE(std::get<0>(ag).value() == 1745.0);
        REQUIRE(std::get<1>(ag).value() == 174.5);
        REQUIRE(std::get<2>(ag).value() == 179.0);
        REQUIRE(std::get<3>(ag).value() == 170.0);
    }
}

TEST_CASE_METHOD(UserModelTestsFixture, "all aggregation with where clause query tests", "[model][query_relation][select][aggregation][insert]") {
    SECTION("Integer attribute"){
        using ID = User::ID;
        INFO(( User::where(ID::between(0,5)).select<ID::sum, ID::avg, ID::max, ID::min>().to_sql() ));

        const auto [error, ag] = User::where(ID::between(0,5)).select<ID::sum, ID::avg, ID::max, ID::min>().exec(conn);
        if(error) FAIL(error.value());

        const auto [error2, sum] = User::where(ID::between(0,5)).sum<ID>().exec(conn);
        if(error2) FAIL(error2.value());
        
        const auto [error3, avg] = User::where(ID::between(0,5)).avg<ID>().exec(conn);
        if(error3) FAIL(error3.value());
        
        const auto [error4, max] = User::where(ID::between(0,5)).max<ID>().exec(conn);
        if(error4) FAIL(error4.value());
        
        const auto [error5, min] = User::where(ID::between(0,5)).min<ID>().exec(conn);
        if(error5) FAIL(error5.value());

        REQUIRE(std::get<0>(ag).value() == 15);
        REQUIRE(sum.value()             == 15);
        REQUIRE(std::get<1>(ag).value() == 2.5);
        REQUIRE(avg.value()             == 2.5);
        REQUIRE(std::get<2>(ag).value() == 5);
        REQUIRE(max.value()             == 5);
        REQUIRE(std::get<3>(ag).value() == 0);
        REQUIRE(min.value()             == 0);
    }

    SECTION("Decimal attribute"){
        using Height = User::Height;
        INFO(( User::where(User::ID::between(0,5)).select<Height::sum, Height::avg, Height::max, Height::min>().to_sql() ));

        const auto [error, ag] = User::where(User::ID::between(0,5)).select<Height::sum, Height::avg, Height::max, Height::min>().exec(conn);
        if(error) FAIL(error.value());

        const auto [error2, sum] = User::where(User::ID::between(0,5)).sum<Height>().exec(conn);
        if(error2) FAIL(error2.value());
        
        const auto [error3, avg] = User::where(User::ID::between(0,5)).avg<Height>().exec(conn);
        if(error3) FAIL(error3.value());
        
        const auto [error4, max] = User::where(User::ID::between(0,5)).max<Height>().exec(conn);
        if(error4) FAIL(error4.value());
        
        const auto [error5, min] = User::where(User::ID::between(0,5)).min<Height>().exec(conn);
        if(error5) FAIL(error5.value());

        REQUIRE(std::get<0>(ag).value() == 1035.0);
        REQUIRE(sum.value()             == 1035.0);
        REQUIRE(std::get<1>(ag).value() == 172.5);
        REQUIRE(avg.value()             == 172.5);
        REQUIRE(std::get<2>(ag).value() == 175.0);
        REQUIRE(max.value()             == 175.0);
        REQUIRE(std::get<3>(ag).value() == 170.0);
        REQUIRE(min.value()             == 170.0);
    }
}