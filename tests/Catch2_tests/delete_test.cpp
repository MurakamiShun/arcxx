#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <filesystem>

#include "user_model.hpp"

TEST_CASE( "delete query tests", "[active_record::model]" ) {
    // Create test data
    auto connection = active_record::sqlite3_adaptor::open("delete_test.sqlite3", active_record::sqlite3::options::create);
    if(const auto error = connection.create_table<User>(); error) {
        FAIL(error.value());
    }
    const auto insert_transaction = [](auto& connection){
        using transaction = active_record::transaction;

        for(auto i = 0; i < 10; ++i){
            User user;
            user.id = i;
            user.name = std::string{ "user" } + std::to_string(i);
            if(const auto error = User::insert(user).exec(connection); error) {
                FAIL(error.value());
                return transaction::rollback;
            }
        }
        return transaction::commit;
    };

    if(const auto [error, trans_result] = connection.transaction(insert_transaction); error) {
        FAIL(error.value());
    }    
    else if(trans_result == active_record::transaction::rollback) {
        FAIL("insert error happned!!");
    }

    SECTION("") {

    }

    std::filesystem::remove("delete_test.sqlite3");
}