##############
Table Creation
##############


Table Definition
================

"table_name" and "attributes" are required in model struct definition.
And column struct requires "column_name" and inheriting constructors in definition.

base classes, "active_record::model" and "active_record::attribute" are using CRTP(Curiously Recurring Template Pattern).

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

    #include <active_record.hpp>

    struct ExampleTable : public active_record::model<ExampleTable> {
        static constexpr auto table_name = "example_table";
        
        // id column
        struct ID : public active_record::attributes::integer<ExampleTable, ID> {
            using active_record::attributes::integer<ExampleTable, ID>::integer;
            static constexpr auto column_name = "id";
            inline static const auto constraints = { primary_key };
        } id;

        // name column
        struct Name : public active_record::attributes::string<ExampleTable, Name> {
            using active_record::attributes::string<ExampleTable, Name>::string;
            static constexpr auto column_name = "name";
        } name;
        
        std::tuple<ID&, Name&> attributes = std::tie(id, name);
    };

Attribute details are wirtten in :doc:`/namespaces/attributes`.

Table Creation
==============

.. code-block:: cpp
    :caption: Generate table creation sql code (SQLite3)

    ExampleTable::schema::to_sql<active_record::sqlite3_adaptor>()

.. code-block:: sql
    :caption: Created SQL code (SQLite3)

    CREATE TABLE example_table(
        id INTEGER PRIMARY KEY,
        name TEXT
    );

.. code-block:: cpp
    :caption: Create table into sql file.

    auto connection = active_record::sqlite3_adaptor::open("example.sqlite3", active_record::sqlite3::options::create);
    connection.create_table<ExampleTable>();