#pragma once
#include "../query.hpp"
#include "../attribute.hpp"

namespace active_record {
    template<Tuple BindAttrs>
    struct query_expression {
        using str_or_bind = std::variant<active_record::string, std::size_t>;
        std::vector<str_or_bind> queries;
        BindAttrs bind_attrs;

        template<typename... Args>
        query_expression(Args&&... args) {
            std::size_t index = 0;
            queries = {([&index]<typename Arg>(Arg&& arg){
                if constexpr (Attribute<Arg>) {
                    ++index;
                    return index;
                }
                else return std::forward<Arg>(arg);
            }(std::forward<Args>(args)))...};
            // bind_attrs = active_record::apply_elements_filter<Attribute>(std::make_tuple(std::forward<Args>(args)...));
        }

        [[nodiscard]] static consteval std::size_t bind_attrs_count() noexcept {
            return std::tuple_size_v<BindAttrs>;
        }
    };
}