#pragma once
#include "test_fixtures.hpp"

/*
 * User model
 */

struct User : public arcxx::model<User> {
    inline static decltype(auto) table_name = "user_table";

    struct ID : public arcxx::attributes::integer<User, ID, std::size_t> {
        inline static decltype(auto) column_name = "id";
        using integer<User, ID, size_t>::integer;

        inline static const auto constraints = primary_key;
    } id;

    struct Name : public arcxx::attributes::string<User, Name> {
        inline static decltype(auto) column_name = "name";
        using string<User, Name>::string;

        inline static const auto constraints = not_null & default_value("unknow");
    } name;

    struct Height : public arcxx::attributes::decimal<User, Height>{
        inline static decltype(auto) column_name = "height";
        using decimal<User, Height>::decimal;
    } height;

    struct CreatedAt : public arcxx::attributes::datetime<User, CreatedAt>{
        using datetime<User, CreatedAt>::datetime;
        inline static decltype(auto) column_name = "created_at";
        inline static const auto constraints = not_null;

        CreatedAt(){ *this = time_point_cast<value_type::duration>(arcxx::system_datetime::clock::now()); }
    } created_at;

    int i_am_structure_noise = 0;
};

/*
 * Test Fixture
 */

class UserModelTestsFixture {
protected:
    connector conn;
    std::size_t get_data_count(){
        if(const auto result = User::count().exec(conn); result.has_value()){
            return result.value();
        }
        else { // error
            FAIL(result.error());
        }
        return -1;
    }
public:
    UserModelTestsFixture() : conn(open_testfile()) {
        conn.drop_table<User>();
        // Create test data
        if(const auto result = conn.create_table<User>(arcxx::abort_if_exists); !result) {
            FAIL(result.error());
        }
        const auto insert_transaction = [](auto& connection){
            namespace trans = arcxx::transaction;

            for(auto i = 0; i < 10; ++i){
                User user;
                user.id = i;
                user.name = std::string{ "user" } + std::to_string(i);
                user.height = 170.0 + i;
                if(const auto result = User::insert(user).exec(connection); !result) {
                    WARN(result.error());
                    return trans::rollback(result.error());
                }
            }
            return trans::commit;
        };
        if(const auto trans_result = conn.transaction(insert_transaction); !trans_result) {
            FAIL(trans_result.error());
        }
    }
    ~UserModelTestsFixture(){
        conn.drop_table<User>();
        close_testfile(conn);
    }
};
