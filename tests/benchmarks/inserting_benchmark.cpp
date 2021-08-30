
#include "user_model.hpp"

TEST_CASE("User model inserting benchmark"){
    // Benchmark
    BENCHMARK_ADVANCED("10000 data inserting bench")(Catch::Benchmark::Chronometer meter){
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