#include "user_model.hpp"

struct UserLog : public active_record::model<UserLog>{
    constexpr static auto table_name = "user_log_table";

    struct ID : public active_record::attributes::integer<UserLog, ID, std::size_t> {
        constexpr static auto column_name = "id";
        using integer<UserLog, ID, std::size_t>::integer;
        inline static const auto constraints = { primary_key };
    } id;

    struct UserID : public active_record::attributes::foreign_key<UserLog, UserID, User::ID> {
        constexpr static auto column_name = "user_id";
        using foreign_key<UserLog, UserID, User::ID>::foreign_key;
    } user_id;

    struct Comment : public active_record::attributes::string<UserLog, Comment> {
        constexpr static auto column_name = "comment";
        using string<UserLog, Comment>::string;
    } comment;

    std::tuple<ID&, UserID&, Comment&> attributes = std::tie(id, user_id, comment);
};