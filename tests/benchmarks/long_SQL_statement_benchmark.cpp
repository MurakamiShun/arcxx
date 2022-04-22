#include "user_model.hpp"

struct Clothes : public active_record::model<Clothes> {
    inline static decltype(auto) table_name = "clothes_table";

    struct ID : public active_record::attributes::integer<Clothes, ID, std::size_t> {
        inline static decltype(auto) column_name = "id";
        using integer<Clothes, ID, size_t>::integer;

        inline static const auto constraints = { primary_key };
    } id;

    struct UserID : public active_record::attributes::foreign_key<Clothes, UserID, User::ID> {
        inline static decltype(auto) column_name = "user_id";
        using foreign_key<Clothes, UserID, User::ID>::foreign_key;

        inline static const auto constraints = { not_null };
    } user_id;

    struct Name : public active_record::attributes::string<Clothes, Name> {
        inline static decltype(auto) column_name = "name";
        using string<Clothes, Name>::string;

        inline static const auto constraints = { not_null, default_value("unknow") };
    } name;

    struct Price : public active_record::attributes::integer<Clothes, Price, uint32_t>{
        inline static decltype(auto) column_name = "price";
        using integer<Clothes, Price, uint32_t>::integer;
    } price;
};

TEST_CASE("Long SQL statement benchmark"){
    // Benchmark
    WARN(
        Clothes::join<User>().where(
            User::Name::like("user%") &&
            User::Height::cmp < 175.0 &&
            Clothes::Price::between(10'000,50'000)
        ).order_by<Clothes::Price>().to_sql()
    );

    BENCHMARK_ADVANCED("Long SQL statement bench")(Catch::Benchmark::Chronometer meter){
        using namespace active_record;
        namespace ranges = std::ranges;

        meter.measure([](){
            std::size_t t = 0; // Optimization prevention
            for([[maybe_unused]]auto i : ranges::views::iota(0,10000)){
                t += Clothes::join<User>().where(
                    User::Name::like("user%") &&
                    User::Height::cmp < 175.0 &&
                    Clothes::Price::between(10'000,50'000)
                ).order_by<Clothes::Price>().to_sql().length();
            }
            return t;
        });
    };
}