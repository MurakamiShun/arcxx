========
Group by
========


Column aggregations by grouping
===============================

To count grouped columns, use :code:`group_by()` and :code:`count()`.

.. code-block:: cpp

    const auto [error, users] = User::group_by<User::Gender>().count().exec(conn);
    static_cast(std::is_same_v<decltype(users), std::unordered_map<User::Gender, std::size_t>>);


To aggregate grouped columns, use :code:`pluck()` or :code:`select()`.

.. code-block:: cpp

    const auto [error, users] = User::group_by<User::Gender>().select<User::Age::max, User::Age::min>().exec(conn);
    static_cast(std::is_same_v<decltype(users), std::unordered_map<User::Gender, std::tuple<User::Age::max, User::Age::min>>>);

    const auto [error, users] = User::group_by<User::Gender>().pluck<User::Age::avg>().exec(conn);
    static_cast(std::is_same_v<decltype(users), std::unordered_map<User::Gender, User::Age::avg>>);