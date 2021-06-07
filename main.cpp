#include "include/active_record.hpp"
#include <string>
#include <iostream>
#include <cmath>

struct Member : public active_record::model<Member> {
    static constexpr auto table_name = "members_table";
    struct ID : public active_record::attributes::integer<Member, ID> {
        static constexpr auto column_name = "id";
        //static constepxr auto validators = { allow_null, uniqueness };
    } id;
    struct Name : public active_record::attributes::string<Member, Name> {
        static constexpr auto column_name = "name";
    } name;
    std::tuple<ID&, Name&> attributes = std::tie(id, name);
};

struct EnteringLog : public active_record::model<EnteringLog> {
    static constexpr auto table_name = "entering_log_table";
    struct ID : public active_record::attributes::integer<Member, ID> {
        static constexpr auto column_name = "id";
        //static constepxr auto validators = { allow_null, uniqueness };
    } id;
    std::tuple<ID&> attributes = std::tie(id);
};

int main(){
    Member::ID id{ 10 };
    id.value() = 10;
    constexpr auto table_name = id.column_full_name().first;
    std::cout << id.column_full_name().first << std::endl;
    std::cout << Member::column_full_names() << std::endl;

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

    MyDatabase::EnteringLog::where<MyDatabase::EnteringLog::Member>(MyDatabase::Member::where("id = {}", {3, 4}).pluck<MyDatabase::Member::Name>()).count();

    MyDatabase::Member::joins<EnteringLog>().select(MyDatabase::Member::ID);

    if(database.errors.size()){
        for(const auto& error : database.errors){
            std::cout  << error << std::endl;
        }
    }
    */
    
    return 0;
}