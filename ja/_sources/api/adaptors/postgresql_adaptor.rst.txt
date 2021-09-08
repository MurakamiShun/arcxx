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

    .. cpp:function:: create_table()

    .. cpp:function:: transaction()