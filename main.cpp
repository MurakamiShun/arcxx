#include "include/active_record.hpp"
#include <string>
#include <iostream>
#include <cmath>

struct EnteringLog;

struct Member : public active_record::model<Member> {
    static constexpr auto table_name = "members_table";
    struct ID : public active_record::attributes::integer<Member, ID> {
        using active_record::attributes::integer<Member, ID>::integer;
        static constexpr auto column_name = "id";
        inline static const auto constraints = { primary_key };
        constexpr ~ID(){}
    } id;
    struct Name : public active_record::attributes::string<Member, Name> {
        using active_record::attributes::string<Member, Name>::string;
        static constexpr auto column_name = "name";
        inline static const auto constraints = { length(8), default_value("unknown") };
    } name;
    std::tuple<ID&, Name&> attributes = std::tie(id, name);
};

struct EnteringLog : public active_record::model<EnteringLog> {
    static constexpr auto table_name = "entering_log_table";
    struct ID : public active_record::attributes::integer<EnteringLog, ID> {
        static constexpr auto column_name = "id";
        inline static const auto constraints = { primary_key };
    } id;
    struct MemberID : public active_record::relation::reference_to<Member::ID, EnteringLog, MemberID> {
        static constexpr auto column_name = "member_id";
    } member_id;
    std::tuple<ID&, MemberID&> attributes = std::tie(id, member_id);
};

int main() {
    constexpr Member::ID id = 10;
    Member::Name name = "test";
    Member member;
    member.id = 123;
    member.name = "nkodice";

    std::array<Member, 4> members;
    members[0].name = "satou";
    members[1].name = "sasaki";
    members[1].name = "watanabe aaaaaaa";
    std::cout << "\033[33m[connecting] \033[m" << std::endl;
    auto connection = active_record::sqlite3_adaptor::open("test.sqlite3", active_record::sqlite3::options::create);
    if(connection.has_error()){
        std::cout << "\033[31m" << connection.error_message() << "\033[m" << std::endl;
    }
    else std::cout << "\033[32m done! \033[m" << std::endl;

    std::cout << "\033[33m[execution] \033[m" << Member::table_definition<active_record::sqlite3_adaptor>().to_sql() << std::endl;
    connection.exec(Member::table_definition<active_record::sqlite3_adaptor>());
    if(connection.has_error()){
        std::cout << "\033[31m" << connection.error_message() << "\033[m" << std::endl;
    }
    else std::cout << "\033[32m done! \033[m" << std::endl;

    std::cout << "\033[33m[execution] \033[m" << EnteringLog::table_definition<active_record::sqlite3_adaptor>().to_sql() << std::endl;
    connection.exec(EnteringLog::table_definition<active_record::sqlite3_adaptor>());
    if(connection.has_error()){
        std::cout << "\033[31m" << connection.error_message() << "\033[m" << std::endl;
    }
    else std::cout << "\033[32m done! \033[m" << std::endl;

    std::cout << "\033[33m[execution] \033[m" << Member::insert(member).to_sql() << std::endl;
    if(connection.exec(Member::insert(member))){
        std::cout << "\033[31m" << connection.error_message() << "\033[m" << std::endl;
    }
    else std::cout << "\033[32m done! \033[m" << std::endl;

    std::cout << "\033[33m[execution] \033[m" << Member::insert(members).to_sql() << std::endl;
    if(connection.exec(Member::insert(members))){
        std::cout << "\033[31m" << connection.error_message() << "\033[m" << std::endl;
    }
    else std::cout << "\033[32m done! \033[m" << std::endl;

    std::cout << "\033[33m[execution] \033[m" << Member::all().to_sql() << std::endl;
    const auto all_members = connection.exec(Member::all());
    if(connection.has_error()){
        std::cout << "\033[31m" << connection.error_message() << "\033[m" << std::endl;
    }
    else {
        std::cout << "\033[32m done! \033[m" << std::endl;
        for(const auto& m : all_members){
            std::cout << "id:" << m.id.to_string() << "\tname:" << m.name.to_string() << std::endl;
        }
    }

    connection.close();

    return 0;
}