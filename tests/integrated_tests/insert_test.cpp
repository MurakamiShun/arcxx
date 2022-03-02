#include "user_model.hpp"


TEST_CASE("Insert query tests", "[model][query_relation][insert][select]") {
    auto connection = open_testfile();
    
    connection.drop_table<User>();
    connection.create_table<User>();

    const auto insert_transaction = [](auto& connection){
        namespace transaction = active_record::transaction;

        for(auto i = 0; i < 10; ++i){
            User user;
            user.id = i;
            user.name = std::string{ "user" } + std::to_string(i);

            if(const auto result = User::insert(user).exec(connection); !result){
                FAIL(result.error());
                return transaction::rollback(result.error());
            }
        }
        return transaction::commit;
    };

    if(const auto trans_result = connection.transaction(insert_transaction); !trans_result){
        FAIL(trans_result.error());
    }

    INFO(User::count().to_sql());
    if(const auto count_result = User::count().exec(connection); !count_result){
        FAIL(count_result.error());
    }
    else{
        REQUIRE(count_result.value() == 10);
    }

    INFO(User::sum<User::ID>().to_sql());
    if(const auto total_result = User::sum<User::ID>().exec(connection); !total_result){
        FAIL(total_result.error());
    }
    else{
        REQUIRE(total_result.value() == 45);
    }

    connection.drop_table<User>();
    close_testfile(connection);
}
