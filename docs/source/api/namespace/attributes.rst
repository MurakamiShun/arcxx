=========================
active_record::attributes
=========================

.. toctree::
   :maxdepth: 1
   :caption: Attributes
   :glob:

   attributes/*

.. list-table::

   * - `active_record::attributes::string`
     - String attribute.

   * - `active_record::attributes::integer`
     - Integer attribute. (default is `int32_t`)

   * - `active_record::attributes::decimal`
     - Decimal attribute. (default is `float`)

   * - `active_record::attributes::boolean`
     - boolean attribute.

   * - `active_record::attributes::datetime`
     - Datetime attribute. The value type is `std::utc_clock`.

   * - `active_record::attributes::binary`
     - Datetime attribute. The value type is `std::vector<std::byte>`.