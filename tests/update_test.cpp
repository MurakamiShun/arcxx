#include "user_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "Update query tests", "[model][query_relation][update][select]") {
    SECTION("Update name of user1"){
        INFO(User::where(User::ID{1}).update(User::Name{"my user1"}).to_sql());
        if (const auto error = User::where(User::ID{1}).update(User::Name{"my user1"}).exec(conn); error) {
            FAIL(error.value());
        }

        if (const auto [error, names] = User::pluck<User::Name>().where(User::ID{1}).exec(conn); error) {
            FAIL(error.value());
        }
        else {
            REQUIRE(names[0].value() == active_record::string{ "my user1" });
        }
    }
}