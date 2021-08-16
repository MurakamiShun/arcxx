#pragma once
#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>
#include <chrono>
#include <tuple>
#include <functional>
#include <variant>
#include <any>
#include <vector>
#include <array>
#include <utility>
#include <optional>
#include <charconv>
#include <unordered_map>

namespace active_record{
    using string = std::string;
    using string_view = std::string_view;
    // utc_clock is not implements any compiler.
    //using datetime = std::chrono::utc_clock;
    using datetime = std::chrono::system_clock;

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

    template<typename T>
    concept same_as_vector = std::same_as<T, std::vector<typename T::value_type>>;

    template<typename T>
    concept same_as_unordered_map = std::same_as<T, std::unordered_map<typename T::key_type, typename T::mapped_type>>;

    template<class Tuple, std::size_t... I>
    [[nodiscard]] constexpr auto tuple_slice(Tuple& t, std::index_sequence<I...>&&) noexcept {
        return std::tie(std::get<I>(t)...);
    }
    template<class Tuple, std::size_t... I>
    [[nodiscard]] constexpr auto tuple_slice(Tuple&& t, std::index_sequence<I...>&&) {
        return std::make_tuple(std::get<I>(t)...);
    }

    template<std::size_t from, std::size_t to>
    [[nodiscard]] constexpr auto make_index_sequence_between() noexcept {
        return []<std::size_t... I>(std::index_sequence<I...>&&){
            return std::index_sequence<(I + from)...>{};
        }(std::make_index_sequence<to-from>());
    }
    
    namespace detail {
        // non const
        template<std::size_t I>
        [[nodiscard]] constexpr auto indexed_apply_aux(std::tuple<>&&) noexcept {
            return std::make_tuple();
        }
        template<std::size_t I>
        [[nodiscard]] constexpr auto indexed_apply_aux(std::tuple<>&) noexcept {
            return std::make_tuple();
        }
        template<std::size_t I, class Head, class... Tail>
        [[nodiscard]] constexpr auto indexed_apply_aux(std::tuple<Head&, Tail&...>&& t) noexcept {
            return std::tuple_cat(
                std::make_tuple(std::make_pair(I, std::ref(std::get<0>(t)))),
                indexed_apply_aux<I+1>(tuple_slice(
                    t,
                    make_index_sequence_between<1, sizeof...(Tail)+1>()
                ))
            );
        }
        template<std::size_t I, class Head, class... Tail>
        [[nodiscard]] constexpr auto indexed_apply_aux(std::tuple<Head, Tail...>& t) noexcept {
            return std::tuple_cat(
                std::make_tuple(std::make_pair(I, std::ref(std::get<0>(t)))),
                indexed_apply_aux<I+1>(tuple_slice(
                    t,
                    make_index_sequence_between<1, sizeof...(Tail)+1>()
                ))
            );
        }
        // const overload
        template<std::size_t I>
        [[nodiscard]] constexpr auto indexed_apply_aux(const std::tuple<>&) noexcept {
            return std::make_tuple();
        }
        template<std::size_t I, class Head, class... Tail>
        [[nodiscard]] constexpr auto indexed_apply_aux(const std::tuple<const Head&, const Tail&...>&& t) noexcept {
            return std::tuple_cat(
                std::make_tuple(std::make_pair(I, std::cref(std::get<0>(t)))),
                indexed_apply_aux<I+1>(tuple_slice(
                    t,
                    make_index_sequence_between<1, sizeof...(Tail)+1>()
                ))
            );
        }
        template<std::size_t I, class Head, class... Tail>
        [[nodiscard]] constexpr auto indexed_apply_aux(const std::tuple<Head, Tail...>& t) noexcept {
            return std::tuple_cat(
                std::make_tuple(std::make_pair(I, std::cref(std::get<0>(t)))),
                indexed_apply_aux<I+1>(tuple_slice(
                    t,
                    make_index_sequence_between<1, sizeof...(Tail)+1>()
                ))
            );
        }
    }

    template<class TupleType>
    concept Tuple = requires {
        std::tuple_size<TupleType>::value;
        // { std::bool_constant<std::is_standard_layout_v<TupleType>>{} } -> std::same_as<std::bool_constant<false>>;
    };

    template<class F, class Tuple>
    constexpr decltype(auto) indexed_apply(F&& f, Tuple&& t){
        return std::apply(f, detail::indexed_apply_aux<0>(t));
    }

    [[nodiscard]] inline active_record::string sanitize(const active_record::string& src) {
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