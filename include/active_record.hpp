#pragma once
//#if __cplusplus < 202002L
//#error C++20 is required.
//#else

#include "active_record/model.hpp"
#include "active_record/attributes/attributes.hpp"
#include "active_record/query_impl/query_relation_impl.hpp"
#include "active_record/model_impl/queries.hpp"
#include "active_record/model_impl/model_impl.hpp"
#include "active_record/adaptor.hpp"

#if __has_include(<sqlite3.h>)
#include "active_record/adaptors/sqlite3_adaptor.hpp"
#endif

#if __has_include(<postgresql/libpq-fe.h>)
#include "active_record/adaptors/postgresql_adaptor.hpp"
#endif
//#endif