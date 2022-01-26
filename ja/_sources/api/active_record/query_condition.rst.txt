==============================
active_record::query_condition
==============================


.. cpp:struct:: template<specialized_from<std::tuple> BindAttrs>\
                query_condition

    .. cpp:function:: operator&&()

        .. code-block:: cpp

            template<specialized_from<std::tuple> SrcBindAttrs>
            auto operator&&(query_condition<DestBindAttrs>&& cond) &&  -> query_condition<std::tuple<bind attributes...>>;

    .. cpp:function:: operator||()

        .. code-block:: cpp

            template<specialized_from<std::tuple> SrcBindAttrs>
            auto operator||(query_condition<DestBindAttrs>&& cond) && -> query_condition<std::tuple<bind attributes...>>;