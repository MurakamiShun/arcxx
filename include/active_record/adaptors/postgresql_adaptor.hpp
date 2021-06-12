#pragma once
#include "../adaptor.hpp"
#if __has_include(<libpq-fe.h>)
#include <libqg-fe.h>

namespace active_record {
    class pg_adaptor : adaptor {
    private:
    public:
    };
}

#endif