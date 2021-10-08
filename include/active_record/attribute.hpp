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
#include "utils.hpp"
#include "adaptor.hpp"

namespace active_record {
    template<typename Model, typename Attribute, typename Type>
    struct attribute;

    template<typename Model, typename Attribute, typename Type>
    class attribute_common;

    template<typename T>
    concept Attribute = requires {
        std::derived_from<T, attribute_common<typename T::model_type, typename T::attribute_type, typename T::value_type>>;
        T::has_column_name;
    };
    template<typename T>
    struct is_attribute{ static constexpr bool value = Attribute<T>; };

    template<Attribute Attr>
    constexpr std::size_t attribute_size([[maybe_unused]] const Attr&){ return sizeof(typename Attr::value_type); }
    template<Attribute Attr>
    requires std::invocable<typename Attr::value_type::size>
    constexpr std::size_t attribute_size(const Attr& attr){ return attr ? attr.value().size() : 0; }

    template<typename Model, typename Attribute, typename Aggregator>
    struct attribute_aggregator;

    template<typename T>
    concept AttributeAggregator = std::derived_from<T, attribute_aggregator<typename T::model_type, typename T::attribute_type, typename T::aggregator_type>>;

    template<std::derived_from<adaptor> Adaptor, Attribute Attr>
    requires false
    [[nodiscard]] constexpr active_record::string to_string(const Attr& attr);

    template<std::derived_from<adaptor> Adaptor, Attribute Attr>
    requires false
    void from_string(Attr& attr, const active_record::string_view str);
}

template<active_record::Attribute Attr>
struct std::hash<Attr> {
    std::hash<std::optional<typename Attr::value_type>> inner_hash;
    std::size_t operator()(const Attr& key) const {
        return inner_hash(static_cast<std::optional<typename Attr::value_type>>(key));
    }
};