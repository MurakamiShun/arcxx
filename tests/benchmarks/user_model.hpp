#include <arcxx.hpp>
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>
#include <ranges>

struct User : public arcxx::model<User> {
    inline static decltype(auto) table_name = "user_table";

    struct ID : public arcxx::attributes::integer<User, ID, std::size_t> {
        inline static decltype(auto) column_name = "id";
        using arcxx::attributes::integer<User, ID, size_t>::integer;

        inline static const auto constraints = primary_key;
    } id;

    struct Name : public arcxx::attributes::string<User, Name> {
        inline static decltype(auto) column_name = "name";
        using arcxx::attributes::string<User, Name>::string;

        inline static const auto constraints = not_null & default_value("unknow");
    } name;

    struct Height : public arcxx::attributes::decimal<User, Height>{
        inline static decltype(auto) column_name = "height";
        using arcxx::attributes::decimal<User, Height>::decimal;
    } height;
};