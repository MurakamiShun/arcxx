#include "../include/active_record.hpp"
#include <catch2/catch.hpp>

struct Test : public active_record::model<Test> {
    static constexpr auto table_name = "test_table";
    std::tuple<> attributes;
}; 

TEST_CASE("Model definition test", "[model]") {
    
}