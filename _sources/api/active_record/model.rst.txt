====================
active_record::model
====================

.. cpp:struct:: template<typename Derived>\
                model

    :code:`table_name` and :code:`attributes` are required in model struct definition.
    :code:`active_record::model` is designed using
    CRTP(Curiously Recurring Template Pattern).

    .. code-block:: cpp
        :caption: Minimum model define code

        struct ExampleTable : public active_record::model<ExampleTable> {
            static constexpr auto table_name = "example_table";
            
            std::tuple<> attributes;
        };

    .. list-table:: Member functions

        * - (constructor)
          - :code:`model` has no internal state
        * - (destructor)
          - :code:`model` has no internal state

    .. list-table:: Generates query functions

        * - :cpp:func:`insert`
          - 
        * - :cpp:func:`all`
          - 
        * - :cpp:func:`select`
          - 
        * - :cpp:func:`pluck`
          - 
        * - :cpp:func:`update`
          - 
        * - :cpp:func:`destroy`
          - 
        * - :cpp:func:`where`
          - 
        * - :cpp:func:`limit`
          - 
        * - :cpp:func:`order_by`
          - 
        * - :cpp:func:`join`
          - 
        * - :cpp:func:`left_join`
          - 
        * - :cpp:func:`count`
          - 
        * - :cpp:func:`sum`
          - 
        * - :cpp:func:`avg`
          - 
        * - :cpp:func:`max`
          - 
        * - :cpp:func:`min`
          - 

    .. list-table:: Member structures

        * - :cpp:struct:`schema`
          -

    .. cpp:struct:: schema

        .. cpp:function::  to_sql()

            .. code-block:: cpp

                template<std::derived_from<adaptor> Adaptor>
                static active_record::string to_sql(bool create_if_not_exist = false);

    .. cpp:function:: insert()

    .. cpp:function:: all()

    .. cpp:function:: select()

    .. cpp:function:: pluck()

    .. cpp:function:: update()

    .. cpp:function:: destroy()

    .. cpp:function:: where()

    .. cpp:function:: limit()

    .. cpp:function:: order_by()

    .. cpp:function:: join()

    .. cpp:function:: count()

    .. cpp:function:: sum()

    .. cpp:function:: avg()

    .. cpp:function:: max()

    .. cpp:function:: min()