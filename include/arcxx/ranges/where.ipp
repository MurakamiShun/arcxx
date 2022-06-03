#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */

namespace arcxx::ranges{
    template<query_range_view ViewSource>
    struct where_view : public query_range_view_interface<where_view<ViewSource>>{
    };

    namespace views{
        namespace detail{

        }
    }
}