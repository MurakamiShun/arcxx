#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "user_model.hpp"


TEST_CASE("Insert query tests", "[model][query_relation][insert][select]") {
    auto connection = open_testfile();
    
    connection.drop_table<User>();
    connection.create_table<User>();

    const auto insert_transaction = [](auto& connection){
        using namespace active_record;
        using transaction = active_record::transaction;

        for(auto i = 0; i < 10; ++i){
            User user;
            user.id = i;
            user.name = std::string{ "user" } + std::to_string(i);

            if(const auto error = User::insert(user).exec(connection); error){
                FAIL(error.value());
                return transaction::rollback;
            }
        }
        return transaction::commit;
    };

    if(const auto [error, trans_result] = connection.transaction(insert_transaction); error){
        FAIL(error.value());
    }

    INFO(User::count().to_sql());
    if(const auto [error, count] = User::count().exec(connection); error){
        FAIL(error.value());
    }
    else{
        REQUIRE(count == 10);
    }

    INFO(User::sum<User::ID>().to_sql());
    if(const auto [error, total] = User::sum<User::ID>().exec(connection); error){
        FAIL(error.value());
    }
    else{
        REQUIRE(total == 45);
    }

    connection.drop_table<User>();
    close_testfile(connection);
}
