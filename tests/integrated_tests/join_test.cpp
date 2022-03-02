#include "user_model.hpp"
#include "user_log_model.hpp"


TEST_CASE_METHOD(UserModelTestsFixture, "Join query tests", "[model][query_relation][select][join][insert]") {
    if (const auto result = conn.create_table<UserLog>(); !result){
        INFO(UserLog::schema::to_sql<adaptor>());
        FAIL(result.error());
    }
    auto users = User::where(User::ID::between(2,3)).exec(conn).value();
    REQUIRE(users.size() == 2);

    // insert user logs
    const auto insert_transaction = [&users](auto& connection){
        namespace transaction = active_record::transaction;

        for(auto i = 0; i < 10; ++i){
            UserLog log;
            log.id = i;
            log.comment = std::string{ "created by user" } + std::to_string(i/5);
            log.user_id = users[i/5].id;
            if(const auto result = UserLog::insert(log).exec(connection); !result) {
                FAIL(result.error());
                return transaction::rollback(result.error());
            }
        }
        // user_id == null
        for(auto i = 10; i < 15; ++i){
            UserLog log;
            log.id = i;
            log.comment = std::string{ "null user_id log" } + std::to_string(i);
            if(const auto result = UserLog::insert(log).exec(connection); !result) {
                FAIL(result.error());
                return transaction::rollback(result.error());
            }
        }
        return transaction::commit;
    };

    if(const auto result = conn.transaction(insert_transaction); !result) {
        FAIL(result.error());
    }

    SECTION("find user2 log") {
        INFO(UserLog::join<User>().where(users[1].name).pluck<UserLog::ID>().to_sql());
        auto find_by_username_result = UserLog::join<User>().where(users[1].name).pluck<UserLog::ID>().exec(conn);
        if (!find_by_username_result) FAIL(find_by_username_result.error());
        REQUIRE(find_by_username_result.value().size() == 5);

        INFO(UserLog::where(UserLog::UserID{users[1].id }).pluck<UserLog::ID>().to_sql());
        auto find_by_user_id_result = UserLog::where(UserLog::UserID{users[1].id }).pluck<UserLog::ID>().exec(conn);
        if (!find_by_user_id_result) FAIL(find_by_user_id_result.error());
        REQUIRE(find_by_user_id_result.value().size() == 5);

        // above queries are same result
        REQUIRE(find_by_username_result.value() == find_by_user_id_result.value());
    }

    SECTION("select multi table columns") {
        INFO((UserLog::join<User>().select<UserLog::ID, UserLog::Comment, User::Name>().to_sql()));
        auto log_with_usernames = UserLog::join<User>().select<UserLog::ID, UserLog::Comment, User::Name>().exec(conn);
        if (!log_with_usernames) FAIL(log_with_usernames.error());

        REQUIRE(log_with_usernames.value().size() == 10);
        REQUIRE(typeid(decltype(log_with_usernames.value()[0])).name() == typeid(std::tuple<UserLog::ID, UserLog::Comment, User::Name>).name());
    }

    SECTION("finding other table column without join will be error") {
        INFO(UserLog::where(users[1].name).to_sql());
        auto find_by_username = UserLog::where(users[1].name).exec(conn);
        REQUIRE(!find_by_username);
    }

    SECTION("testing left join and outer join"){
        INFO(UserLog::join<User>().count().to_sql());
        auto outer_join_count = UserLog::UserLog::join<User>().count().exec(conn);
        if(!outer_join_count) FAIL(outer_join_count.error());
        REQUIRE(outer_join_count.value() == 10);

        INFO(UserLog::left_join<User>().count().to_sql());
        auto left_join_count = UserLog::UserLog::left_join<User>().count().exec(conn);
        if(!left_join_count) FAIL(left_join_count.error());
        REQUIRE(left_join_count.value() == 15);
    }

    conn.drop_table<UserLog>();
}