#include "user_model.hpp"

struct Clothes : public arcxx::model<Clothes> {
    inline static decltype(auto) table_name = "clothes_table";

    struct ID : public arcxx::attributes::integer<Clothes, ID, std::size_t> {
        inline static decltype(auto) column_name = "id";
        using integer<Clothes, ID, size_t>::integer;

        inline static const auto constraints = primary_key;
    } id;

    struct UserID : public arcxx::attributes::foreign_key<Clothes, UserID, User::ID> {
        inline static decltype(auto) column_name = "user_id";
        using foreign_key<Clothes, UserID, User::ID>::foreign_key;

        inline static const auto constraints = not_null;
    } user_id;

    struct Name : public arcxx::attributes::string<Clothes, Name> {
        inline static decltype(auto) column_name = "name";
        using string<Clothes, Name>::string;

        inline static const auto constraints = not_null & default_value("unknow");
    } name;

    struct Price : public arcxx::attributes::integer<Clothes, Price, uint32_t>{
        inline static decltype(auto) column_name = "price";
        using integer<Clothes, Price, uint32_t>::integer;
    } price;

    struct UpdateAt : public arcxx::attributes::datetime<Clothes, UpdateAt>{
        inline static decltype(auto) column_name = "update_at";
        using datetime<Clothes, UpdateAt>::datetime;
    } update_at;
};

TEST_CASE("Long SQL statement benchmark"){
    // Benchmark
    using namespace std::chrono;
    using namespace std::chrono_literals;
    WARN(
        Clothes::join<User>().where(
            User::Name::like("user%") &&
            User::Height::cmp < 175.0 &&
            Clothes::Price::between(10'000,50'000) &&
            Clothes::UpdateAt::between(
                sys_days(2022y/April/1d) + 00s,
                sys_days(2022y/April/last) + 23h + 59min + 59s
            )
        ).order_by<Clothes::Price>().to_sql()
    );

    BENCHMARK_ADVANCED("Long SQL statement bench")(Catch::Benchmark::Chronometer meter){
        using namespace arcxx;
        namespace ranges = std::ranges;

        meter.measure([](){
            std::size_t t = 0; // Optimization prevention
            for([[maybe_unused]]auto i : ranges::views::iota(0,10000)){
                t += Clothes::join<User>().where(
                    User::Name::like("user%") &&
                    User::Height::cmp < 175.0 &&
                    Clothes::Price::between(10'000,50'000) &&
                    Clothes::UpdateAt::between(
                        sys_days(2022y/April/1d) + 00s,
                        sys_days(2022y/April/last) + 23h + 59min + 59s
                    )
                ).order_by<Clothes::Price>().to_sql().length();
            }
            return t;
        });
    };
}