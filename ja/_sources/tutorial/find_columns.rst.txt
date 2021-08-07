============
Find columns
============


Where clause
============

To find and get columns, use :code:`where()`.

.. code-block:: cpp

    auto connection = active_record::sqlite3::adaptor::open("example.sqlite3");
    
    auto sql_stmt = ExampleTable::where(ExampleTable::ID{1});

    // print SQL code
    std::cout << sql_stmt.to_sql() << std::endl;

    // Execute data finding
    const auto [error, columns] = sql_stmt.exec(connection);

    if(error){
        // error handling
    }

    // decltype(columns) == std::vector<ExampleTable>
    std::cout << "column count:" << columns.size() << std::endl;
    std::cout << "column name:" << columns[0].name << std::endl;


Where clause can chain the conditions. The conditions will join with AND.
The below codes will be the same result.

.. code-block:: cpp

    ExampleTable::where(ExampleTable::ID::between(0,10)).where(ExampleTable::Name::like("user_"));

    ExampleTable::where(ExampleTable::ID::between(0,10) && ExampleTable::Name::like("user_"));