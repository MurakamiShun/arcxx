===========
Aggregation
===========


Columns count
=============

To count columns, use :code:`count()`.

.. code-block:: cpp

    auto connection = active_record::sqlite3::adaptor::open("example.sqlite3");
    auto sql_stmt = ExampleTable::count();

    // print SQL code
    std::cout << sql_stmt.to_sql() << std::endl;

    // Execute data counting
    const auto [error, count] = sql_stmt.exec(connection);

    // decltype(error) == std::optional<active_record::string>
    if(error){
        // error handling
    }

    std::cout << "columns count:" << count << std::endl;
