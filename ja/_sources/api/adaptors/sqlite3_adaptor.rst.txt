==============================
active_record::sqlite3_adaptor
==============================


.. cpp:class:: sqlite3_adaptor

    .. code-block:: cpp

        class sqlite3_adaptor : public adaptor;

    This class is database connector class for SQLite3.
    "libsqlite3" is required to use this class.

    .. cpp:function:: open()

        .. code-block:: cpp
            
            static sqlite3_adaptor open(
                const active_record::string& file_name,
                const int flags = active_record::sqlite3::options::readwrite
            );

        The flag details are on :doc:`active_record::sqlite3::options </api/active_record/sqlite3>`

    .. cpp:function:: close()

        .. code-block:: cpp

            bool close();

        Close SQLite3 connection.

    .. cpp:function:: version()

        .. code-block:: cpp
            
            static active_record::string_view version();

        Returns libsqlite3 version string.

    .. cpp:function:: version_number()

        .. code-block:: cpp
            
            static int version_number();

        Returns libsqlite3 version number.

    .. cpp:function:: exec()

        .. code-block:: cpp
            
            template<specialized_from<std::tuple> BindAttrs>
            auto exec(const query_relation<bool, BindAttrs>& query) -> std::optional<active_record::string>;

            template<typename Result, specialized_from<std::tuple> BindAttrs>
            auto exec(const query_relation<Result, BindAttrs>& query) -> std::pair<std::optional<active_record::string>, Result>;

    .. cpp:function:: create_table()

        .. code-block:: cpp

            template<is_model Mod>
            std::optional<active_record::string> create_table(bool abort_if_exist = true);

    .. cpp:function:: drop_table()

        .. code-block:: cpp

            template<is_model Mod>
            std::optional<active_record::string> drop_table();

    .. cpp:function:: transaction()

        .. code-block:: cpp

            template<std::convertible_to<std::function<active_record::transaction(void)>> F>
            std::pair<std::optional<active_record::string>, active_record::transaction> transaction(F& func);
            template<std::convertible_to<std::function<active_record::transaction(void)>> F>
            std::pair<std::optional<active_record::string>, active_record::transaction> transaction(F&& func);
            template<std::convertible_to<std::function<active_record::transaction(postgresql_adaptor&)>> F>
            std::pair<std::optional<active_record::string>, active_record::transaction> transaction(F& func);
            template<std::convertible_to<std::function<active_record::transaction(postgresql_adaptor&)>> F>
            std::pair<std::optional<active_record::string>, active_record::transaction> transaction(F&& func);