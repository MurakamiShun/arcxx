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
        inline static const auto constraints = { not_null, length(64) };
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
    //id = std::nullopt;
    constexpr auto table_name = id.column_full_name().first;
    std::cout << id.column_full_name().first << std::endl;
    std::cout << Member::column_names()[0] << std::endl;
    std::cout << EnteringLog::MemberID{}.column_full_name().first << std::endl;
    std::cout << id.to_string() << std::endl;
    std::cout << "primary key: " << Member::ID::is_primary_key << std::endl;

    Member member;
    member.id = 123;
    member.name = "nkodice";
    std::cout << member.get_attribute_strings()[1] << std::endl;
    std::cout << member["name"].to_string() << std::endl;

    std::array<Member, 4> members;
    std::cout << Member::insert(members).to_sql() << std::endl;
    std::cout << Member::all().to_sql() << std::endl;
    std::cout << Member::select(Member::ID{}).to_sql() << std::endl;
    std::cout << Member::pluck(Member::ID{}).to_sql() << std::endl;
    std::cout << Member::where(Member::ID{10}, Member::Name::like("nko\\\' OR 1=1;--dice表")).to_sql() << std::endl;
    std::cout << Member::where(Member::ID::in(10,20,30)).to_sql() << std::endl;

    std::cout << Member::table_definition<active_record::sqlite3_adaptor>() << std::endl;
    std::cout << EnteringLog::table_definition<active_record::sqlite3_adaptor>() << std::endl;
    
    /*
    databaseAdaptor adapt = SQLite3("test.sqlite3");
    //databaseAdaptor = Postgresql("postgresql://localhost/mydb");
    MyDatabase database = databaseAdaptor.connect<MyDatabase>();

    Member new_member;
    new_member.id = 10;
    new_member.name = "watanabe taro";

    database.execute(MyDatabase::Member::create(new_member));

    Member finder;
    finder.id = 10;
    auto result = database.execute(MyDatabase::Member::all());
    auto query = MyDatabase::Member::select<MyDatabase::Member::ID, MyDatabase::Member::Name>().where("id < {}", 10).count();
    
    MyDatabase::Member::find(MyDatabase::Member::ID{10});

    MyDatabase::EnteringLog::where<MyDatabase::EnteringLog::Member>(MyDatabase::Member::where().pluck<MyDatabase::Member::Name>()).count();

    MyDatabase::Member::joins<EnteringLog>().select(MyDatabase::Member::ID);

    if(database.errors.size()){
        for(const auto& error : database.errors){
            std::cout  << error << std::endl;
        }
    }
    */
    
    return 0;
}