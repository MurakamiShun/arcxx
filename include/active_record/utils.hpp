#pragma once
#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>
#include <chrono>
#include <tuple>

namespace active_record{
    using string = std::string;
    using string_view = std::string_view;
    using datetime = std::chrono::utc_clock;

    template <class ContainerType> 
    concept Container = requires(ContainerType a, const ContainerType b) 
    {
        requires std::regular<ContainerType>;
        requires std::swappable<ContainerType>;
        requires std::destructible<typename ContainerType::value_type>;
        requires std::same_as<typename ContainerType::reference, typename ContainerType::value_type &>;
        requires std::same_as<typename ContainerType::const_reference, const typename ContainerType::value_type &>;
        requires std::forward_iterator<typename ContainerType::iterator>;
        requires std::forward_iterator<typename ContainerType::const_iterator>;
        requires std::signed_integral<typename ContainerType::difference_type>;
        requires std::same_as<typename ContainerType::difference_type, typename std::iterator_traits<typename ContainerType::iterator>::difference_type>;
        requires std::same_as<typename ContainerType::difference_type, typename std::iterator_traits<typename ContainerType::const_iterator>::difference_type>;
        { a.begin() } -> std::same_as<typename ContainerType::iterator>;
        { a.end() } -> std::same_as<typename ContainerType::iterator>;
        { b.begin() } -> std::same_as<typename ContainerType::const_iterator>;
        { b.end() } -> std::same_as<typename ContainerType::const_iterator>;
        { a.cbegin() } -> std::same_as<typename ContainerType::const_iterator>;
        { a.cend() } -> std::same_as<typename ContainerType::const_iterator>;
        { a.size() } -> std::same_as<typename ContainerType::size_type>;
        { a.max_size() } -> std::same_as<typename ContainerType::size_type>;
        { a.empty() } -> std::same_as<bool>;
    };

    template<class TupleType>
    concept Tuple = requires {
        std::tuple_size<TupleType>::value;
        // { std::bool_constant<std::is_standard_layout_v<TupleType>>{} } -> std::same_as<std::bool_constant<false>>;
    };

    active_record::string sanitize(const active_record::string& src) {
        active_record::string result;
        for(const auto& c : src) {
            switch(c){
                case '\'':
                    result += "\'\'";
                    break;
                case '\\':
                    result += "\\\\";
                    break;
                default:
                    result += c;
                    break;
            }
        };
        return result;
    }
}