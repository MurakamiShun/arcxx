#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
//#if __cplusplus < 202002L
//#error C++20 is required.
//#else

#include "arcxx/attributes/attributes.hpp"
#include "arcxx/query_impl/query_relation.hpp"

#if __has_include(<sqlite3.h>)
#include "arcxx/connectors/sqlite3_connector.hpp"
#endif

#if __has_include(<libpq-fe.h>)
#include "arcxx/connectors/postgresql_connector.hpp"
#endif
//#endif