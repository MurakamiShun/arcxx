#include <arcxx.hpp>
#include <iostream>

struct Goods : public arcxx::model<Goods> {
    static constexpr auto table_name = "goods_table";
    struct ID : public arcxx::attributes::integer<Goods, ID, std::size_t> {
        using integer<Goods, ID, std::size_t>::integer;
        static constexpr auto column_name = "id";
        inline static const auto constraints = primary_key & not_null;
    } id;

    struct Name : public arcxx::attributes::string<Goods, Name> {
        using string<Goods, Name>::string;
        static constexpr auto column_name = "name";
        inline static const auto constraints = not_null;
    } name;

    struct Price : public arcxx::attributes::integer<Goods, Price, uint32_t> {
        using integer<Goods, Price, uint32_t>::integer;
        static constexpr auto column_name = "price";
        inline static const auto constraints = not_null;
    } price;
};

auto setup() -> arcxx::expected<arcxx::sqlite3::connector, arcxx::string>{
    using namespace arcxx;
    // Connect and create table
    auto conn = sqlite3::connector::open("find_column_example.sqlite3", sqlite3::options::create);
    if (conn.has_error()) return make_unexpected(conn.error_message());

    if(const auto result = conn.template create_table<Goods>(); !result){
        return make_unexpected(result.error());
    }

    const auto insert_transaction = [](auto& conn){
        namespace transaction = arcxx::transaction;
        // Inserting data
        std::array<Goods, 4> goods = {
            Goods{.id = 1, .name = "apple",.price = 100},
            Goods{.id = 2, .name = "note", .price = 300},
            Goods{.id = 3, .name = "gold", .price = 100000},
            Goods{.id = 4, .name = "pen",  .price = 5},
        };
        for(const auto& good : goods){
            if(const auto insert_result = Goods::insert(good).exec(conn); !insert_result){
                return transaction::rollback(insert_result.error());
            }
        }
        return transaction::commit;
    };
    if(const auto transaction_result = conn.transaction(insert_transaction); !transaction_result){
        return make_unexpected(transaction_result.error());
    }
    std::cout << "Setting up is done!!" << std::endl;
    return conn;
}

int main(){
    using namespace arcxx;
    
    auto setup_result = setup();
    if(!setup_result) {
        std::cout << "Setup failed:" << setup_result.error() << std::endl;
        return -1;
    }

    auto conn = std::move(setup_result.value());

    const auto find_goods_stmt = Goods::where(Goods::Price::cmp < 1000);
    std::cout << "SQL statement:" << find_goods_stmt.to_sql<decltype(conn)>() << std::endl;
    
    if(const auto find_result = conn.exec(find_goods_stmt); !find_result){
        std::cout << "Error message:" << find_result.error() << std::endl;
        return -1;
    }
    else {
        auto& goods = find_result.value();
        std::cout << "Found count:" << goods.size() << std::endl;
        std::cout << "\"name\",\"price\"" << std::endl;
        for(const auto& g : goods){
            std::cout << g.name.value() << " , " << g.price.value() << std::endl;
        }
    }

    std::cout << "Done!!" << std::endl;
    return 0;
}