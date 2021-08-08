====================
active_record::model
====================

.. cpp:struct:: template<typename Derived>\
                model

    :code:`table_name` and :code:`attributes` are required in model struct definition.
    :code:`active_record::model` is designed using
    CRTP(Curiously Recurring Template Pattern).

    .. code-block:: cpp
        :caption: A Minimum model define code

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
        * - :cpp:func:`where`
          - 
        * - :cpp:func:`destroy`
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

      The data inserting example is :doc:`here </tutorial/inserting>`.

      .. code-block:: cpp

        static query_relation<bool, BindAttrs> insert(const Derived& model);
        static query_relation<bool, BindAttrs> insert(Derived&& model);

    .. cpp:function:: all()

      Get all model data in database.

      .. code-block:: cpp

        static query_relation<std::vector<Derived>, std::tuple<>> all();

    .. cpp:function:: select()

      The select function example is :doc:`here </tutorial/select_columns>`.

      .. code-block:: cpp

        template<Attribute... Attrs>
        static query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<>> select();

        template<Attribute... Attrs>
        static query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<>> select(const Attrs...);

    .. cpp:function:: pluck()

      The pluck function example is :doc:`here </tutorial/select_columns>`.

      .. code-block:: cpp

        template<Attribute Attr>
        static query_relation<std::vector<Attr>, std::tuple<>> pluck();

        template<Attribute Attr>
        static query_relation<std::vector<Attr>, std::tuple<>> pluck(const Attr);

    .. cpp:function:: where()

      The example is :doc:`here </tutorial/find_columns>`.

      .. code-block:: cpp

        template<Attribute Attr>
        static query_relation<std::vector<Derived>, std::tuple<const Attr*>> where(const Attr&&);
        
        template<Tuple SrcBindAttrs>
        static query_relation<std::vector<Derived>, SrcBindAttrs> where(query_condition<SrcBindAttrs>&&);

    .. cpp:function:: destroy()

      .. code-block:: cpp

        template<Attribute Attr>
        static query_relation<bool, std::tuple<const Attr*>> destroy(const Attr&&);
        template<Tuple SrcBindAttrs>
        static query_relation<bool, SrcBindAttrs> destroy(query_condition<SrcBindAttrs>&&);


    .. cpp:function:: limit()

      .. code-block:: cpp

        static query_relation<std::vector<Derived>, std::tuple<>> limit(const std::size_t);

    .. cpp:function:: order_by()

      .. code-block:: cpp

        template<Attribute Attr>
        static query_relation<std::vector<Derived>, std::tuple<>> order_by(const active_record::order = active_record::order::asc);

    .. cpp:function:: join()

      .. code-block:: cpp

        template<typename ReferModel>
        static query_relation<std::vector<Derived>, std::tuple<>> join();

    .. cpp:function:: left_join()

      .. code-block:: cpp

        template<typename ReferModel>
        requires std::derived_from<ReferModel, model<ReferModel>>
        static query_relation<std::vector<Derived>, std::tuple<>> left_join();

    .. cpp:function:: count()

      The aggregation examples is :doc:`here </tutorial/aggregation>`.

      .. code-block:: cpp

        static query_relation<std::size_t, std::tuple<>> count();

    .. cpp:function:: sum()

      The aggregation examples is :doc:`here </tutorial/aggregation>`.

      .. code-block:: cpp

        template<Attribute Attr>
        requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
        static query_relation<typename Attr::value_type, std::tuple<>> sum();

    .. cpp:function:: avg()

      The aggregation examples is :doc:`here </tutorial/aggregation>`.

      .. code-block:: cpp

        template<Attribute Attr>
        requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
        static query_relation<typename Attr::value_type, std::tuple<>> avg();

    .. cpp:function:: max()

      The aggregation examples is :doc:`here </tutorial/aggregation>`.

      .. code-block:: cpp

        template<Attribute Attr>
        requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
        static query_relation<typename Attr::value_type, std::tuple<>> max();

    .. cpp:function:: min()

      The aggregation examples is :doc:`here </tutorial/aggregation>`.

      .. code-block:: cpp

        template<Attribute Attr>
        requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
        static query_relation<typename Attr::value_type, std::tuple<>> min();