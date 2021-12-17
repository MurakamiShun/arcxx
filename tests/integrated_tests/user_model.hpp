#pragma once
#include "test_fixtures.hpp"

/*
 * User model (active record)
 */

struct User : public active_record::model<User> {
    inline static decltype(auto) table_name = "user_table";

    struct ID : public active_record::attributes::integer<User, ID, std::size_t> {
        inline static decltype(auto) column_name = "id";
        using integer<User, ID, size_t>::integer;

        inline static const auto constraints = { primary_key };
    } id;

    struct Name : public active_record::attributes::string<User, Name> {
        inline static decltype(auto) column_name = "name";
        using string<User, Name>::string;

        inline static const auto constraints = { not_null, default_value("unknow") };
    } name;

    struct Height : public active_record::attributes::decimal<User, Height>{
        inline static decltype(auto) column_name = "height";
        using decimal<User, Height>::decimal;
    } height;

    int i_am_structure_noise = 0;
};

/*
 * Test Fixture
 */

class UserModelTestsFixture {
protected:
    adaptor conn;
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
    UserModelTestsFixture() : conn(open_testfile()) {
        // Create test data
        conn.drop_table<User>();
        if(const auto error = conn.create_table<User>(false); error) {
            FAIL(error.value());
        }
        const auto insert_transaction = [](auto& connection){
            using transaction = active_record::transaction;

            for(auto i = 0; i < 10; ++i){
                User user;
                user.id = i;
                user.name = std::string{ "user" } + std::to_string(i);
                user.height = 170.0 + i;
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
        const auto error = conn.drop_table<User>();
        if(error) FAIL(error.value());
        close_testfile(conn);
    }
};
