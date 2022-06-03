#include <arcxx.hpp>
#include <iostream>

struct Goods : public arcxx::model<Goods> {
    static constexpr auto table_name = "goods_table";
    struct ID : public arcxx::attributes::integer<Goods, ID, std::size_t> {
        using integer<Goods, ID, std::size_t>::integer;
        static constexpr auto column_name = "id";
        static inline const auto constraints = { primary_key, not_null };
    } id;

    struct Name : public arcxx::attributes::string<Goods, Name> {
        using string<Goods, Name>::string;
        static constexpr auto column_name = "name";
        static inline const auto constraints = { not_null };
    } name;

    struct Price : public arcxx::attributes::integer<Goods, Price, uint32_t> {
        using integer<Goods, Price, uint32_t>::integer;
        static constexpr auto column_name = "price";
        static inline const auto constraints = { not_null };
    } price;
};

int main(){
    using namespace arcxx;

    auto connector = sqlite3::connector::open("create_table_example.sqlite3", sqlite3::options::create);
    if (connector.has_error()){
        std::cout << "Error message:" << connector.error_message() << std::endl;
        return -1;
    }

    std::cout << "SQL Statement:\n" << Goods::schema::to_sql<decltype(connector)>() << std::endl;
    if(const auto result = connector.create_table<Goods>(); !result){
        std::cout << "Create table failed:" << result.error() << std::endl;
        return -1;
    }
    
    std::cout << "Done!!" << std::endl;
    return 0;
}