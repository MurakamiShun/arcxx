#include "user_model.hpp"
#include "user_log_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "group_by query tests", "[model][select][aggregation][insert]") {
    if (const auto error = conn.create_table<UserLog>(); error){
        INFO(UserLog::schema::to_sql<active_record::sqlite3_adaptor>());
        FAIL(error.value());
    }
    auto users = User::where(User::ID::between(2,3)).exec(conn).second;
    REQUIRE(users.size() == 2);

    // insert user logs
    const auto insert_transaction = [&users](auto& connection){
        using transaction = active_record::transaction;

        for(auto i = 0; i < 9; ++i){
            UserLog log;
            log.id = i;
            log.comment = std::string{ "created by user" } + std::to_string(i/5);
            log.user_id = users[i/5].id;
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

    SECTION("group by user_log.user_id"){
        INFO(UserLog::group_by<UserLog::UserID>().count().to_sql());
        const auto [error, log_count] = UserLog::group_by<UserLog::UserID>().count().exec(conn);
        if(error) FAIL(error.value());
        
        REQUIRE(typeid(log_count).name() == typeid(std::unordered_map<UserLog::UserID, std::size_t>).name());
        
        REQUIRE(log_count.size() == 2);
        REQUIRE(log_count.at(UserLog::UserID{2}) == 5);
        REQUIRE(log_count.at(UserLog::UserID{3}) == 4);
    }

    SECTION("aggregate group by user_log.user_id") {
        using UserID = UserLog::UserID;
        INFO(( UserLog::group_by<UserLog::UserID>().select<UserID::sum, UserID::avg, UserID::max, UserID::min>().to_sql()) );
        const auto [error, log_ag] = UserLog::group_by<UserLog::UserID>().select<UserID::sum, UserID::avg, UserID::max, UserID::min>().exec(conn);
        if(error) FAIL(error.value());
        
        REQUIRE(typeid(log_ag).name() ==
                typeid(std::unordered_map<UserLog::UserID, std::tuple<UserID::sum::attribute_type, UserID::avg::attribute_type, UserID::max::attribute_type, UserID::min::attribute_type>>).name());
        
        REQUIRE(log_ag.size() == 2);
        // sum
        REQUIRE(std::get<0>(log_ag.at(UserLog::UserID{2})).value() == 10);
        REQUIRE(std::get<0>(log_ag.at(UserLog::UserID{3})).value() == 12);

        // avg
        REQUIRE(std::get<1>(log_ag.at(UserLog::UserID{2})).value() == 2.0);
        REQUIRE(std::get<1>(log_ag.at(UserLog::UserID{3})).value() == 3.0);
    }
}