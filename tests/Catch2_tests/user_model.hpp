#pragma once
#include "../../include/active_record.hpp"
#include <filesystem>
#include <catch2/catch.hpp>

/*
 * User model (active record)
 */

struct User : public active_record::model<User> {
    static constexpr auto table_name = "user_table";

    struct ID : public active_record::attributes::integer<User, ID, std::size_t> {
        static constexpr auto column_name = "id";
        using active_record::attributes::integer<User, ID, size_t>::integer;

        inline static const auto constraints = { primary_key };
        constexpr ~ID(){}
    } id;

    struct Name : public active_record::attributes::string<User, Name> {
        static constexpr auto column_name = "name";
        using active_record::attributes::string<User, Name>::string;

        inline static const auto constraints = { not_null, default_value("unknow") };
        constexpr ~Name(){}
    } name;

    std::tuple<ID&, Name&> attributes = std::tie(id, name);
};

/*
 * Test Fixture
 */

class UserModelTestsFixture {
protected:
    active_record::sqlite3::adaptor conn;
    std::size_t get_data_count(){
        if(const auto [error, data_count] = User::count().exec(conn); error){
            FAIL(error.value());
        }
        else {
            return data_count;
        }
        return -1;
    }
public:
    UserModelTestsFixture() : conn(active_record::sqlite3_adaptor::open("user_model_test.sqlite3", active_record::sqlite3::options::create)) {
        // Create test data
        if(const auto error = conn.create_table<User>(); error) {
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

        if(const auto [error, trans_result] = conn.transaction(insert_transaction); error) {
            FAIL(error.value());
        }    
        else if(trans_result == active_record::transaction::rollback) {
            FAIL("Insert error happned!!");
        }
    }
    ~UserModelTestsFixture(){
        conn.close();
        std::filesystem::remove("user_model_test.sqlite3");
    }
};