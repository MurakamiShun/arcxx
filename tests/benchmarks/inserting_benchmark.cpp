
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <ranges>
#include "../../include/active_record.hpp"

struct User : public active_record::model<User> {
    inline static decltype(auto) table_name = "user_table";

    struct ID : public active_record::attributes::integer<User, ID, std::size_t> {
        inline static decltype(auto) column_name = "id";
        using active_record::attributes::integer<User, ID, size_t>::integer;

        inline static const auto constraints = { primary_key };
    } id;

    struct Name : public active_record::attributes::string<User, Name> {
        inline static decltype(auto) column_name = "name";
        using active_record::attributes::string<User, Name>::string;

        inline static const auto constraints = { not_null, default_value("unknow") };
    } name;

    struct Height : public active_record::attributes::decimal<User, Height>{
        inline static decltype(auto) column_name = "height";
        using active_record::attributes::decimal<User, Height>::decimal;
    } height;

    std::tuple<ID&, Name&, Height&> attributes = std::tie(id, name, height);
};


TEST_CASE("User model inserting benchmark"){
    // Benchmark
    BENCHMARK_ADVANCED("1000 data inserting bench")(Catch::Benchmark::Chronometer meter){
        using namespace active_record;
        namespace ranges = std::ranges;

        const auto users = []{
            std::array<User, 10000> users;
            for(auto i : ranges::views::iota(0,10000)){
                users[i].id = i;
                users[i].name = std::string{ "user" } + std::to_string(i);
                users[i].height = 170.0 + i;
            }
            return users;
        }();

        meter.measure([&users](){
            std::size_t t = 0; // Optimization prevention
            for(const auto& user : users) {
                t += User::insert(user).to_sql().length();
            }
            return t;
        });
    };
}