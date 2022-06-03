#pragma once
#include <arcxx.hpp>
#include <filesystem>
#include <catch2/catch_all.hpp>

#ifdef POSTGRESQL_TEST
using connector = arcxx::PostgreSQL::connector;
inline connector open_testfile(){
    auto adpt = arcxx::postgresql_connector::open(
        arcxx::PostgreSQL::endpoint{.server_name = "postgres", .db_name = "test_db"},
        arcxx::PostgreSQL::auth{.user = "postgres", .password = "root"}
    );
    if(adpt.has_error()) FAIL(adpt.error_message());
    return adpt;
}
inline void close_testfile(connector& adpt){
    adpt.close();
}
#else
using connector = arcxx::sqlite3::connector;
inline connector open_testfile(){
    auto adpt = arcxx::sqlite3_connector::open("user_model_test.sqlite3", arcxx::sqlite3::options::create);
    if(adpt.has_error()) FAIL(adpt.error_message());
    return adpt;
}
inline void close_testfile(connector& adpt){
    adpt.close();
    std::filesystem::remove("user_model_test.sqlite3");
}
#endif