//#include "../include/active_record.hpp"
#include "user_model.hpp"
#include <iostream>
#include <cassert>


int main(){
    auto connection = active_record::sqlite3_adaptor::open("delete_test.sqlite3", active_record::sqlite3::options::create);
    
    // create table
    connection.create_table<User>();

    // insert test users
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

    // get users count will be 10
    std::cout << User::count().to_sql() << std::endl;
    if(const auto [error, count] = User::count().exec(connection); error){
        std::cout << "Error:" << error.value() << std::endl;
        assert(!error);
    }
    else{
        std::cout << "User count:" << count << std::endl;
        assert(count == 10);
    }

    // delete users whose ids are 0 to 3
    std::cout << User::destroy(User::ID::between(0,3)).to_sql() << std::endl;
    if(const auto error = User::destroy(User::ID::between(0,3)).exec(connection); error){
        std::cout << "Error:" << error.value() << std::endl;
        assert(!error);
    }
    else{
        std::cout << "Deleted!" << std::endl;
        //assert(count == 4);
    }

    // Remaining users count will be 5.
    std::cout << User::count().to_sql() << std::endl;
    if(const auto [error, count] = User::count().exec(connection); error){
        std::cout << "Error:" << error.value() << std::endl;
        assert(!error);
    }
    else{
        std::cout << "Remaining count:" << count << std::endl;
        assert(count == 6);
    }
    
    return 0;
}