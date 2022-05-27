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
};

auto setup() -> active_record::expected<active_record::sqlite3::connector, active_record::string>{
    using namespace active_record;
    // Connect and create table
    auto conn = sqlite3::connector::open("insert_data_example.sqlite3", sqlite3::options::create);
    if (conn.has_error()) return make_unexpected(conn.error_message());

    if(const auto result = conn.template create_table<Goods>(); result){
        return conn;
    }
    else{
        return make_unexpected(result.error());
    }
}

int main(){
    using namespace active_record;
    
    auto setup_result = setup();
    if(!setup_result) {
        std::cout << "Setup failed" << std::endl;
        std::cout << "Error message:" << setup_result.error() << std::endl;
        return -1;
    }

    auto conn = std::move(setup_result.value());

    // Inserting data
    Goods apple = {
        .id = 1,
        .name = "apple",
        .price = 100
    };

    const auto insert_stmt = Goods::insert(apple);

    std::cout << "SQL Statement:\n" << insert_stmt.to_sql<decltype(conn)>() << std::endl;
    if(const auto result = insert_stmt.exec(conn); !result){
        std::cout << "Error message:" << result.error() << std::endl;
        return -1;
    }

    std::cout << "Done!!" << std::endl;

    return 0;
}