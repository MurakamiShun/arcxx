==============================
active_record::query_condition
==============================


.. cpp:struct:: template<Tuple BindAttrs>\
                query_condition

    .. cpp:function:: operator&&()

        .. code-block:: cpp

            template<Tuple SrcBindAttrs>
            query_condition<std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>>
            operator&&(query_condition<DestBindAttrs>&& cond) &&;

    .. cpp:function:: operator||()

        .. code-block:: cpp

            template<Tuple SrcBindAttrs>
            query_condition<std::invoke_result_t<decltype(std::tuple_cat<BindAttrs, SrcBindAttrs>), BindAttrs, SrcBindAttrs>>
            operator||(query_condition<DestBindAttrs>&& cond) &&;