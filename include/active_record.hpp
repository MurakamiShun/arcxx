#pragma once
/*
 * Copyright 2021 akisute514
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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