==============
Table Creation
==============


Model Definition
================

:code:`table_name` is required in model struct definition. Also, the model needs to be structured bindable.
And column struct requires :code:`column_name` and inheriting constructors in the definition.

base classes, :doc:`active_record::model </api/active_record/model>` and :doc:`active_record::attribute </api/active_record/attributes>` are using CRTP(Curiously Recurring Template Pattern).

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
    };

Attribute details are written in :doc:`/api/active_record/attributes`.

Table Creation
==============

.. code-block:: cpp
    :caption: Generate table creation SQL code (SQLite3)

    ExampleTable::schema::to_sql<active_record::sqlite3_adaptor>()

.. code-block:: sql
    :caption: Generated SQL code (SQLite3)

    CREATE TABLE example_table(
        id INTEGER PRIMARY KEY,
        name TEXT
    );

.. code-block:: cpp
    :caption: Create table into Database file.

    auto connection = active_record::sqlite3_adaptor::open("example.sqlite3", active_record::sqlite3::options::create);
    connection.create_table<ExampleTable>();