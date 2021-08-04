#include "../include/active_record.hpp"
#include "user_model.hpp"
#include <iostream>
#include <cassert>

int main(){
    auto connection = active_record::sqlite3_adaptor::open("insert_test.sqlite3", active_record::sqlite3::options::create);
    
    connection.create_table<User>();

    const auto insert_transaction = [](auto& connection){
        using namespace active_record;
        using transaction = active_record::transaction;

        for(auto i = 0; i < 10; ++i){
            User user;
            user.id = i;
            user.name = std::string{ "user" } + std::to_string(i);
            if(const auto error = User::insert(user).exec(connection); error){
                std::cout << "Error:" << error.value() << std::endl;
                return transaction::rollback;
            }
        }
        return transaction::commit;
    };

    if(const auto [error, trans_result] = connection.transaction(insert_transaction); error){
        std::cout << "Error:" << error.value() << std::endl;
        assert(!error);
    }
    else if(trans_result == active_record::transaction::rollback){
        std::cout << "Rollbacked" << std::endl;
        assert(false);
    }

    std::cout << User::count().to_sql() << std::endl;
    if(const auto [error, count] = User::count().exec(connection); error){
        std::cout << "Error:" << error.value() << std::endl;
        assert(!error);
    }
    else{
        std::cout << "User count:" << count << std::endl;
        assert(count == 10);
    }

    std::cout << User::sum<User::ID>().to_sql() << std::endl;
    if(const auto [error, total] = User::sum<User::ID>().exec(connection); error){
        std::cout << "Error:" << error.value() << std::endl;
        assert(!error);
    }
    else{
        std::cout << "User sum(id):" << total << std::endl;
        assert(total == 45);
    }


    std::cout << User::where(User::ID::between(0,4)).count().to_sql() << std::endl;
    if(const auto [error, count] = User::where(User::ID::between(0,4)).count().exec(connection); error){
        std::cout << "Error:" << error.value() << std::endl;
        assert(!error);
    }
    else{
        std::cout << "User count:" << count << std::endl;
        assert(count == 5);
    }
    
    return 0;
}