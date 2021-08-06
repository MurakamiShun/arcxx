=============================
active_record::query_relation
=============================

.. cpp:struct:: template<typename Result, Tuple BindAttrs>\
                query_relation

    :code:`Result` is returned type of :cpp:func:`exec`.

    .. list-table:: Member functions

        * - :cpp:func:`exec`
          - 
        * - :cpp:func:`to_sql`
          - 

    .. list-table:: Generates query functions

        * - :cpp:func:`select`
          - requires Container<Result>
        * - :cpp:func:`pluck`
          - requires Container<Result>
        * - :cpp:func:`update`
          - requires Container<Result>
        * - :cpp:func:`where`
          - requires !std::same_as<Result, bool>
        * - :cpp:func:`limit`
          - requires Container<Result>
        * - :cpp:func:`order_by`
          - requires Container<Result>
        * - :cpp:func:`count`
          - requires Container<Result>
        * - :cpp:func:`sum`
          - requires Container<Result>
        * - :cpp:func:`avg`
          - requires Container<Result>
        * - :cpp:func:`max`
          - requires Container<Result>
        * - :cpp:func:`min`
          - requires Container<Result>

    .. cpp:function:: exec()

        .. code-block:: cpp

            template<std::derived_from<adaptor> Adaptor>
            [[nodiscard]] decltype(auto) exec(Adaptor& adapt) const

    .. cpp:function:: to_sql()

        .. code-block:: cpp

            template<std::derived_from<adaptor> Adaptor = common_adaptor>
            [[nodiscard]] const active_record::string to_sql() const 

    .. cpp:function:: where()

        .. code-block:: cpp

            template<Attribute Attr>
            [[nodiscard]] query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attr*>>), BindAttrs, std::tuple<const Attr*>>> where(const Attr&&) &&;
            template<Attribute Attr>
            [[nodiscard]] query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attr*>>), BindAttrs, std::tuple<const Attr*>>> where(const Attr&&) const &;

            template<Tuple SrcBindAttrs>
            [[nodiscard]] query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) &&;
            template<Tuple SrcBindAttrs>
            [[nodiscard]] query_relation<Result, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>> where(query_condition<SrcBindAttrs>&&) const&;



    .. cpp:function:: select()

        .. code-block:: cpp

            template<Attribute... Attrs>
            [[nodiscard]] query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> select() &&;
            template<Attribute... Attrs>
            [[nodiscard]] query_relation<std::vector<std::tuple<Attrs...>>, BindAttrs> select() const &;

    .. cpp:function:: pluck()

        .. code-block:: cpp

            template<Attribute Attr>
            [[nodiscard]] query_relation<std::vector<Attr>, BindAttrs> pluck() &&;
            template<Attribute Attr>
            [[nodiscard]] query_relation<std::vector<Attr>, BindAttrs> pluck() const &;

    .. cpp:function:: update()

        .. code-block:: cpp

            template<Attribute... Attrs>
            requires Model<typename Result::value_type>
            [[nodiscard]] query_relation<bool, std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, std::tuple<const Attrs*...>>), BindAttrs, std::tuple<const Attrs*...>>> update(const Attrs...);

    .. cpp:function:: limit()

        .. code-block:: cpp

            [[nodiscard]] query_relation<Result, BindAttrs>& limit(const std::size_t) &&;
            [[nodiscard]] query_relation<Result, BindAttrs> limit(const std::size_t) const &;


    .. cpp:function:: order_by()

        .. code-block:: cpp

            template<Attribute Attr>
            [[nodiscard]] query_relation<Result, BindAttrs>& order_by(const active_record::order = active_record::order::asc) &&;
            template<Attribute Attr>
            [[nodiscard]] query_relation<Result, BindAttrs> order_by(const active_record::order = active_record::order::asc) const &;


    .. cpp:function:: count()

        .. code-block:: cpp

            [[nodiscard]] query_relation<std::size_t, BindAttrs> count() &&;
            [[nodiscard]] query_relation<std::size_t, BindAttrs> count() const&;

    .. cpp:function:: sum()

        .. code-block:: cpp

            template<Attribute Attr>
            requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
            [[nodiscard]] query_relation<typename Attr::value_type, BindAttrs> sum() &&;
            template<Attribute Attr>
            requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
            [[nodiscard]] query_relation<typename Attr::value_type, BindAttrs> sum() const&;


    .. cpp:function:: avg()

        .. code-block:: cpp

            template<Attribute Attr>
            requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
            [[nodiscard]] query_relation<typename Attr::value_type, BindAttrs> avg() &&;
            template<Attribute Attr>
            requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
            [[nodiscard]] query_relation<typename Attr::value_type, BindAttrs> avg() const&;


    .. cpp:function:: max()

        .. code-block:: cpp

            template<Attribute Attr>
            requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
            [[nodiscard]] query_relation<typename Attr::value_type, BindAttrs> max() &&;
            template<Attribute Attr>
            requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
            [[nodiscard]] query_relation<typename Attr::value_type, BindAttrs> max() const&;


    .. cpp:function:: min()

        .. code-block:: cpp

            template<Attribute Attr>
            requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
            [[nodiscard]] query_relation<typename Attr::value_type, BindAttrs> min() &&;
            template<Attribute Attr>
            requires std::integral<typename Attr::value_type> || std::floating_point<typename Attr::value_type>
            [[nodiscard]] query_relation<typename Attr::value_type, BindAttrs> min() const&;