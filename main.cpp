#include "include/active_record.hpp"
#include "include/active_record.hpp" // reimplementation testing
#include <string>
#include <iostream>

struct Member : public active_record::model<Member> {
    static constexpr auto table_name = "members_table";
    struct ID : public active_record::attributes::integer<Member, ID> {
        using active_record::attributes::integer<Member, ID>::integer;
        static constexpr auto column_name = "id";
        inline static const auto constraints = { primary_key };
        //constexpr ~ID(){}
    } id;
    struct Name : public active_record::attributes::string<Member, Name> {
        using active_record::attributes::string<Member, Name>::string;
        static constexpr auto column_name = "name";
        inline static const auto constraints = { not_null, length(64), default_value("unknown") };
    } name;

    struct Height : public active_record::attributes::decimal<Member, Height> {
        using active_record::attributes::decimal<Member, Height>::decimal;
        static constexpr auto column_name = "height";
        inline static const auto constraints = { default_value(160.0) };
    } height;
    std::tuple<ID&, Name&, Height&> attributes = std::tie(id, name, height);
};

struct EnteringLog : public active_record::model<EnteringLog> {
    static constexpr auto table_name = "entering_log_table";
    struct ID : public active_record::attributes::integer<EnteringLog, ID> {
        using active_record::attributes::integer<EnteringLog, ID>::integer;
        static constexpr auto column_name = "id";
        inline static const auto constraints = { primary_key };
    } id;
    struct MemberID : public active_record::attributes::foreign_key<EnteringLog, MemberID, Member::ID> {
        using active_record::attributes::foreign_key<EnteringLog, MemberID, Member::ID>::foreign_key;
        static constexpr auto column_name = "member_id";
    } member_id;
    std::tuple<ID&, MemberID&> attributes = std::tie(id, member_id);
};

int main() {
    constexpr Member::ID id = 10;
    Member::Name name = "test";
    Member member;
    member.id = 123;
    member.name = "testuser1";
    member.height = 170.0;

    // PostgreSQL
    namespace pg = active_record::PostgreSQL;
    auto pg_conn = pg::adaptor::open(
        pg::endpoint{.server_name = "postgres", .db_name = "test_db"},
        pg::auth{.user = "postgres", .password = "password"}
    );
    if(pg_conn.has_error()){
        std::cout << "\033[31m" << pg_conn.error_message() << "\033[m" << std::endl;
        return -1;
    }
    else std::cout << "\033[32m done! \033[m" << std::endl;

    std::cout << "    \033[33m[query] \033[m" << Member::schema::to_sql<active_record::PostgreSQL::adaptor>() << std::endl;
    if(const auto error = pg_conn.template create_table<Member>(); error){
        std::cout << "\033[31m" << error.value() << "\033[m" << std::endl;
    }

    std::cout << "\033[33m[execution] \033[m" << Member::insert(member).to_sql<active_record::postgresql_adaptor>() << std::endl;
    if(const auto error = Member::insert(member).exec(pg_conn); error){
        std::cout << "\033[31m" << error.value() << "\033[m" << std::endl;
    }
    else std::cout << "\033[32m done! \033[m" << std::endl;

    std::cout << "\033[33m[execution] \033[m" << Member::all().to_sql<active_record::postgresql_adaptor>() << std::endl;
    
    if(const auto [error, all_members] = Member::all().exec(pg_conn); error){
        std::cout << "\033[31m" << error.value() << "\033[m" << std::endl;
    }
    else {
        std::cout << "\033[32m done! \033[m" << std::endl;
        for(const auto& m : all_members){
            std::cout << "id:" << m.id.to_string() << "\tname:" << m.name.to_string() << "\theight:" << m.height.to_string() << std::endl;
        }
    }

    // SQLite3
    std::array<Member, 4> members;
    members[0].name = "satou";
    members[1].name = "sasaki";
    members[1].name = "watanabe";
    std::cout << "\033[33m[connecting] \033[m" << std::endl;
    auto con = active_record::sqlite3_adaptor::open("test.sqlite3", active_record::sqlite3::options::create);
    if(con.has_error()){
        std::cout << "\033[31m" << con.error_message() << "\033[m" << std::endl;
        return -1;
    }
    else std::cout << "\033[32m done! \033[m" << std::endl;
    
    std::cout << "\033[33m[transaction]" << std::endl;
    std::cout << "    \033[33m[query] \033[m" << Member::schema::to_sql<active_record::sqlite3::adaptor>() << std::endl;
    std::cout << "    \033[33m[query] \033[m" << EnteringLog::schema::to_sql<active_record::sqlite3::adaptor>() << std::endl;
    
    const auto creata_table_transaction = [](auto& connection){
        using namespace active_record;
        using transaction = active_record::transaction;

        if(const auto error = connection.template create_table<Member>(); error){
            std::cout << "\033[31m" << error.value() << "\033[m" << std::endl;
            return transaction::rollback;
        }
        if(const auto error = connection.template create_table<EnteringLog>(); error){
            std::cout << "\033[31m" << error.value() << "\033[m" << std::endl;
            return transaction::rollback;
        }
        return transaction::commit;
    };
    
    if(const auto [error, trans_result] = con.transaction(creata_table_transaction); error){
        std::cout << "\033[31m" << error.value() << "\033[m" << std::endl;
    }
    else if(trans_result == active_record::transaction::rollback){
        std::cout << "\033[31m" << "rollbacked!!" << "\033[m" << std::endl;
    }
    else std::cout << "\033[32m done! \033[m" << std::endl;
    
    std::cout << "\033[33m[execution] \033[m" << Member::insert(member).to_sql<active_record::sqlite3_adaptor>() << std::endl;
    if(const auto error = Member::insert(member).exec(con); error){
        std::cout << "\033[31m" << con.error_message() << "\033[m" << std::endl;
    }
    else std::cout << "\033[32m done! \033[m" << std::endl;

    // std::cout << "\033[33m[execution] \033[m" << Member::insert(members).to_sql() << std::endl;
    // if(const auto error = con.exec(Member::insert(members)); error){
    //     std::cout << "\033[31m" << con.error_message() << "\033[m" << std::endl;
    // }
    // else std::cout << "\033[32m done! \033[m" << std::endl;

    std::cout << "\033[33m[execution] \033[m" << Member::all().to_sql<active_record::sqlite3_adaptor>() << std::endl;
    
    if(const auto [error, all_members] = Member::all().exec(con); error){
        std::cout << "\033[31m" << error.value() << "\033[m" << std::endl;
    }
    else {
        std::cout << "\033[32m done! \033[m" << std::endl;
        for(const auto& m : all_members){
            std::cout << "id:" << m.id.to_string() << "\tname:" << m.name.to_string() << std::endl;
        }
    }

    std::cout << "\033[33m[execution] \033[m" << Member::where(Member::ID{123}).to_sql<active_record::sqlite3_adaptor>() << std::endl;
    
    if(const auto [error, find_members] = Member::where(Member::ID{123}).exec(con); error){
        std::cout << "\033[31m" << error.value() << "\033[m" << std::endl;
    }
    else {
        std::cout << "\033[32m done! \033[m" << std::endl;
        for(const auto& m : find_members){
            std::cout << "id:" << m.id.to_string() << "\tname:" << m.name.to_string() << std::endl;
        }
    }

    std::cout << "\033[33m[execution] \033[m" << Member::limit(2).to_sql<active_record::sqlite3_adaptor>() << std::endl;
    
    if(const auto [error, find_members] = Member::limit(2).exec(con); error){
        std::cout << "\033[31m" << error.value() << "\033[m" << std::endl;
    }
    else {
        std::cout << "\033[32m done! \033[m" << std::endl;
        for(const auto& m : find_members){
            std::cout << "id:" << m.id.to_string() << "\tname:" << m.name.to_string() << std::endl;
        }
    }

    std::cout << "\033[33m[execution] \033[m" << Member::order_by<Member::ID>(active_record::order::desc).to_sql<active_record::sqlite3_adaptor>() << std::endl;
    
    if(const auto [error, find_members] = Member::order_by<Member::ID>(active_record::order::desc).exec(con); error){
        std::cout << "\033[31m" << error.value() << "\033[m" << std::endl;
    }
    else {
        std::cout << "\033[32m done! \033[m" << std::endl;
        for(const auto& m : find_members){
            std::cout << "id:" << m.id.to_string() << "\tname:" << m.name.to_string() << std::endl;
        }
    }

    EnteringLog log;
    log.member_id = member.id;

    std::cout << "\033[33m[execution] \033[m" << EnteringLog::join<Member>().select<EnteringLog::ID, Member::Name>().to_sql<active_record::sqlite3_adaptor>() << std::endl;
    if(const auto [error, logs] = EnteringLog::join<Member>().select<EnteringLog::ID, Member::Name>().exec(con); error){
        std::cout << "\033[31m" << error.value() << "\033[m" << std::endl;
    }
    else {
        std::cout << "\033[32m done! \033[m" << std::endl;
        for(const auto& log : logs){
            std::cout << "id:" << std::get<0>(log).to_string() << "\tname:" << std::get<1>(log).to_string() << std::endl;
        }
    }
    EnteringLog::insert(log).exec(con);


    std::cout << "\033[33m[execution] \033[m" << Member::where(Member::ID{123}).count().to_sql<active_record::sqlite3_adaptor>() << std::endl;
    
    if(const auto [error, find_members] = Member::where(Member::ID{128} || Member::ID{123}).exec(con); error){
        std::cout << "\033[31m" << error.value() << "\033[m" << std::endl;
    }
    else {
        std::cout << "\033[32m done! \033[m" << std::endl;
        for(const auto& m : find_members){
            std::cout << "id:" << m.id.to_string() << "\tname:" << m.name.to_string() << std::endl;
        }
    }

    con.close();
    return 0;
}