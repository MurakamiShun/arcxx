#include "user_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "Delete query tests", "[active_record::model][delete]") {
    SECTION("Delete users whose ids are 0 to 3.") {
        INFO(User::destroy(User::ID::between(0,3)).to_sql());
        
        std::size_t data_count_before_delete = get_data_count();
        REQUIRE(data_count_before_delete > 0);
        
        std::size_t expect_delete_count;
        if(const auto [error, data_count] = User::where(User::ID::between(0,3)).count().exec(conn); error){
            FAIL(error.value());
        }
        else expect_delete_count = data_count;
        REQUIRE(expect_delete_count > 0);

        if(const auto error = User::destroy(User::ID::between(0,3)).exec(conn); error){
            FAIL(error.value());
        }

        // Remaining users count will be data_count_before_delete - expect_delete_count.
        REQUIRE(get_data_count() == data_count_before_delete - expect_delete_count);
    }

    SECTION("Delete a user.") {
        INFO(User::destroy(User::ID{7}).to_sql());
        
        std::size_t data_count_before_delete = get_data_count();
        REQUIRE(data_count_before_delete > 0);
        
        if(const auto error = User::destroy(User::ID{7}).exec(conn); error){
            FAIL(error.value());
        }

        // Remaining users count will be data_count_before_delete - 1.
        REQUIRE(get_data_count() == data_count_before_delete - 1);
    }

    SECTION("Delete users whose names are like 'user_'.") {
        INFO(User::destroy(User::Name::like("user_")).to_sql());
        
        std::size_t data_count_before_delete = get_data_count();
        REQUIRE(data_count_before_delete > 0);
        
        if(const auto error = User::destroy(User::Name::like("user_")).exec(conn); error){
            FAIL(error.value());
        }

        // Remaining users count will be data_count_before_delete - 1.
        REQUIRE(get_data_count() == 0);
    }
}