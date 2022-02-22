=======================================
active_record::postgresql_adaptor
=======================================

.. cpp:class:: postgresql_adaptor

    .. code-block:: cpp

        class postgresql_adaptor : public adaptor;

    This class is database connector class for PostgreSQL.
    "libpq-dev" is required to use this class.


    .. cpp:function:: open()

        .. code-block:: cpp

            static postgresql_adaptor open(
                const PostgreSQL::endpoint endpoint_info,
                const std::optional<PostgreSQL::auth> auth_info = std::nullopt,
                const std::optional<PostgreSQL::options> option = std::nullopt
            )
            static postgresql_adaptor open(const active_record::string& connection_info)

    .. cpp:function:: close()


    .. cpp:function:: protocol_version()

        Return PQprotocolVersion.

        .. code-block:: cpp

            int protocol_version() const;

    .. cpp:function:: server_version()

        Return PQserverVersion.

        .. code-block:: cpp

            int server_version() const;

    .. cpp:function:: exec()

        .. code-block:: cpp
            
            template<specialized_from<std::tuple> BindAttrs>
            auto exec(const query_relation<bool, BindAttrs>& query) -> std::optional<active_record::string>;

            template<typename Result, specialized_from<std::tuple> BindAttrs>
            auto exec(const query_relation<Result, BindAttrs>& query) -> std::pair<std::optional<active_record::string>, Result>;

    .. cpp:function:: create_table()

        .. code-block:: cpp

            template<is_model Mod>
            std::optional<active_record::string> create_table(decltype(abort_if_exist));
            template<is_model Mod>
            std::optional<active_record::string> create_table();

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