==============================
active_record::sqlite3_connector
==============================


.. cpp:class:: sqlite3_connector

    .. code-block:: cpp

        class sqlite3_connector : public connector;

    This class is database connector class for SQLite3.
    "libsqlite3" is required to use this class.

    .. cpp:function:: open()

        .. code-block:: cpp
            
            static sqlite3_connector open(
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
            
            template<typename Result, specialized_from<std::tuple> BindAttrs>
            auto exec(const query_relation<Result, BindAttrs>& query) -> active_record::expected<Result, active_record::string>;

    .. cpp:function:: create_table()

        .. code-block:: cpp

            template<is_model Mod>
            auto create_table(decltype(abort_if_exist)) -> active_record::expected<void, active_record::string>;
            
            template<is_model Mod>
            auto create_table() -> active_record::expected<void, active_record::string>;

    .. cpp:function:: drop_table()

        .. code-block:: cpp

            template<is_model Mod>
            auto drop_table() -> active_record::expected<void, active_record::string>;

    .. cpp:function:: transaction()

        .. code-block:: cpp

            template<typename F>
            requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t()>>
            auto transaction(F&& func) -> active_record::expected<void, active_record::string>;
            
            template<typename F>
            requires std::convertible_to<F, std::function<transaction::detail::commit_or_rollback_t(sqlite3_connector&)>>
            auto transaction(F&& func) -> active_record::expected<void, active_record::string>;
