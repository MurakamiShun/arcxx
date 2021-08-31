#include "user_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "Find query tests", "[model][query_relation][select]") {
    SECTION("equal condition test") {
        // same result
        INFO(User::where(User::ID{1}).to_sql());
        INFO(User::where(User::ID::cmp == 1).to_sql());

        if (const auto [error, user] = User::where(User::ID{1}).exec(conn); error) {
            FAIL(error.value());
        }
        else {
            REQUIRE(user.size() == 1);
            REQUIRE(user[0].id.value() == 1);
        }

        if (const auto [error, user] = User::where(User::ID::cmp == 1).exec(conn); error) {
            FAIL(error.value());
        }
        else {
            REQUIRE(user.size() == 1);
            REQUIRE(user[0].id.value() == 1);
        }
    }

    SECTION("not equal condition test") {
        INFO(User::where(User::ID::cmp != 2).to_sql());

        if (const auto [error, users] = User::where(User::ID::cmp < 2).exec(conn); error) {
            FAIL(error.value());
        }
        else {
            REQUIRE(users.size() == 2);
            REQUIRE(std::find_if(users.begin(), users.end(), [](const auto& user){ return user.id == 2; }) == users.end());
        }
    }
    
    SECTION("less condition test") {
        INFO(User::where(User::ID::cmp < 2).to_sql());

        if (const auto [error, users] = User::where(User::ID::cmp < 2).exec(conn); error) {
            FAIL(error.value());
        }
        else {
            REQUIRE(users.size() == 2);
            REQUIRE(users[0].id.value() == 0);
            REQUIRE(users[1].id.value() == 1);
        }
    }

    SECTION("less or equal condition test") {
        INFO(User::where(User::ID::cmp <= 2).to_sql());

        if (const auto [error, users] = User::where(User::ID::cmp <= 2).exec(conn); error) {
            FAIL(error.value());
        }
        else {
            REQUIRE(users.size() == 3);
            REQUIRE(users[0].id.value() == 0);
            REQUIRE(users[2].id.value() == 2);
        }
    }
    
    SECTION("greater condition test") {
        INFO(User::where(User::ID::cmp > 7).to_sql());

        if (const auto [error, users] = User::where(User::ID::cmp > 7).exec(conn); error) {
            FAIL(error.value());
        }
        else {
            REQUIRE(users.size() == 2);
            REQUIRE(users[0].id.value() == 8);
            REQUIRE(users[1].id.value() == 9);
        }
    }

    SECTION("greater or equal condition test") {
        INFO(User::where(User::ID::cmp >= 7).to_sql());

        if (const auto [error, users] = User::where(User::ID::cmp >= 7).exec(conn); error) {
            FAIL(error.value());
        }
        else {
            REQUIRE(users.size() == 3);
            REQUIRE(users[0].id.value() == 7);
            REQUIRE(users[2].id.value() == 9);
        }
    }

    SECTION("find user with like condition and between id") {
        INFO(User::where(User::Name::like("user_")).where(User::ID{1}).to_sql());
        if (const auto [error1, users1] = User::where(User::Name::like("user_")).where(User::ID{1}).exec(conn); error1) {
            FAIL(error1.value());
        }
        else{
            if (const auto [error2, users2] = User::where(User::Name::like("user_") && User::ID::cmp == 1).exec(conn); error2) {
                FAIL(error2.value());
            }
            else {
                REQUIRE(typeid(users1).name() == typeid(std::vector<User>).name());
                //STATIC_REQUIRE(std::is_same_v<decltype(users), std::vector<User::Name>>);
                REQUIRE(users1.size() == 1);
                REQUIRE(users2.size() == 1);
                REQUIRE(users1[0].name.value() == active_record::string{ "user1" });
                REQUIRE(users2[0].name.value() == active_record::string{ "user1" });
            }
        }
    }
}