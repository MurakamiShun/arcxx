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

    struct CreatedAt : public active_record::attributes::datetime<User, CreatedAt>{
        using datetime<User, CreatedAt>::datetime;
        inline static decltype(auto) column_name = "created_at";
        inline static const auto constraints = { not_null };

        CreatedAt(){ *this = time_point_cast<value_type::duration>(active_record::system_datetime::clock::now()); }
    } created_at;

    int i_am_structure_noise = 0;
};

/*
 * Test Fixture
 */

class UserModelTestsFixture {
protected:
    adaptor conn;
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
        if(const auto result = conn.create_table<User>(active_record::abort_if_exists); !result) {
            FAIL(result.error());
        }
        const auto insert_transaction = [](auto& connection){
            namespace trans = active_record::transaction;

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
