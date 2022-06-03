================================
namespace arcxx::sqlite3
================================


.. cpp:struct:: arcxx::sqlite3::connector

    Details are :doc:`/api/connectors/sqlite3_connector`

.. object:: namespace arcxx::sqlite3::options
    
    :cpp:func:`sqlite3_connector::open` flags are hear.

    .. code-block:: cpp

        constexpr auto readonly   = SQLITE_OPEN_READONLY;
        constexpr auto readwrite  = SQLITE_OPEN_READWRITE;
        constexpr auto create     = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;
        constexpr auto uri        = SQLITE_OPEN_URI;
        constexpr auto memory     = SQLITE_OPEN_MEMORY;
        constexpr auto no_mutex   = SQLITE_OPEN_NOMUTEX;
        constexpr auto full_mutex = SQLITE_OPEN_FULLMUTEX;

    Flag explanations are https://www.sqlite.org/c3ref/open.html 