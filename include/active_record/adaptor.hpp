#pragma once
#include "utils.hpp"

namespace active_record{
    struct adaptor {
        static constexpr bool bindable = false;
        template<typename T>
        active_record::string attribute_typename();
        static active_record::string bind_variable_str(const size_t idx);
    };

    struct common_adaptor : public adaptor{
        static constexpr bool bindable = false;
        static active_record::string bind_variable_str(const size_t idx) {
            return active_record::string{ "'common_adaptor can not bind parameters. Library has some problems.'" } + std::to_string(idx);
        }
    };

    enum class transaction{
        rollback,
        commit
    };
}