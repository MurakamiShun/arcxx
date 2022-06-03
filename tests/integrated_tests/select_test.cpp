#include "user_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "Select column query tests", "[model][query_relation][select]") {
    SECTION("select user name"){
        INFO(User::select<User::Name>().where(User::Name{"user1"}).to_sql());
        if (const auto result = User::select<User::Name>().where(User::Name{"user1"}).exec(conn); !result) {
            FAIL(result.error());
        }
        else {
            REQUIRE(typeid(result.value()).name() == typeid(std::vector<std::tuple<User::Name>>).name());
            REQUIRE(result.value().size() == 1);
            REQUIRE(std::get<0>(result.value()[0]).value() == arcxx::string{ "user1" });
        }
    }

    SECTION("pluck user name") {
        INFO(User::pluck<User::Name>().where(User::ID{1}).to_sql());
        if (const auto result = User::pluck<User::Name>().where(User::ID{1}).exec(conn); !result) {
            FAIL(result.error());
        }
        else {
            REQUIRE(typeid(result.value()).name() == typeid(std::vector<User::Name>).name());
            REQUIRE(result.value().size() == 1);
            REQUIRE(result.value()[0].value() == arcxx::string{ "user1" });
        }
    }
}