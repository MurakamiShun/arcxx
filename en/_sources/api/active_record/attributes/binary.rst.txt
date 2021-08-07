==================================
active_record::attributes::binary
==================================

.. cpp:struct:: template<typename Model, typename Attribute> \
                binary

    binary attribute.

    .. list-table:: Member functions

        * - :ref:`(constructor) <binary_constructors>`
          - constructs the binary attribute
        * - :cpp:func:`operator=`
          - 
        * - :ref:`(destructor) <binary_destructors>`
          - destroy the attribute

    .. list-table:: Member variables

        * - :cpp:var:`data`
          - :code:`std::optional<std::vector<std::byte>>`

    .. list-table:: Member types

        * - :cpp:type:`model_type`
          - :code:`Model`
        * - :cpp:type:`attribute_type`
          - :code:`Attribute`
        * - :cpp:type:`value_type`
          - :code:`std::vector<std::byte>`
        * - :cpp:type:`constraint`
          - :code:`std::function<bool(const std::optional<std::vector<std::byte>>&)>`

    .. list-table:: Observers

        * - :cpp:func:`operator bool`
          - check whether null
        * - :cpp:func:`value`
          - returns the contained value
        
    .. list-table:: Condition Queries
        
        * - :cpp:func:`in`
          - generate in condition
        * - :cpp:func:`between`
          - generate between condition
        * - :cpp:func:`operator&&`
          - 
        * - :cpp:func:`operator||`
          -

    .. list-table:: String mutual converts

        * - :cpp:func:`to_string`
          - converts to string
        * - :cpp:func:`from_string`
          - converts from string

    .. _binary_constructors:
    .. cpp:function:: binary()

        .. code-block:: cpp

            constexpr binary();
            constexpr binary(const std::optional<std::vector<std::byte>>&);
            constexpr binary(const std::optional<std::vector<std::byte>>&&);
            constexpr binary(std::nullopt_t);

            constexpr binary(const std::vector<std::byte>&);
            constexpr binary(const std::vector<std::byte>&&);
        
    .. cpp:function:: operator=()
    
        .. code-block:: cpp

            Attribute& operator=(const Attribute&);
            Attribute& operator=(Attribute&&);

            Attribute& operator=(const std::optional<std::vector<std::byte>>&);
            Attribute& operator=(const std::optional<std::vector<std::byte>>&&);
            Attribute& operator=(std::nullopt_t);

            Attribute& operator=(const std::vector<std::byte>&);
            Attribute& operator=(const std::vector<std::byte>&&);

    .. _binary_destructors:
    .. cpp:function:: ~binary()
        
        .. code-block:: cpp

            constexpr virtual ~binary();

    .. cpp:function:: operator bool()

        .. code-block:: cpp

            constexpr operator bool() const noexcept;
        
        Return false if attribute value is null.


    .. cpp:function:: value()

        .. code-block:: cpp

            [[nodiscard]] std::vector<std::byte>& value() &;
            [[nodiscard]] const std::vector<std::byte>& value() const&;
            [[nodiscard]] std::vector<std::byte>&& value() &&;

        Return attribute value.
        If the value is null, throw :code:`std::bad_optional_access`.

    .. cpp:function:: in()

        .. code-block:: cpp

            template<typename... Attrs>
            query_condition in(const Attrs&&... args);

        The returned object will generate
        :code:`Attribute::column_name IN (args...)`
    

    .. cpp:function:: between()
    
        .. code-block:: cpp

            query_condition between(std::convertible_to<std::vector<std::byte>> arg1, std::convertible_to<std::vector<std::byte>> arg2);

        The returned object will generate
        :code:`Attribute::column_name BETWEEN arg1 AND arg2`

    .. cpp:function:: operator&&()
    
        .. code-block:: cpp

            query_condition operator&&(const query_condition& condition); // (1)
            
            template<typename Arg>
            query_condition operator&&(const Arg& arg); // (2)

        The returned object will generate "AND condition SQL".
        
        .. code-block:: sql
            
            (Attribute::column_name = this AND condition) -- (1)
            (Attribute::column_name = this AND Attr::column_name = arg) -- (2)

    .. cpp:function:: operator||()
    
        .. code-block:: cpp

            query_condition operator||(const query_condition& condition); // (1)
            
            template<typename Arg>
            query_condition operator||(const Arg& arg); // (2)

        The returned object will generate "OR condition SQL".
        
        .. code-block:: sql
            
            (Attribute::column_name = this OR condition) -- (1)
            (Attribute::column_name = this OR Attr::column_name = arg) -- (2)

        
    .. cpp:function:: to_string()

        .. code-block:: cpp

            [[nodiscard]] active_record::string to_string() const;

        Converts attribute value to string.
        
    .. cpp:function:: from_string()

        .. code-block:: cpp

            void from_string(const active_record::string_view str);

        Converts string to attribute value.
        