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

    // decltype(count) == size_t
    std::cout << "columns count:" << count << std::endl;


Column aggregations
===================

Column aggregation functions, :code:`avg()` and :code:`max` etc... are used like :code:`count()` function. 

.. code-block:: cpp

    // decltype(sum) == ExampleTable::ID
    const auto [error, max] = ExampleTable::max<ExampleTable::ID>().exec(connection);
    sum.value();

    /*
     * decltype(avg) == ExampleTable::ID::avg_attribute
     * value_type of avg_attribute is std::optional<double>.
     * Because the average type will be double.
     */
    const auto [error, sum] = ExampleTable::avg<ExampleTable::ID>().exec(connection);
    avg.value();

Also, aggregation functions can be used in select.

.. code-block:: cpp

    /*
     * decltype(aggrgated_tuple) == std::tuple<ExampleTable::ID, ExampleTable::ID>
     * The first element is max id.
     * Second one is min id;
     */
    const auto [error, aggrgated_tuple] =
        ExampleTable::select<ExampleTable::ID::max, ExampleTable::ID::min>().exec(connection);