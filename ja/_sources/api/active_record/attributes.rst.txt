===================================
namespace active_record::attributes
===================================

.. toctree::
   :maxdepth: 1
   :hidden:
   :glob:

   /api/active_record/attributes/*

Attributes
==========

.. list-table::

    * - :doc:`attributes/string`
      - String attribute.

    * - :doc:`attributes/integer`
      - Integer attribute. (default value type is `int32_t`)

    * - :doc:`attributes/decimal`
      - Decimal attribute. (default value type is `double`)

    * - :doc:`attributes/boolean`
      - boolean attribute.

    * - :doc:`attributes/datetime`
      - Datetime attribute. The value type is `std::chrono::system_clock`.

    * - :doc:`attributes/binary`
      - Datetime attribute. The value type is `std::vector<std::byte>`.

    * - :doc:`attributes/foreign_key`
      - Foreign key attribute.