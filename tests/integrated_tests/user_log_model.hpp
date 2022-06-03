#include "user_model.hpp"

struct UserLog : public arcxx::model<UserLog>{
    constexpr static auto table_name = "user_log_table";

    struct ID : public arcxx::attributes::integer<UserLog, ID, std::size_t> {
        constexpr static auto column_name = "id";
        using integer<UserLog, ID, std::size_t>::integer;
        inline static const auto constraints = primary_key;
    } id;

    struct UserID : public arcxx::attributes::foreign_key<UserLog, UserID, User::ID> {
        constexpr static auto column_name = "user_id";
        using foreign_key<UserLog, UserID, User::ID>::foreign_key;
    } user_id;

    struct Comment : public arcxx::attributes::string<UserLog, Comment> {
        constexpr static auto column_name = "comment";
        using string<UserLog, Comment>::string;
    } comment;
};