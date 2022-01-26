#include "user_model.hpp"

TEST_CASE("Table creation tests", "[model]") {
    adaptor conn(open_testfile());

    SECTION("create user model"){
        conn.drop_table<User>();
        if(const auto error = conn.create_table<User>(false); error) {
            FAIL(error.value());
        }
        REQUIRE(conn.exists_table<User>() == true);
        
        const auto error = conn.drop_table<User>();
        if(error) FAIL(error.value());
        REQUIRE(conn.exists_table<User>() == false);
    }
    close_testfile(conn);
}