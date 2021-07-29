===================================
namespace active_record::attributes
===================================

.. toctree::
   :maxdepth: 1
   :hidden:
   :glob:

   /api/attributes/*

Attributes
==========

.. list-table::

    * - :doc:`/api/attributes/string`
      - String attribute.

    * - :doc:`/api/attributes/integer`
      - Integer attribute. (default is `int32_t`)

    * - :doc:`/api/attributes/decimal`
      - Decimal attribute. (default is `float`)

    * - :doc:`/api/attributes/boolean`
      - boolean attribute.

    * - :doc:`/api/attributes/datetime`
      - Datetime attribute. The value type is `std::utc_clock`.

    * - :doc:`/api/attributes/binary`
      - Datetime attribute. The value type is `std::vector<std::byte>`.

    * - :doc:`/api/attributes/reference_to`
      - Relation attribute.