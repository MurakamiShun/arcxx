#include "user_model.hpp"
#include "user_log_model.hpp"

TEST_CASE_METHOD(UserModelTestsFixture, "group_by query tests", "[model][select][aggregation][insert]") {
    if (const auto result = conn.create_table<UserLog>(); !result){
        INFO(UserLog::schema::to_sql<connector>());
        FAIL(result.error());
    }
    auto users = User::where(User::ID::between(2,3)).exec(conn).value();
    REQUIRE(users.size() == 2);

    // insert user logs
    const auto insert_transaction = [&users](auto& connection){
        namespace transaction = arcxx::transaction;

        for(auto i = 0; i < 9; ++i){
            UserLog log;
            log.id = i;
            log.comment = std::string{ "created by user" } + std::to_string(i/5);
            log.user_id = users[i/5].id;
            if(const auto insert_result = UserLog::insert(log).exec(connection); !insert_result) {
                FAIL(insert_result.error());
                return transaction::rollback(insert_result.error());
            }
        }
        return transaction::commit;
    };

    if(const auto trans_result = conn.transaction(insert_transaction); !trans_result) {
        FAIL(trans_result.error());
    }

    SECTION("group by user_log.user_id"){
        INFO(UserLog::group_by<UserLog::UserID>().count().to_sql());
        const auto log_count = UserLog::group_by<UserLog::UserID>().count().exec(conn);
        if(!log_count) FAIL(log_count.error());
        
        REQUIRE(typeid(log_count.value()).name() == typeid(std::unordered_map<UserLog::UserID, std::size_t>).name());
        
        REQUIRE(log_count.value().size() == 2);
        REQUIRE(log_count.value().at(UserLog::UserID{2}) == 5);
        REQUIRE(log_count.value().at(UserLog::UserID{3}) == 4);
    }

    SECTION("aggregate group by user_log.user_id") {
        using UserID = UserLog::UserID;
        INFO(( UserLog::group_by<UserLog::UserID>().select<UserID::sum, UserID::avg, UserID::max, UserID::min>().to_sql()) );
        const auto log_aggregation_result = UserLog::group_by<UserLog::UserID>().select<UserID::sum, UserID::avg, UserID::max, UserID::min>().exec(conn);
        if(!log_aggregation_result) FAIL(log_aggregation_result.error());
        
        REQUIRE(typeid(log_aggregation_result.value()).name() ==
                typeid(std::unordered_map<UserLog::UserID, std::tuple<UserID::sum::attribute_type, UserID::avg::attribute_type, UserID::max::attribute_type, UserID::min::attribute_type>>).name());
        
        REQUIRE(log_aggregation_result.value().size() == 2);
        // sum
        REQUIRE(std::get<0>(log_aggregation_result.value().at(UserLog::UserID{2})) == 10);
        REQUIRE(std::get<0>(log_aggregation_result.value().at(UserLog::UserID{3})) == 12);

        // avg
        REQUIRE(std::get<1>(log_aggregation_result.value().at(UserLog::UserID{2})) == 2.0);
        REQUIRE(std::get<1>(log_aggregation_result.value().at(UserLog::UserID{3})) == 3.0);
    }

    conn.drop_table<UserLog>();
}