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
      - Integer attribute. (default is `int32_t`)

    * - :doc:`attributes/decimal`
      - Decimal attribute. (default is `float`)

    * - :doc:`attributes/boolean`
      - boolean attribute.

    * - :doc:`attributes/datetime`
      - Datetime attribute. The value type is `std::utc_clock`.

    * - :doc:`attributes/binary`
      - Datetime attribute. The value type is `std::vector<std::byte>`.

    * - :doc:`attributes/foreign_key`
      - Foreign key attribute.