==============
Select columns
==============

Select function
===============

:code:`select` function generate SQL statement 
which return selected columns as :code:`std::tuple<selected columns...>`.

.. code-block:: cpp
    :caption: select example code

    auto sql_stmt = ExampleTable::select<ExampleTable::ID, ExampleTable::Name>();

    // print SQL code
    std::cout << sql_stmt.to_sql() << std::endl;

    // Execute data inserting
    const auto id_name_result = sql_stmt.exec(connection);
    
    // decltype(id_name_result) == tl::expected<std::tuple<ExampleTable::ID, ExampleTable::Name>, arcxx::string>
    if(!id_name_result){
        // error handling
        std::cout << "Error message:" << names_result.error() << std::endl;
    }

    // decltype(columns) == std::vector<std::tuple<ExampleTable::ID, ExampleTable::Name>>



Pluck function
==============

:code:`pluck` function generate SQL statement 
which return only selected column.

.. code-block:: cpp
    :caption: pluck example code

    auto sql_stmt = ExampleTable::pluck<ExampleTable::Name>();

    // print SQL code
    std::cout << sql_stmt.to_sql() << std::endl;

    // Execute data inserting
    const auto names_result = sql_stmt.exec(connection);
    
    // decltype(names_result) == tl::expected<std::vector<ExampleTable::Name>, arcxx::string>
    if(!names_result){
        // error handling
        std::cout << "Error message:" << names_result.error() << std::endl;
    }

