#include "user_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "Select query tests", "[model][query_relation][select]") {
    SECTION("select user name"){
        INFO(User::select<User::Name>().where(User::Name{"user1"}).to_sql());
        if (const auto [error, user_names] = User::select<User::Name>().where(User::Name{"user1"}).exec(conn); error) {
            FAIL(error.value());
        }
        else {
            STATIC_REQUIRE(std::is_same_v<decltype(user_names), std::vector<std::tuple<User::Name>>>);
            REQUIRE(user_names.size() == 1);
            REQUIRE(std::get<0>(user_names[0]).value() == active_record::string{ "user1" });
        }
    }

    SECTION("pluck user name") {
        INFO(User::pluck<User::Name>().where(User::ID{1}).to_sql());
        if (const auto [error, user_names] = User::pluck<User::Name>().where(User::ID{1}).exec(conn); error) {
            FAIL(error.value());
        }
        else {
            STATIC_REQUIRE(std::is_same_v<decltype(user_names), std::vector<User::Name>>);
            REQUIRE(user_names.size() == 1);
            REQUIRE(user_names[0].value() == active_record::string{ "user1" });
        }
    }

    SECTION("find user with like condition and between id") {
        INFO(User::where(User::Name::like("user_")).where(User::ID{1}).to_sql());
        if (const auto [error, users] = User::where(User::Name::like("user_")).where(User::ID{1}).exec(conn); error) {
            FAIL(error.value());
        }
        else {
            STATIC_REQUIRE(std::is_same_v<decltype(user_names), std::vector<User::Name>>);
            REQUIRE(user_names.size() == 1);
            REQUIRE(user_names[0].value() == active_record::string{ "user1" });
        }
    }

}