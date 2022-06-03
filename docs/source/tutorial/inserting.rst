==============
Data Inserting
==============


Example Table
=============

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

Data Inserting
==============

This example will insert the below values.

.. list-table:: example data

    * - column name
      - data value
    * - example_table.id
      - :code:`1`
    * - example_table.name
      - :code:`unknown`

.. code-block:: cpp
    :caption: inserting example code

    auto connection = arcxx::sqlite3::connector::open("example.sqlite3");
    ExampleTable data = {
        .id = 1,
        .name = "unknown"
    };

    auto sql_stmt = ExampleTable::insert(data);

    // print SQL code
    std::cout << sql_stmt.to_sql() << std::endl;

    // Execute data inserting
    const auto result = sql_stmt.exec(connection);
    
    // decltype(result) == tl::expected<void, arcxx::string>
    if(!result){
        // error handling
    }

