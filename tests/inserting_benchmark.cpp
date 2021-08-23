
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include "../include/active_record.hpp"
#include <filesystem>

struct User : public active_record::model<User> {
    static constexpr auto table_name = "user_table";

    struct ID : public active_record::attributes::integer<User, ID, std::size_t> {
        static constexpr auto column_name = "id";
        using active_record::attributes::integer<User, ID, size_t>::integer;

        inline static const auto constraints = { primary_key };
    } id;

    struct Name : public active_record::attributes::string<User, Name> {
        static constexpr auto column_name = "name";
        using active_record::attributes::string<User, Name>::string;

        inline static const auto constraints = { not_null, default_value("unknow") };
    } name;

    struct Height : public active_record::attributes::decimal<User, Height>{
        static constexpr auto column_name = "height";
        using active_record::attributes::decimal<User, Height>::decimal;
    } height;

    std::tuple<ID&, Name&, Height&> attributes = std::tie(id, name, height);
};


TEST_CASE("User model inserting benchmark"){
    // Benchmark
    BENCHMARK_ADVANCED("1000 data inserting bench")(Catch::Benchmark::Chronometer meter){
        using namespace active_record;
        // Setup
        auto conn = sqlite3_adaptor::open("user_model_test.sqlite3", sqlite3::options::create);
        if(const auto error = conn.create_table<User>(); error) {
            FAIL(error.value());
        }

        meter.measure([&conn](int k){
            const auto insert_transaction = [k](auto& connection){
                for(auto i = k*1000; i < (k+1)*1000; ++i){
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
            const auto [error, result] = conn.transaction(insert_transaction);
            if (error){ FAIL(error.value()); }
            else if(result == transaction::rollback) { FAIL("something happened"); }
        });


        // Finish
        conn.close();
        std::filesystem::remove("user_model_test.sqlite3");
    };
}