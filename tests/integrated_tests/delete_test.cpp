#include "user_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "Delete query tests", "[model][query_relation][delete][select]") {
    SECTION("Delete users whose ids are 0 to 3.") {
        INFO(User::destroy(User::ID::between(0,3)).to_sql());
        
        std::size_t data_count_before_delete = get_data_count();
        REQUIRE(data_count_before_delete > 0);
        
        std::size_t expect_delete_count;
        if(const auto data_count_result = User::where(User::ID::between(0,3)).count().exec(conn); !data_count_result){
            FAIL(data_count_result.error());
        }
        else expect_delete_count = data_count_result.value();
        REQUIRE(expect_delete_count > 0);

        if(const auto destoy_result = User::destroy(User::ID::between(0,3)).exec(conn); !destoy_result){
            FAIL(destoy_result.error());
        }

        // Remaining users count will be data_count_before_delete - expect_delete_count.
        REQUIRE(get_data_count() == data_count_before_delete - expect_delete_count);
    }

    SECTION("Delete a user.") {
        INFO(User::destroy(User::ID{7}).to_sql());
        
        std::size_t data_count_before_delete = get_data_count();
        REQUIRE(data_count_before_delete > 0);
        
        if(const auto destory_result = User::destroy(User::ID{7}).exec(conn); !destory_result){
            FAIL(destory_result.error());
        }

        // Remaining users count will be data_count_before_delete - 1.
        REQUIRE(get_data_count() == data_count_before_delete - 1);
    }

    SECTION("Delete users whose names are like 'user_'.") {
        INFO(User::destroy(User::Name::like("user_")).to_sql());
        
        std::size_t data_count_before_delete = get_data_count();
        REQUIRE(data_count_before_delete > 0);
        
        if(const auto destory_result = User::destroy(User::Name::like("user_")).exec(conn); !destory_result){
            FAIL(destory_result.error());
        }

        // Remaining users count will be data_count_before_delete - 1.
        REQUIRE(get_data_count() == 0);
    }
}