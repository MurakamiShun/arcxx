=============================
arcxx::query_relation
=============================

.. cpp:struct:: template<typename Result, specialized_from<std::tuple> BindAttrs>\
                query_relation

    :code:`Result` is returned type of :cpp:func:`exec`.

    .. list-table:: Member functions

        * - :cpp:func:`exec`
          - 
        * - :cpp:func:`to_sql`
          - 

    .. list-table:: Generates query functions

        * - :cpp:func:`select`
          - 
        * - :cpp:func:`pluck`
          - 
        * - :cpp:func:`update`
          - 
        * - :cpp:func:`where`
          - 
        * - :cpp:func:`limit`
          - 
        * - :cpp:func:`order_by`
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

    .. cpp:function:: exec()

        .. code-block:: cpp

            template<std::derived_from<connector> Connector>
            auto exec(Connector& adapt) const -> std::pair<std::optional<arcxx::string>, Result>;

            template<std::derived_from<connector> Connector>
            requires std::same_as<Result, bool>
            auto exec(Connector& adapt) const -> std::optional<arcxx::string>;


    .. cpp:function:: to_sql()

        .. code-block:: cpp

            template<std::derived_from<connector> Connector = common_connector>
            const arcxx::string to_sql() const 

    .. cpp:function:: where()

        .. code-block:: cpp

            template<is_attribute Attr>
            auto where(const Attr&) -> query_relation<Result, std::tuple<bind attributes...>>;
            
            template<specialized_from<std::tuple> SrcBindAttrs>
            auto where(query_condition<SrcBindAttrs>&&) -> query_relation<Result, std::tuple<bind attributes...>>;
            
    .. cpp:function:: select()

        .. code-block:: cpp

            /* result type is specialized from std::vector */
            template<is_attribute... Attrs>
            auto select() -> query_relation<std::vector<std::tuple<Attrs...>>, std::tuple<bind attributes...>>;

            template<is_attribute_aggregator... Attrs>
            auto select() -> query_relation<std::tuple<Attrs::attribute_type...>, std::tuple<bind attributes...>>;


            /* result type is specialized from std::unordered_map */
            template<is_attribute_aggregator... Attrs>
            auto select() -> query_relation<std::unordered_map<Result::key_type, std::tuple<Attrs::attribute_type...>>, std::tuple<bind attributes...>>;
            

    .. cpp:function:: pluck()

        .. code-block:: cpp

            /* result type is specialized from std::vector */
            template<is_attribute Attr>
            auto pluck() -> query_relation<std::vector<Attr>, std::tuple<bind attributes...>>;
            
            template<is_attribute_aggregator Attr>
            auto pluck() -> query_relation<std::vector<Attr>, std::tuple<bind attributes...>>;
            

            /* result type is specialized from std::unordered_map */
            template<is_attribute_aggregator Attr>
            auto pluck() -> query_relation<std::unordered_map<Result::key_type, Attr::attribute_type>, std::tuple<bind attributes...>>;

    .. cpp:function:: update()

        .. code-block:: cpp

            /* result type is std::vector<Model> */
            template<is_attribute... Attrs>
            requires is_model<Result::value_type>
            auto update(const Attrs&...) -> query_relation<bool, std::tuple<bind attributes...>>;

    .. cpp:function:: limit()

        .. code-block:: cpp

            /* result type is specialized from std::vector */
            auto limit(const std::size_t) -> query_relation<Result, std::tuple<bind attributes...>>;

            /* result type is specialized from std::unordered_map */
            auto limit(const std::size_t) -> query_relation<Result, std::tuple<bind attributes...>>;


    .. cpp:function:: order_by()

        .. code-block:: cpp

            /* result type is specialized from std::vector */
            template<Attribute Attr>
            auto order_by(const arcxx::order = arcxx::order::asc) -> query_relation<Result, std::tuple<bind attributes...>>;

            /* result type is specialized from std::unordered_map */
            template<Attribute Attr>
            auto order_by(const arcxx::order = arcxx::order::asc) -> query_relation<Result, std::tuple<bind attributes...>>;


    .. cpp:function:: count()

        .. code-block:: cpp

            /* result type is specialized from std::vector */
            auto count() -> query_relation<std::size_t, std::tuple<bind attributes...>>;

            /* result type is specialized from std::unordered_map */
            auto count() -> query_relation<std::unordered_map<Result::key_type, std::size_t>, std::tuple<bind attributes...>>;


    .. cpp:function:: sum()

        .. code-block:: cpp

            /* result type is specialized from std::vector */
            template<is_attribute Attr>
            requires requires{ typename Attr::sum; }
            auto sum() -> query_relation<Attr::sum::attribute_type, std::tuple<bind attributes...>>;

            /* result type is specialized from std::unordered_map */
            template<is_attribute Attr>
            requires requires{ typename Attr::sum; }
            auto sum() -> query_relation<std::unordered_map<typename Result::key_type, Attr::sum::attribute_type>, std::tuple<bind attributes...>>;


    .. cpp:function:: avg()

        .. code-block:: cpp

            /* result type is specialized from std::vector */
            template<is_attribute Attr>
            requires requires{ typename Attr::avg; }
            auto avg() -> query_relation<Attr::avg::attribute_type, std::tuple<bind attributes...>>;

            /* result type is specialized from std::unordered_map */
            template<is_attribute Attr>
            requires requires{ typename Attr::avg; }
            auto avg() -> query_relation<std::unordered_map<typename Result::key_type, Attr::avg::attribute_type>, std::tuple<bind attributes...>>;


    .. cpp:function:: max()

        .. code-block:: cpp

            /* result type is specialized from std::vector */
            template<is_attribute Attr>
            requires requires{ typename Attr::max; }
            auto max() -> query_relation<Attr::max::attribute_type, std::tuple<bind attributes...>>;

            /* result type is specialized from std::unordered_map */
            template<is_attribute Attr>
            requires requires{ typename Attr::max; }
            auto max() -> query_relation<std::unordered_map<typename Result::key_type, Attr::max::attribute_type>, std::tuple<bind attributes...>>;


    .. cpp:function:: min()

        .. code-block:: cpp

            /* result type is specialized from std::vector */
            template<is_attribute Attr>
            requires requires{ typename Attr::min; }
            auto min() -> query_relation<Attr::min::attribute_type, std::tuple<bind attributes...>>;

            /* result type is specialized from std::unordered_map */
            template<is_attribute Attr>
            requires requires{ typename Attr::min; }
            auto min() -> query_relation<std::unordered_map<typename Result::key_type, Attr::min::attribute_type>, std::tuple<bind attributes...>>;
