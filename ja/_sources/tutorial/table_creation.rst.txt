==============
Table Creation
==============


Model Definition
================

:code:`table_name` is required in model struct definition. Also, the model needs to be structured bindable.
And column struct requires :code:`column_name` and inheriting constructors in the definition.

base classes, :doc:`arcxx::model </api/arcxx/model>` and :doc:`arcxx::attribute </api/arcxx/attributes>` are using CRTP(Curiously Recurring Template Pattern).

.. list-table:: example_table

    * - column name
      - data type
      - options
    * - id
      - INTEGER
      - PRIMARY_KEY
    * - name
      - TEXT
      -

.. code-block:: cpp
    :caption: example_table code

    #include <arcxx.hpp>

    struct ExampleTable : public arcxx::model<ExampleTable> {
        static constexpr auto table_name = "example_table";
        
        // id column
        struct ID : public arcxx::attributes::integer<ExampleTable, ID> {
            using arcxx::attributes::integer<ExampleTable, ID>::integer;
            static constexpr auto column_name = "id";
            inline static const auto constraints = { primary_key };
        } id;

        // name column
        struct Name : public arcxx::attributes::string<ExampleTable, Name> {
            using arcxx::attributes::string<ExampleTable, Name>::string;
            static constexpr auto column_name = "name";
        } name;
    };

Attribute details are written in :doc:`/api/arcxx/attributes`.

Table Creation
==============

Use :code:`connector::create_table()` to create table into database.
:code:`create_table()` will not cause error when a given table exists.
:code:`create_table(abort_if_exists)` will return error message when exists the table.

.. code-block:: sql
    :caption: Generated SQL code (SQLite3)

    CREATE TABLE example_table(
        id INTEGER PRIMARY KEY,
        name TEXT
    );

.. code-block:: cpp
    :caption: Create table into Database file.

    auto connection = arcxx::sqlite3_connector::open("example.sqlite3", arcxx::sqlite3::options::create);
    connection.create_table<ExampleTable>();

    connection.create_table<ExampleTable>(arcxx::abort_if_exists); // return error message due to exists table