#include "user_model.hpp"
#include <iostream>

struct UserLog : public active_record::model<UserLog>{
    constexpr static auto table_name = "user_log_table";

    struct ID : public active_record::attributes::integer<UserLog, ID, std::size_t> {
        constexpr static auto column_name = "id";
        using active_record::attributes::integer<UserLog, ID, std::size_t>::integer;
        inline static const auto constraints = { primary_key };
    } id;

    struct UserID : public active_record::attributes::reference_to<UserLog, UserID, User::ID> {
        constexpr static auto column_name = "user_id";
        using active_record::attributes::reference_to<UserLog, UserID, User::ID>::reference_to;
    } user_id;

    struct Comment : public active_record::attributes::string<UserLog, Comment> {
        constexpr static auto column_name = "comment";
        using active_record::attributes::string<UserLog, Comment>::string;
    } comment;

    std::tuple<ID&, UserID&, Comment&> attributes = std::tie(id, user_id, comment);
};


TEST_CASE_METHOD(UserModelTestsFixture, "Join query tests", "[model][query_relation][select][join][insert]") {
    if (const auto error = conn.create_table<UserLog>(); error){
        INFO(UserLog::schema::to_sql<active_record::sqlite3_adaptor>());
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

    SECTION("finding other table column without join will be error") {
        INFO(UserLog::where(users[1].name).to_sql());
        auto [error, find_by_username] = UserLog::where(users[1].name).exec(conn);
        REQUIRE(error);
    }
}