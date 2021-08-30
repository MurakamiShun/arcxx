#include <active_record.hpp>
#include <iostream>

struct Goods : public active_record::model<Goods> {
    static constexpr auto table_name = "goods_table";
    struct ID : public active_record::attributes::integer<Goods, ID, std::size_t> {
        using integer<Goods, ID, std::size_t>::integer;
        static constexpr auto column_name = "id";
        inline static const auto constraints = { primary_key, not_null };
    } id;

    struct Name : public active_record::attributes::string<Goods, Name> {
        using string<Goods, Name>::string;
        static constexpr auto column_name = "name";
        inline static const auto constraints = { not_null };
    } name;

    struct Price : public active_record::attributes::integer<Goods, Price, uint32_t> {
        using integer<Goods, Price, uint32_t>::integer;
        static constexpr auto column_name = "price";
        inline static const auto constraints = { not_null };
    } price;

    std::tuple<ID&, Name&, Price&> attributes = std::tie(id, name, price);
};

std::optional<active_record::sqlite3::adaptor> setup(auto conn){
    using namespace active_record;
    // Connect and create table
    auto conn = sqlite3::adaptor::open("create_table_example.sqlite3", sqlite3::options::readwrite);
    if (conn.has_error()){
        std::cout << conn.error_message() << std::endl;
        return std::nullopt;
    }

    if(const auto error = conn.template create_table<Goods>(); error){
        std::cout << "Error:" << error.value() << std::endl;
        std::cout << "SQL Statement:\n" << Goods::schema::template to_sql<decltype(conn)>() << std::endl;
        return std::nullopt;
    }

    // Inserting data
    Goods apple = {
        .id = 1,
        .name = "apple",
        .price = 100
    };

    if(const auto error = Goods::insert(apple).exec(conn); error){
        std::cout << "Error:" << error.value() << std::endl;
        return -1;
    }
    else {
        std::cout << "Done!!" << std::endl;
    }
    return conn;
}

int main(){
    using namespace active_record;
    
    auto result = setup();
    if(!result) { return -1; }

    auto& conn = result.value();

    if(const auto [error, goods] = Goods::where(Goods::Price::cmp < 1000).exec(conn); error){
        std::cout << "Error:" << error.value() << std::endl;
        return -1;
    }
    else {
        std::cout << "Found count : " << goods.size() << std::endl;
        for(const auto& g : goods){
            std::cout << g.name.value() << " : " << g.price.value() << std::endl;
        }
        std::cout << "Done!!" << std::endl;
    }
    return conn;

    return 0;
}