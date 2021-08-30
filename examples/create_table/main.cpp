#include <active_record.hpp>
#include <iostream>

struct Goods : public active_record::model<Goods> {
    static constexpr auto table_name = "goods_table";
    struct ID : public active_record::attributes::integer<Goods, ID, std::size_t> {
        using integer<Goods, ID, std::size_t>::integer;
        static constexpr auto column_name = "id";
        static const auto constraints = { primary_key, not_null };
    } id;

    struct Name : public active_record::attributes::string<Goods, Name> {
        using string<Goods, Name>::string;
        static constexpr auto column_name = "name";
        static const auto constraints = { not_null };
    } name;

    struct Price : public active_record::attributes::integer<Goods, Price, uint32_t> {
        using integer<Goods, Price, uint32_t>::integer;
        static constexpr auto column_name = "price";
        static const auto constraints = { not_null };
    } price;

    std::tuple<ID&, Name&, Price&> attributes = std::tie(id, name, price);
};

int main(){
    using namespace active_record;
    auto connector = sqlite3::adaptor::open("create_table_example.sqlite3", sqlite3::options::create);
    if (connector.has_error()){
        std::cout << connector.error_message() << std::endl;
        return -1;
    }

    std::cout << "SQL Statement:\n" << Goods::schema::to_sql<decltype(connector)>() << std::endl;
    if(const auto error = connector.create_table<Goods>(); error){
        std::cout << "Error:" << error.value() << std::endl;
        return -1;
    }
    else {
        std::cout << "Done!!" << std::endl;
    }

    return 0;
}