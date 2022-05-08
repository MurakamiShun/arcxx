#include "user_model.hpp"

TEST_CASE("Table creation tests", "[model]") {
    adaptor conn = open_testfile();

    SECTION("create user model"){
        conn.drop_table<User>();
        if(const auto result = conn.create_table<User>(); !result) {
            FAIL(result.error());
        }
        REQUIRE(conn.exists_table<User>() == true);
        
        const auto result = conn.drop_table<User>();
        if(!result) FAIL(result.error());
        REQUIRE(conn.exists_table<User>() == false);
    }
    close_testfile(conn);
}