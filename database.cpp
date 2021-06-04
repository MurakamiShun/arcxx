#include <active_record.hpp>

struct MyDatabase : public active_record::database {
    struct Member : public database::table<Member> {
        static auto table_name = "members_table";
        struct ID : public column::integer<Member, ID> {
            static auto column_name = "id";
            static auto validators = { allow_null, uniqueness };
        } id;
        struct Name : public column::string<Member, Name> {
            static auto column_name = "name";
        } name;
        using columns = std::tuple<ID, Name>;
    };

    struct EnteringLog : public database::table {
        struct Mebmer : public relation::belongs_to<Member> {
            
        } member;
    };
};

int main(){
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

    
    return 0;
}