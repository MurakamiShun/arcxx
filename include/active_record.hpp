#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT Lisence.
 */
//#if __cplusplus < 202002L
//#error C++20 is required.
//#else

#include "active_record/model.hpp"
#include "active_record/attributes/attributes.hpp"
#include "active_record/query_impl/query_relation.hpp"
#include "active_record/query_impl/query_condition.hpp"
#include "active_record/model_impl/queries.hpp"

#if __has_include(<sqlite3.h>)
#include "active_record/adaptors/sqlite3_adaptor.hpp"
#endif

#if __has_include(<libpq-fe.h>)
#include "active_record/adaptors/postgresql_adaptor.hpp"
#endif
//#endif