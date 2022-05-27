===================================
namespace active_record::PostgreSQL
===================================


.. cpp:struct:: active_record::PostgreSQL::connector

    Details are :doc:`/api/connectors/postgresql_connector`

.. cpp:struct:: active_record::PostgreSQL::endpoint

    .. cpp:member:: const active_record::string server_name
    .. cpp:member:: const active_record::string port = "5432"
    .. cpp:member:: const active_record::string db_name

.. cpp:struct:: active_record::PostgreSQL::auth

    .. cpp:member:: const active_record::string user
    .. cpp:member:: const active_record::string password

.. cpp:struct:: active_record::PostgreSQL::options

    .. cpp:member:: const active_record::string option
    .. cpp:member:: const active_record::string debug_of