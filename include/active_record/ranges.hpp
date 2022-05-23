#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include <ranges>
#include "adaptor.hpp"

namespace active_record::ranges{

    template<typename Derived>
    struct query_range_view_interface : public std::ranges::view_interface<Derived>{
        using derived_type = Derived;

        static_assert(requires(Derived d){ d.make_query_relation(); });
        static_assert(requires{ Derived::result_type; });
        static_assert(requires(Derived d){ d.db_adaptor(); });
    };

    template<typename T>
    concept query_range_view = std::derived_from<T, query_range_view_interface<typename T::derived_type>>;

    template<std::derived_from<adaptor> Adaptor>
    struct db_adaptor_view : query_range_view_interface<Adaptor>{
                using result_type = std::tuple<SelectedColumns...>;
        ViewSource source = ViewSource{};

        constexpr select_view() = delete;
        constexpr select_view(ViewSource&& vs) : source(std::move(vs)){};
        constexpr select_view(const ViewSource& vs) : source(vs){};

        auto make_query_relation() const {
            return source.make_query_relation().select<SelectedColumns...>();
        }

        decltype(auto) db_adaptor() const {
            return source.db_adaptor();
        }
    };
}

#include "ranges/select.ipp"