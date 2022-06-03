.. ARCXX documentation master file, created by
   sphinx-quickstart on Sat Jun 26 22:17:41 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

ARCXX Documentation
===============================

.. toctree::
   :maxdepth: 1
   :caption: Installation

   /installation/cmake

.. toctree::
   :maxdepth: 1
   :caption: Tutorial

   /tutorial/database_connection
   /tutorial/table_creation
   /tutorial/inserting
   /tutorial/find_columns
   /tutorial/select_columns
   /tutorial/aggregation
   /tutorial/group_by

API
===

.. toctree::
   :maxdepth: 1
   :caption: namespace
   :glob:

   /api/arcxx
   /api/arcxx/attributes
   /api/arcxx/sqlite3
   /api/arcxx/PostgreSQL

.. toctree::
   :maxdepth: 1
   :caption: class
   :glob:

   api/arcxx/model
   api/arcxx/query_condition
   api/arcxx/query_relation
   api/arcxx/attributes/*
   api/connectors/*

