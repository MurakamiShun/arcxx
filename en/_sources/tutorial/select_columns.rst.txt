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
    const auto [error, columns] = sql_stmt.exec(connection);
    
    // decltype(error) == std::optional<active_record::string>
    if(error){
        // error handling
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
    const auto [error, names] = sql_stmt.exec(connection);
    
    // decltype(error) == std::optional<active_record::string>
    if(error){
        // error handling
    }

    // decltype(names) == std::vector<ExampleTable::Name>

