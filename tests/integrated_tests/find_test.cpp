#include "user_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "Find query tests", "[model][query_relation][select]") {
    SECTION("equal condition test") {
        // same result
        INFO(User::where(User::ID{1}).to_sql());
        INFO(User::where(User::ID::cmp == 1).to_sql());

        if (const auto result = User::where(User::ID{1}).exec(conn); !result) {
            FAIL(result.error());
        }
        else {
            REQUIRE(result.value().size() == 1);
            REQUIRE(result.value()[0].id == 1);
        }

        if (const auto result = User::where(User::ID::cmp == 1).exec(conn); !result) {
            FAIL(result.error());
        }
        else {
            REQUIRE(result.value().size() == 1);
            REQUIRE(result.value()[0].id == 1);
        }
    }

    SECTION("not equal condition test") {
        INFO(User::where(User::ID::cmp != 2).to_sql());

        if (const auto find_users_result = User::where(User::ID::cmp < 2).exec(conn); !find_users_result) {
            FAIL(find_users_result.error());
        }
        else {
            const auto& users = find_users_result.value();
            REQUIRE(users.size() == 2);
            REQUIRE(std::find_if(users.begin(), users.end(), [](const auto& user){ return user.id == 2; }) == users.end());
        }
    }
    
    SECTION("less condition test") {
        INFO(User::where(User::ID::cmp < 2).to_sql());

        if (const auto find_users_result = User::where(User::ID::cmp < 2).exec(conn); !find_users_result) {
            FAIL(find_users_result.error());
        }
        else {
            REQUIRE(find_users_result.value().size() == 2);
            REQUIRE(find_users_result.value()[0].id == 0);
            REQUIRE(find_users_result.value()[1].id == 1);
        }
    }

    SECTION("less or equal condition test") {
        INFO(User::where(User::ID::cmp <= 2).to_sql());

        if (const auto find_users_result = User::where(User::ID::cmp <= 2).exec(conn); !find_users_result) {
            FAIL(find_users_result.error());
        }
        else {
            REQUIRE(find_users_result.value().size() == 3);
            REQUIRE(find_users_result.value()[0].id == 0);
            REQUIRE(find_users_result.value()[2].id == 2);
        }
    }
    
    SECTION("greater condition test") {
        INFO(User::where(User::ID::cmp > 7).to_sql());

        if (const auto find_users_result = User::where(User::ID::cmp > 7).exec(conn); !find_users_result) {
            FAIL(find_users_result.error());
        }
        else {
            REQUIRE(find_users_result.value().size() == 2);
            REQUIRE(find_users_result.value()[0].id == 8);
            REQUIRE(find_users_result.value()[1].id == 9);
        }
    }

    SECTION("greater or equal condition test") {
        INFO(User::where(User::ID::cmp >= 7).to_sql());

        if (const auto find_users_result = User::where(User::ID::cmp >= 7).exec(conn); !find_users_result) {
            FAIL(find_users_result.error());
        }
        else {
            REQUIRE(find_users_result.value().size() == 3);
            REQUIRE(find_users_result.value()[0].id == 7);
            REQUIRE(find_users_result.value()[2].id == 9);
        }
    }

    SECTION("find user with like condition and between id") {
        INFO(User::where(User::Name::like("user_")).where(User::ID{1}).to_sql());
        if (const auto find_users_result1 = User::where(User::Name::like("user_")).where(User::ID{1}).exec(conn); !find_users_result1) {
            FAIL(find_users_result1.error());
        }
        else{
            if (const auto find_users_result2 = User::where(User::Name::like("user_") && User::ID::cmp == 1).exec(conn); !find_users_result2) {
                FAIL(find_users_result2.error());
            }
            else {
                const auto& users1 = find_users_result1.value();
                const auto& users2 = find_users_result2.value();
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