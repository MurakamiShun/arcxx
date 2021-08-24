#include "user_model.hpp"
#include "user_log_model.hpp"


TEST_CASE_METHOD(UserModelTestsFixture, "Join query tests", "[model][query_relation][select][join][insert]") {
    if (const auto error = conn.create_table<UserLog>(); error){
        INFO(UserLog::schema::to_sql<adaptor>());
        FAIL(error.value());
    }
    auto users = User::where(User::ID::between(2,3)).exec(conn).second;
    REQUIRE(users.size() == 2);

    // insert user logs
    const auto insert_transaction = [&users](auto& connection){
        using transaction = active_record::transaction;

        for(auto i = 0; i < 10; ++i){
            UserLog log;
            log.id = i;
            log.comment = std::string{ "created by user" } + std::to_string(i/5);
            log.user_id = users[i/5].id;
            if(const auto error = UserLog::insert(log).exec(connection); error) {
                FAIL(error.value());
                return transaction::rollback;
            }
        }
        // user_id == null
        for(auto i = 10; i < 15; ++i){
            UserLog log;
            log.id = i;
            log.comment = std::string{ "null user_id log" } + std::to_string(i);
            if(const auto error = UserLog::insert(log).exec(connection); error) {
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

    SECTION("find user2 log") {
        INFO(UserLog::join<User>().where(users[1].name).pluck<UserLog::ID>().to_sql());
        auto [error1, find_by_username] = UserLog::join<User>().where(users[1].name).pluck<UserLog::ID>().exec(conn);
        if (error1) FAIL(error1.value());
        REQUIRE(find_by_username.size() == 5);

        INFO(UserLog::where(UserLog::UserID{users[1].id }).pluck<UserLog::ID>().to_sql());
        auto [error2, find_by_userid] = UserLog::where(UserLog::UserID{users[1].id }).pluck<UserLog::ID>().exec(conn);
        if (error2) FAIL(error2.value());
        REQUIRE(find_by_userid.size() == 5);

        // above queries are same result
        REQUIRE(find_by_username == find_by_userid);
    }

    SECTION("select multi table columns") {
        INFO((UserLog::join<User>().select<UserLog::ID, UserLog::Comment, User::Name>().to_sql()));
        auto [error1, log_with_usernames] = UserLog::join<User>().select<UserLog::ID, UserLog::Comment, User::Name>().exec(conn);
        if (error1) FAIL(error1.value());

        REQUIRE(log_with_usernames.size() == 10);
        REQUIRE(typeid(decltype(log_with_usernames)::value_type).name() == typeid(std::tuple<UserLog::ID, UserLog::Comment, User::Name>).name());
    }

    SECTION("finding other table column without join will be error") {
        INFO(UserLog::where(users[1].name).to_sql());
        auto [error, find_by_username] = UserLog::where(users[1].name).exec(conn);
        REQUIRE(error);
    }

    SECTION("testing left join and outer join"){
        INFO(UserLog::join<User>().count().to_sql());
        auto [error1, outer_join_count] = UserLog::UserLog::join<User>().count().exec(conn);
        if(error1) FAIL(error1.value());
        REQUIRE(outer_join_count == 10);

        INFO(UserLog::left_join<User>().count().to_sql());
        auto [error2, left_join_count] = UserLog::UserLog::left_join<User>().count().exec(conn);
        if(error2) FAIL(error2.value());
        REQUIRE(left_join_count == 15);
    }

    conn.drop_table<UserLog>();
}