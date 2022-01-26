====================
active_record::model
====================

.. cpp:struct:: template<typename Derived>\
                model

    :code:`table_name` static member variable is required in model struct definition.
    :code:`active_record::model` is designed using
    CRTP(Curiously Recurring Template Pattern).

    .. code-block:: cpp
        :caption: A Minimum model define code

        struct ExampleTable : public active_record::model<ExampleTable> {
            static constexpr auto table_name = "example_table";
        };

    .. list-table:: Member functions

        * - (constructor)
          - 
        * - (destructor)
          - 

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
        * - :cpp:func:`group_by`
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

        static auto insert(const Derived& model) -> query_relation<bool, std::tuple<bind attributes...>>;
        static auto insert(Derived&& model) -> query_relation<bool, std::tuple<bind attributes...>>;

    .. cpp:function:: all()

      Get all model data in database.

      .. code-block:: cpp

        static auto all() -> query_relation<std::vector<Derived>, std::tuple<>>;

    .. cpp:function:: select()

      The select function example is :doc:`here </tutorial/select_columns>`.

      .. code-block:: cpp

        template<is_attribute... Attrs>
        static auto select() -> query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<>>;
        template<is_attribute_aggregator... Aggregators>
        static auto select() -> query_relation<std::tuple<Aggregators::attribute_type...>, std::tuple<>>;

    .. cpp:function:: pluck()

      The pluck function example is :doc:`here </tutorial/select_columns>`.

      .. code-block:: cpp

        template<is_attribute Attr>
        static auto pluck() -> query_relation<std::vector<Attr>, std::tuple<>>;
        template<is_attribute_aggregator Aggregator>
        static auto pluck() -> query_relation<Aggregator::attribute_type, std::tuple<>>;

    .. cpp:function:: where()

      The example is :doc:`here </tutorial/find_columns>`.

      .. code-block:: cpp

        template<is_attribute Attr>
        static auto where(const Attr&) -> query_relation<std::vector<Derived>, std::tuple<Attr>>;
        
        template<specialized_from<std::tuple> SrcBindAttrs>
        static auto where(query_condition<SrcBindAttrs>&&) -> query_relation<std::vector<Derived>, SrcBindAttrs>;

    .. cpp:function:: destroy()

      .. code-block:: cpp

        template<is_attribute Attr>
        static auto destroy(const Attr&&) -> query_relation<bool, std::tuple<Attr>>;
        
        template<specialized_from<std::tuple> SrcBindAttrs>
        static auto destroy(query_condition<SrcBindAttrs>&&) -> query_relation<bool, SrcBindAttrs>;


    .. cpp:function:: limit()

      .. code-block:: cpp

        static auto limit(const std::size_t) -> query_relation<std::vector<Derived>, std::tuple<>>;

    .. cpp:function:: order_by()

      .. code-block:: cpp

        template<is_attribute Attr>
        static auto order_by(const active_record::order = active_record::order::asc) -> query_relation<std::vector<Derived>, std::tuple<>>;

    .. cpp:function:: join()

      .. code-block:: cpp

        template<typename ReferModel>
        requires std::derived_from<ReferModel, model<ReferModel>>
        static auto join() -> query_relation<std::vector<Derived>, std::tuple<>>;

    .. cpp:function:: left_join()

      .. code-block:: cpp

        template<typename ReferModel>
        requires std::derived_from<ReferModel, model<ReferModel>>
        static auto left_join() -> query_relation<std::vector<Derived>, std::tuple<>>;

    .. cpp:function:: group_by()

      .. code-block:: cpp

        template<is_attribute Attr>
        static auto group_by() -> query_relation<std::unordered_map<Attr, std::tuple<>>, std::tuple<>>;

    .. cpp:function:: count()

      The aggregation example is :doc:`here </tutorial/aggregation>`.

      .. code-block:: cpp

        static auto count() -> query_relation<std::size_t, std::tuple<>>;

    .. cpp:function:: sum()

      The aggregation example is :doc:`here </tutorial/aggregation>`.

      .. code-block:: cpp

        template<is_attribute Attr>
        requires requires{ typename Attr::sum; }
        static auto sum() -> query_relation<Attr::sum::attribute_type, std::tuple<>>;

    .. cpp:function:: avg()

      The aggregation example is :doc:`here </tutorial/aggregation>`.

      .. code-block:: cpp

        template<is_attribute Attr>
        requires requires{ typename Attr::avg; }
        static auto avg() -> query_relation<Attr::avg::attribute_type, std::tuple<>>;

    .. cpp:function:: max()

      The aggregation example is :doc:`here </tutorial/aggregation>`.

      .. code-block:: cpp

        template<is_attribute Attr>
        requires requires{ typename Attr::max; }
        static auto max() -> query_relation<Attr::max::attribute_type, std::tuple<>>;

    .. cpp:function:: min()

      The aggregation example is :doc:`here </tutorial/aggregation>`.

      .. code-block:: cpp

        template<is_attribute Attr>
        requires requires{ typename Attr::min; }
        static auto min() -> query_relation<Attr::min::attribute_type, std::tuple<>>;
