#pragma once
//#if __cplusplus < 202002L
//#error C++20 is required.
//#else

#include "active_record/database.hpp"
#include "active_record/model.hpp"
#include "active_record/attributes/attributes.hpp"
#include "active_record/query_impl/query_impl.hpp"
#include "active_record/query_impl/model_queries.hpp"
#include "active_record/relation.hpp"

#include "active_record/adaptors/sqlite3_adaptor.hpp"

//#endif