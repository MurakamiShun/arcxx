##############
Table Creation
##############


Table Definition
================

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
        
        struct ID : public active_record::attributes::integer<ExampleTable, ID> {
            using active_record::attributes::integer<ExampleTable, ID>::integer;
            static constexpr auto column_name = "id";
            inline static const auto constraints = { primary_key };
        } id;

        struct Name : public active_record::attributes::string<ExampleTable, Name> {
            using active_record::attributes::string<ExampleTable, Name>::string;
            static constexpr auto column_name = "name";
        } name;
        
        std::tuple<ID&, Name&> attributes = std::tie(id, name);
    };

Table Creation
==============

.. code-block:: cpp
    :caption: Generate table creation sql code (SQLite3)

    ExampleTable::table_definition<active_record::sqlite3_adaptor>().to_sql()

.. code-block:: sql
    :caption: Created SQL code (SQLite3)

    CREATE TABLE IF NOT EXISTS example_table(
        id INTEGER PRIMARY KEY,
        name TEXT
    );