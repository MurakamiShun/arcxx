#pragma once
#include <string>
#include <string_view>
#include <chrono>

namespace active_record{
    class database{
    private:
        template<typename Result>
        class query_relation;
    public:
        using string = std::string;
        using string_view = std::string_view;
        using datetime = std::chrono::utc_clock;
        template<typename SubClass>
        class table;

        string schema();
    };
}