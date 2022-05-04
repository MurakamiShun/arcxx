#pragma once
/*
 * Active Record C++: https://github.com/akisute514/active_record_cpp
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */
#include "query.hpp"

namespace active_record {
    template<typename Derived>
    struct model {
        struct schema {
            template<std::derived_from<adaptor> Adaptor>
            [[nodiscard]] static active_record::string to_sql(decltype(abort_if_exists));
            template<std::derived_from<adaptor> Adaptor>
            [[nodiscard]] static active_record::string to_sql();
        };

        static constexpr bool has_table_name() noexcept { return requires {Derived::table_name;}; }

        [[nodiscard]] static constexpr auto column_names() noexcept {
            return std::apply(
                []<is_attribute... Attrs>([[maybe_unused]]Attrs...) constexpr noexcept { return std::array<const active_record::string_view, sizeof...(Attrs)>{(Attrs::column_name)...}; },
                Derived{}.attributes_as_tuple()
            );
        }

        [[nodiscard]] auto attributes_as_tuple() noexcept {
            using namespace tuptup::type_placeholders;
            auto attributes_tuple = tuptup::struct_binder<Derived>{}(*reinterpret_cast<Derived*>(this));
            return tuptup::tuple_filter<is_attribute_type<defer<std::remove_reference<_1>>>>(attributes_tuple);
        }
        [[nodiscard]] auto attributes_as_tuple() const noexcept {
            using namespace tuptup::type_placeholders;
            const auto attributes_tuple = tuptup::struct_binder<Derived>{}(*reinterpret_cast<const Derived*>(this));
            return tuptup::tuple_filter<is_attribute_type<defer<std::remove_reference<_1>>>>(attributes_tuple);
        }
        template<std::derived_from<std::function<void()>> T>
        void exec_callbacks(){
            using namespace tuptup::type_placeholders;
            const auto attributes_tuple = tuptup::struct_binder<Derived>{}(*reinterpret_cast<const Derived*>(this));
            tuptup::apply_each(
                [](const auto& callback){ callback(); },
                tuptup::tuple_filter<std::is_same<T, defer<std::remove_reference<_1>>>>(attributes_tuple)
            );
        }

        struct before_create  : std::function<void()>{ using function<void()>::function; };
        struct after_create   : std::function<void()>{ using function<void()>::function; };
        struct before_update  : std::function<void()>{ using function<void()>::function; };
        struct after_update   : std::function<void()>{ using function<void()>::function; };
        struct before_save    : std::function<void()>{ using function<void()>::function; };
        struct after_save     : std::function<void()>{ using function<void()>::function; };
        struct before_destroy : std::function<void()>{ using function<void()>::function; };
        struct after_destroy  : std::function<void()>{ using function<void()>::function; };

        [[nodiscard]] static auto insert(const Derived& model);
        [[nodiscard]] static auto insert(Derived&& model);

        [[nodiscard]] static auto all();

        template<is_attribute... Attrs>
        [[nodiscard]] static auto select();

        template<is_attribute_aggregator... Aggregators>
        [[nodiscard]] static auto select();

        template<is_attribute Attr>
        [[nodiscard]] static auto pluck();
        template<is_attribute_aggregator Aggregator>
        [[nodiscard]] static auto pluck();

        // delete is identifier word
        template<is_attribute Attr>
        [[nodiscard]] static auto destroy(const Attr&&);
        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] static auto destroy(query_condition<SrcBindAttrs>&&);

        template<is_attribute Attr>
        [[nodiscard]] static auto where(const Attr&);
        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] static auto where(query_condition<SrcBindAttrs>&&);

        [[nodiscard]] static auto limit(const std::size_t);

        template<is_attribute Attr>
        [[nodiscard]] static auto order_by(const active_record::order = active_record::order::asc);

        template<typename ReferModel>
        requires std::derived_from<ReferModel, model<ReferModel>>
        [[nodiscard]] static auto join();

        template<typename ReferModel>
        requires std::derived_from<ReferModel, model<ReferModel>>
        [[nodiscard]] static auto left_join();

        template<is_attribute Attr>
        [[nodiscard]] static auto group_by();

        [[nodiscard]] static auto count();

        template<is_attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] static auto sum();

        template<is_attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] static auto avg();

        template<is_attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] static auto max();

        template<is_attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] static auto min();
    };

    template<typename T>
    concept is_model = requires {
        std::derived_from<T, model<T>>;
        requires T::has_table_name();
    };
}