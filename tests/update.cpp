//#include "../include/active_record.hpp"
#include "user_model.hpp"
#include <iostream>
#include <cassert>


int main(){
    auto connection = active_record::sqlite3_adaptor::open("update_test.sqlite3", active_record::sqlite3::options::create);
    
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

    // update user1 name
    std::cout << User::where(User::ID{1}).update(User::Name{"my user1"}).to_sql() << std::endl;
    if(const auto error = User::where(User::ID{1}).update(User::Name{"my user1"}).exec(connection); error){
        std::cout << "Error:" << error.value() << std::endl;
        assert(!error);
    }
    else{
        std::cout << "updated!" << std::endl;
    }

    // name will be "my user1".
    std::cout << User::pluck<User::Name>().where(User::ID{1}).to_sql() << std::endl;
    if(const auto [error, names] = User::pluck<User::Name>().where(User::ID{1}).exec(connection); error){
        std::cout << "Error:" << error.value() << std::endl;
        assert(!error);
    }
    else{
        std::cout << "updated user name: " << names[0].value() << std::endl;
        assert(names[0].value() == active_record::string{ "my user1" });
    }
    
    return 0;
}