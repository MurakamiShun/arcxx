#pragma once
#include "../include/active_record.hpp"
#include <filesystem>

#ifdef POSTGRESQL_TEST
using adaptor = active_record::PostgreSQL::adaptor;
inline adaptor open_testfile(){
    return active_record::postgresql_adaptor::open(
        active_record::PostgreSQL::endpoint{.server_name = "postgresql", .db_name = "test_db"},
        active_record::PostgreSQL::auth{.user = "postgres", .password = "password"}
    );
}
inline void close_testfile(adaptor& adpt){
    adpt.close();
}
#else
using adaptor = active_record::sqlite3::adaptor;
inline adaptor open_testfile(){
    return active_record::sqlite3_adaptor::open("user_model_test.sqlite3", active_record::sqlite3::options::create);
}
inline void close_testfile(adaptor& adpt){
    adpt.close();
    std::filesystem::remove("user_model_test.sqlite3");
}
#endif