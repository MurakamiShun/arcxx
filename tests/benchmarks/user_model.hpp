#include "../../include/active_record.hpp"
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>
#include <ranges>

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

    //std::tuple<ID&, Name&, Height&> attributes = std::tie(id, name, height);
};