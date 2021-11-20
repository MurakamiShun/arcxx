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

namespace active_record {
    template<typename Result, specialized_from<std::tuple> BindAttrs>
    struct query_relation : public query_relation_common<BindAttrs> {
        using query_relation_common<BindAttrs>::query_relation_common;
        template<std::derived_from<adaptor> Adaptor>
        [[nodiscard]] auto exec(Adaptor& adapt) const {
            return adapt.exec(*this);
        }

        /* scalar */
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) &&;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) const&;

        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) &&;
        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const&;

        /* vector */
        template<Attribute... Attrs>
        [[nodiscard]] query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> select() && requires specialized_from<Result, std::vector>;
        template<Attribute... Attrs>
        [[nodiscard]] query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> select() const & requires specialized_from<Result, std::vector>;

        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::tuple<typename Attrs::attribute_type...>, BindAttrs> select() && requires specialized_from<Result, std::vector>;
        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::tuple<typename Attrs::attribute_type...>, BindAttrs> select() const& requires specialized_from<Result, std::vector>;

        template<Attribute Attr>
        [[nodiscard]] query_relation<std::vector<Attr>, BindAttrs> pluck() && requires specialized_from<Result, std::vector>;
        template<Attribute Attr>
        [[nodiscard]] query_relation<std::vector<Attr>, BindAttrs> pluck() const& requires specialized_from<Result, std::vector>;

        template<Attribute... Attrs>
        requires Model<typename Result::value_type>
        [[nodiscard]] query_relation<bool, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attrs...>>> update(const Attrs&...) requires specialized_from<Result, std::vector>;

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) && requires specialized_from<Result, std::vector>;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) const& requires specialized_from<Result, std::vector>;

        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) && requires specialized_from<Result, std::vector>;
        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const& requires specialized_from<Result, std::vector>;

        [[nodiscard]] query_relation<Result, BindAttrs>& limit(const std::size_t) && requires specialized_from<Result, std::vector>;
        [[nodiscard]] query_relation<Result, BindAttrs> limit(const std::size_t) const& requires specialized_from<Result, std::vector>;

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs>& order_by(const active_record::order = active_record::order::asc) && requires specialized_from<Result, std::vector>;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs> order_by(const active_record::order = active_record::order::asc) const& requires specialized_from<Result, std::vector>;

        [[nodiscard]] query_relation<std::size_t, BindAttrs> count() && requires specialized_from<Result, std::vector>;
        [[nodiscard]] query_relation<std::size_t, BindAttrs> count() const& requires specialized_from<Result, std::vector>;

        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<typename Attr::sum::attribute_type, BindAttrs> sum() && requires specialized_from<Result, std::vector>;
        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<typename Attr::sum::attribute_type, BindAttrs> sum() const& requires specialized_from<Result, std::vector>;

        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<typename Attr::avg::attribute_type, BindAttrs> avg() && requires specialized_from<Result, std::vector>;
        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<typename Attr::avg::attribute_type, BindAttrs> avg() const& requires specialized_from<Result, std::vector>;

        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<typename Attr::max::attribute_type, BindAttrs> max() && requires specialized_from<Result, std::vector>;
        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<typename Attr::max::attribute_type, BindAttrs> max() const& requires specialized_from<Result, std::vector>;

        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<typename Attr::min::attribute_type, BindAttrs> min() && requires specialized_from<Result, std::vector>;
        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<typename Attr::min::attribute_type, BindAttrs> min() const& requires specialized_from<Result, std::vector>;

        /* map */
        template<typename T>
        struct group_type_impl{ using type = int; };
        template<specialized_from<std::unordered_map> T>
        struct group_type_impl<T>{ using type = typename T::key_type; };

        template<typename T>
        struct mapped_type_impl{ using type = std::tuple<int>; };
        template<specialized_from<std::unordered_map> T>
        struct mapped_type_impl<T>{ using type = typename T::mapped_type; };

        using group_type = group_type_impl<Result>::type;
        using mapped_type = mapped_type_impl<Result>::type;

        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::unordered_map<group_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> select() && requires specialized_from<Result, std::unordered_map>;
        template<AttributeAggregator... Attrs>
        [[nodiscard]] query_relation<std::unordered_map<group_type, std::tuple<typename Attrs::attribute_type...>>, BindAttrs> select() const& requires specialized_from<Result, std::unordered_map>;

        template<AttributeAggregator Attr>
        [[nodiscard]] query_relation<std::unordered_map<group_type, typename Attr::attribute_type>, BindAttrs> pluck() && requires specialized_from<Result, std::unordered_map>;
        template<AttributeAggregator Attr>
        [[nodiscard]] query_relation<std::unordered_map<group_type, typename Attr::attribute_type>, BindAttrs> pluck() const& requires specialized_from<Result, std::unordered_map>;

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) && requires specialized_from<Result, std::unordered_map>;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, std::tuple<Attr>>> where(const Attr&) const& requires specialized_from<Result, std::unordered_map>;

        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) && requires specialized_from<Result, std::unordered_map>;
        template<specialized_from<std::tuple> SrcBindAttrs>
        [[nodiscard]] query_relation<Result, tuptup::tuple_cat_t<BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const& requires specialized_from<Result, std::unordered_map>;

        [[nodiscard]] query_relation<Result, BindAttrs> limit(const std::size_t) && requires specialized_from<Result, std::unordered_map>;
        [[nodiscard]] query_relation<Result, BindAttrs> limit(const std::size_t) const& requires specialized_from<Result, std::unordered_map>;

        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs> order_by(const active_record::order = active_record::order::asc) && requires specialized_from<Result, std::unordered_map>;
        template<Attribute Attr>
        [[nodiscard]] query_relation<Result, BindAttrs> order_by(const active_record::order = active_record::order::asc) const& requires specialized_from<Result, std::unordered_map>;

        [[nodiscard]] query_relation<std::unordered_map<group_type, std::size_t>, BindAttrs> count() && requires specialized_from<Result, std::unordered_map>;
        [[nodiscard]] query_relation<std::unordered_map<group_type, std::size_t>, BindAttrs> count() const& requires specialized_from<Result, std::unordered_map>;

        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<std::unordered_map<group_type, typename Attr::sum::attribute_type>, BindAttrs> sum() && requires specialized_from<Result, std::unordered_map>;
        template<Attribute Attr>
        requires requires{ typename Attr::sum; }
        [[nodiscard]] query_relation<std::unordered_map<group_type, typename Attr::sum::attribute_type>, BindAttrs> sum() const& requires specialized_from<Result, std::unordered_map>;

        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<std::unordered_map<group_type, typename Attr::avg::attribute_type>, BindAttrs> avg() && requires specialized_from<Result, std::unordered_map>;
        template<Attribute Attr>
        requires requires{ typename Attr::avg; }
        [[nodiscard]] query_relation<std::unordered_map<group_type, typename Attr::avg::attribute_type>, BindAttrs> avg() const& requires specialized_from<Result, std::unordered_map>;

        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<std::unordered_map<group_type, typename Attr::max::attribute_type>, BindAttrs> max() && requires specialized_from<Result, std::unordered_map>;
        template<Attribute Attr>
        requires requires{ typename Attr::max; }
        [[nodiscard]] query_relation<std::unordered_map<group_type, typename Attr::max::attribute_type>, BindAttrs> max() const& requires specialized_from<Result, std::unordered_map>;

        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<std::unordered_map<group_type, typename Attr::min::attribute_type>, BindAttrs> min() && requires specialized_from<Result, std::unordered_map>;
        template<Attribute Attr>
        requires requires{ typename Attr::min; }
        [[nodiscard]] query_relation<std::unordered_map<group_type, typename Attr::min::attribute_type>, BindAttrs> min() const& requires specialized_from<Result, std::unordered_map>;
    };
}

#include "query_relation_impl/result_scalar.ipp"
#include "query_relation_impl/result_vector.ipp"
#include "query_relation_impl/result_map.ipp"