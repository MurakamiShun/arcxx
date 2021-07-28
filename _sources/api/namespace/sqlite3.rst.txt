================================
namespace active_record::sqlite3
================================


.. cpp:struct:: active_record::sqlite3::adaptor

    Details are :doc:`/api/adaptors/sqlite3_adaptor`

.. object:: namespace active_record::sqlite3::options
    
    :cpp:func:`sqlite3_adaptor::open` flags are hear.

    .. code-block:: cpp

        constexpr auto readonly   = SQLITE_OPEN_READONLY;
        constexpr auto readwrite  = SQLITE_OPEN_READWRITE;
        constexpr auto create     = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;
        constexpr auto uri        = SQLITE_OPEN_URI;
        constexpr auto memory     = SQLITE_OPEN_MEMORY;
        constexpr auto no_mutex   = SQLITE_OPEN_NOMUTEX;
        constexpr auto full_mutex = SQLITE_OPEN_FULLMUTEX;