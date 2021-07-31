#pragma once
#include "../include/active_record.hpp"

struct User : public active_record::model<User> {
    static constexpr auto table_name = "user_table";

    struct ID : public active_record::attributes::integer<User, ID, std::size_t> {
        static constexpr auto column_name = "id";
        using active_record::attributes::integer<User, ID, size_t>::integer;

        inline static const auto constraints = { primary_key };
        constexpr ~ID(){}
    } id;

    struct Name : public active_record::attributes::string<User, Name> {
        static constexpr auto column_name = "name";
        using active_record::attributes::string<User, Name>::string;

        inline static const auto constraints = { not_null, default_value("unknow") };
        constexpr ~Name(){}
    } name;

    std::tuple<ID&, Name&> attributes = std::tie(id, name);
};