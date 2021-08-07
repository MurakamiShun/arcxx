==============================
active_record::sqlite3_adaptor
==============================


.. cpp:class:: sqlite3_adaptor

    .. code-block:: cpp

        class sqlite3_adaptor : public adaptor;

    Database connector class for SQLite3.
    "libsqlite3" is required to use this class.

    .. cpp:function:: open()

        .. code-block:: cpp
            
            static sqlite3_adaptor open(
                const active_record::string& file_name,
                const int flags = active_record::sqlite3::options::readwrite
            );

        Flags are :doc:`active_record::sqlite3::options </api/active_record/sqlite3>`

    .. cpp:function:: close()

        .. code-block:: cpp

            bool close();

        Close sqlite3 connection.

    .. cpp:function:: version()

        .. code-block:: cpp
            
            static active_record::string_view version();

        Returns libsqlite3 version string.

    .. cpp:function:: verison_number()

        .. code-block:: cpp
            
            static int version_number();

        Returns libsqlite3 version number.

    .. cpp:function:: exec()

    .. cpp:function:: create_table()

    .. cpp:function:: transaction()