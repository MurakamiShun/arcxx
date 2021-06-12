#pragma once

#include "../adaptor.hpp"
#if __has_include(<sqlite3.h>)
#include <sqlite3.h>

namespace active_record{
    class sqlite3_adaptor : adaptor {
    private:
    public:
    };
}

#endif