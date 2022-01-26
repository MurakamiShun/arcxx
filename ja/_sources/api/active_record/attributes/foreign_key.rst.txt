=======================================
active_record::attributes::foreign_key
=======================================

.. cpp:struct:: template<typename Model, typename Attribute, std::integral ForeignKey=uint32_t> \
                foreign_key

    Reference attribute.

    .. list-table:: Member functions

        * - :ref:`(constructor) <foreign_key_constructors>`
          - constructs the foreign_key attribute
        * - :cpp:func:`operator=`
          - 
        * - :ref:`(destructor) <foreign_key_destructors>`
          - destroy the attribute

    .. list-table:: Member variables

        * - :cpp:var:`data`
          - :code:`std::optional<ForeignKey::value_type>`

    .. list-table:: Member types

        * - :cpp:type:`model_type`
          - :code:`Model`
        * - :cpp:type:`attribute_type`
          - :code:`Attribute`
        * - :cpp:type:`value_type`
          - :code:`ForeignKey::value_type`
        * - :cpp:type:`foreign_key_type`
          - :code:`ForeignKey`
        * - :cpp:type:`constraint`
          - :code:`std::function<bool(const std::optional<ForeignKey::value_type>&)>`

    .. list-table:: Observers

        * - :cpp:func:`operator bool`
          - check whether null
        * - :cpp:func:`value`
          - returns the contained value
        
    .. list-table:: Condition Queries
        
        * - :cpp:func:`in`
          - generate in condition
        * - :cpp:func:`operator&&`
          - 
        * - :cpp:func:`operator||`
          -

    .. list-table:: Constraints

        * - :cpp:func:`default_value`
          - generate a default value constraint
        * - :cpp:var:`not_null`
          -
        * - :cpp:var:`unique`
          -
        * - :cpp:var:`primary_key`
          -

    .. list-table:: String mutual converts

        * - :cpp:func:`to_string`
          - converts to string
        * - :cpp:func:`from_string`
          - converts from string

    .. _foreign_key_constructors:
    .. cpp:function:: foreign_key()

        .. code-block:: cpp

            constexpr foreign_key();
            constexpr foreign_key(const std::optional<ForeignKey::value_type>&);
            constexpr foreign_key(const std::optional<ForeignKey::value_type>&&);
            constexpr foreign_key(std::nullopt_t);

            constexpr foreign_key(const ForeignKey::value_type&);
            constexpr foreign_key(const ForeignKey::value_type&&);

            constexpr foreign_key(const ForeignKey&);
        
    .. cpp:function:: operator=()
    
        .. code-block:: cpp

            Attribute& operator=(const Attribute&);
            Attribute& operator=(Attribute&&);

            Attribute& operator=(const std::optional<ForeignKey::value_type>&);
            Attribute& operator=(const std::optional<ForeignKey::value_type>&&);
            Attribute& operator=(std::nullopt_t);

            Attribute& operator=(const ForeignKey::value_type&);
            Attribute& operator=(const ForeignKey::value_type&&);

            Attribute& operator=(const ForeignKey&);

    .. _foreign_key_destructors:
    .. cpp:function:: ~foreign_key()
        
        .. code-block:: cpp

            constexpr virtual ~foreign_key();

    .. cpp:function:: operator bool()

        .. code-block:: cpp

            constexpr operator bool() const noexcept;
        
        Return false if attribute value is null.


    .. cpp:function:: value()

        .. code-block:: cpp

            [[nodiscard]] ForeignKey& value() &;
            [[nodiscard]] const ForeignKey& value() const&;
            [[nodiscard]] ForeignKey&& value() &&;

        Return attribute value.
        If the value is null, throw :code:`std::bad_optional_access`.

    .. cpp:function:: in()

        .. code-block:: cpp

            template<typename... Attrs>
            query_condition in(const Attrs&&... args);

        The returned object will generate
        :code:`Attribute::column_name IN (args...)`

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
        