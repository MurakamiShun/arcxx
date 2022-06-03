#include "user_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "Update query tests", "[model][query_relation][update][select]") {
    SECTION("Update name of user1"){
        INFO(User::where(User::ID{1}).update(User::Name{"my user1"}).to_sql());
        if (const auto result = User::where(User::ID{1}).update(User::Name{"my user1"}).exec(conn); !result) {
            FAIL(result.error());
        }

        if (const auto result = User::pluck<User::Name>().where(User::ID{1}).exec(conn); !result) {
            FAIL(result.error());
        }
        else {
            REQUIRE(result.value()[0] == arcxx::string{ "my user1" });
        }
    }
}