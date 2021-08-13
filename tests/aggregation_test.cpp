#include "user_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "sum query tests", "[model][query_relation][select][aggregation][insert]") {
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

TEST_CASE_METHOD(UserModelTestsFixture, "avg query tests", "[model][query_relation][select][aggregation][insert]") {
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

TEST_CASE_METHOD(UserModelTestsFixture, "max query tests", "[model][query_relation][select][aggregation][insert]") {
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

TEST_CASE_METHOD(UserModelTestsFixture, "min query tests", "[model][query_relation][select][aggregation][insert]") {
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

TEST_CASE_METHOD(UserModelTestsFixture, "all aggregation query tests", "[model][query_relation][select][aggregation][insert]") {
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