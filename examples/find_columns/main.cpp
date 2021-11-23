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

std::optional<active_record::sqlite3::adaptor> setup(){
    using namespace active_record;
    // Connect and create table
    auto conn = sqlite3::adaptor::open("find_column_example.sqlite3", sqlite3::options::create);
    if (conn.has_error()){
        std::cout << conn.error_message() << std::endl;
        return std::nullopt;
    }

    if(const auto error = conn.template create_table<Goods>(); error){
        std::cout << "Error:" << error.value() << std::endl;
        std::cout << "SQL Statement:\n" << Goods::schema::template to_sql<decltype(conn)>() << std::endl;
        return std::nullopt;
    }

    const auto insert_transaction = [](auto& conn){
        using transaction = active_record::transaction;
        // Inserting data
        std::array<Goods, 4> goods = {
            Goods{.id = 1, .name = "apple",.price = 100},
            Goods{.id = 2, .name = "note", .price = 300},
            Goods{.id = 3, .name = "gold", .price = 100000},
            Goods{.id = 4, .name = "pen",  .price = 5},
        };
        for(const auto& good : goods){
            if(const auto error = Goods::insert(good).exec(conn); error){
                return transaction::rollback;
            }
        }
        return transaction::commit;
    };
    if(const auto [error, transaction_result] = conn.transaction(insert_transaction); error){
        std::cout << "Error:" << error.value() << std::endl;
        return std::nullopt;
    }
    else if(transaction_result == active_record::transaction::rollback){
        std::cout << "Insertion is rollbacked!!" << error.value() << std::endl;
        return std::nullopt;
    }
    std::cout << "Setting up is done!!" << std::endl;
    return conn;
}

int main(){
    using namespace active_record;
    
    auto result = setup();
    if(!result) { return -1; }

    auto conn = std::move(result.value());

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

    return 0;
}