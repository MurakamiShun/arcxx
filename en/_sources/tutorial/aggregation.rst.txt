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
    const auto count_result = sql_stmt.exec(connection);

    // decltype(count_result) == tl::expected<size_t, active_record::string>
    if(!count_result){
        // error handling
        std::cout << "Error message:" << count_result.error() << std::endl;
    }

    std::cout << "columns count:" << count_result.value() << std::endl;


Column aggregations
===================

Column aggregation functions, :code:`avg()` and :code:`max` etc... are used like :code:`count()` function. 

.. code-block:: cpp

    // decltype(max_result) == tl::expected<ExampleTable::ID, active_record::string>
    const auto max_result = ExampleTable::max<ExampleTable::ID>().exec(connection);
    max_result.value();

    /*
     * decltype(avg_result) == tl::expected<ExampleTable::ID::avg_attribute, active_record::string>
     * value_type of avg_attribute is std::optional<double>.
     * Because the average type will be double.
     */
    const auto avg_result = ExampleTable::avg<ExampleTable::ID>().exec(connection);
    avg_result.value();

Also, aggregation functions can be used in select.

.. code-block:: cpp

    /*
     * decltype(aggrgated_tuple) == std::tuple<ExampleTable::ID, ExampleTable::ID>
     * The first element is max id.
     * Second one is min id;
     */
    const auto aggrgated_tuple_result =
        ExampleTable::select<ExampleTable::ID::max, ExampleTable::ID::min>().exec(connection);
    
    std::get<1>(aggrgated_tuple_result.value());