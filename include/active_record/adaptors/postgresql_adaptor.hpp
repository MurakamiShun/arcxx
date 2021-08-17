#pragma once
#include "../adaptor.hpp"
#if __has_include(<postgresql/libpq-fe.h>)
#include <postgresql/libpq-fe.h>

namespace active_record {
    class pg_adaptor : adaptor {
    private:
    public:
    };
}

#endif