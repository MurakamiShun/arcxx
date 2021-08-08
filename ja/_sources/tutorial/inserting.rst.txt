==============
Data Inserting
==============


Example Table
=============

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

Data inserting code.

.. code-block:: cpp
    :caption: inserting example code

    auto connection = active_record::sqlite3::adaptor::open("example.sqlite3");
    ExampleTable data = {
        .id = 1,
        .name = "unknown"
    };

    auto sql_stmt = ExampleTable::insert(data);

    // print SQL code
    std::cout << sql_stmt.to_sql() << std::endl;

    // Execute data inserting
    const auto error = sql_stmt.exec(connection);
    
    // decltype(error) == std::optional<active_record::string>
    if(error){
        // error handling
    }

