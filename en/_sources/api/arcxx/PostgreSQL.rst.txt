===================================
namespace arcxx::PostgreSQL
===================================


.. cpp:struct:: arcxx::PostgreSQL::connector

    Details are :doc:`/api/connectors/postgresql_connector`

.. cpp:struct:: arcxx::PostgreSQL::endpoint

    .. cpp:member:: const arcxx::string server_name
    .. cpp:member:: const arcxx::string port = "5432"
    .. cpp:member:: const arcxx::string db_name

.. cpp:struct:: arcxx::PostgreSQL::auth

    .. cpp:member:: const arcxx::string user
    .. cpp:member:: const arcxx::string password

.. cpp:struct:: arcxx::PostgreSQL::options

    .. cpp:member:: const arcxx::string option
    .. cpp:member:: const arcxx::string debug_of