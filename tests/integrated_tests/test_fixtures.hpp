#pragma once
#include <active_record.hpp>
#include <filesystem>
#include <catch2/catch_all.hpp>

#ifdef POSTGRESQL_TEST
using connector = active_record::PostgreSQL::connector;
inline connector open_testfile(){
    auto adpt = active_record::postgresql_connector::open(
        active_record::PostgreSQL::endpoint{.server_name = "localhost", .db_name = "test_db"},
        active_record::PostgreSQL::auth{.user = "postgres", .password = "root"}
    );
    if(adpt.has_error()) FAIL(adpt.error_message());
    return adpt;
}
inline void close_testfile(connector& adpt){
    adpt.close();
}
#else
using connector = active_record::sqlite3::connector;
inline connector open_testfile(){
    auto adpt = active_record::sqlite3_connector::open("user_model_test.sqlite3", active_record::sqlite3::options::create);
    if(adpt.has_error()) FAIL(adpt.error_message());
    return adpt;
}
inline void close_testfile(connector& adpt){
    adpt.close();
    std::filesystem::remove("user_model_test.sqlite3");
}
#endif