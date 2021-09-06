# Active Record C++ [![test](https://github.com/akisute514/active_record_cpp/actions/workflows/test.yml/badge.svg)](https://github.com/akisute514/active_record_cpp/actions/workflows/test.yml) [![doc_build](https://github.com/akisute514/active_record_cpp/actions/workflows/doc_build.yml/badge.svg)](https://github.com/akisute514/active_record_cpp/actions/workflows/doc_build.yml)

Active Record C++ is an Open Source C++20 O/R mapper of Active Record pattern.

Here is a simple Model definition example. There are no magic macros in this library.

```cpp
struct User : public active_record::model<User> {
    static constexpr auto table_name = "user_table";

    struct ID : public active_record::attributes::integer<User, ID, std::size_t> {
        static constexpr auto column_name = "id";
        using integer<User, ID, size_t>::integer;

        inline static const auto constraints = { primary_key };
    } id;

    struct Name : public active_record::attributes::string<User, Name> {
        static constexpr auto column_name = "name";
        using string<User, Name>::string;

        inline static const auto constraints = { default_value("unknow"), length(128) };
    } name;
};
```

The below example is how to create a user table and insert new data into a database.

```cpp
auto connector = active_record::sqlite3::adaptor::open("test.sqlite3");
if (connector.has_error()){
    // connection error handling
    std::cout << connector.error_message() << std::endl;
}

/*
 * CREATE TABLE members_table(
 *   id INTEGER PRIMARY KEY,
 *   name VARCHAR(128) DEFAULT 'unknown' NOT NULL
 * );
 */
const std::optional<std::string> error = connector.create_table<User>();
if (error){
    // print error
    std::cout << error.value() << std::endl;
}

const std::optional<std::string> error = User::insert(User{ .id=1, .name="foo" }).exec(connector);
if (error){
    // print inserting error
    std::cout << error.value() << std::endl;
}

connector.close();
```

To get data, there are no SQL statements.

```cpp
const auto [error, users] = User::where(User::ID::between(0,10)).exec(connector);
for(const auto& user : users){
    std::cout << "user.id : " << user.id.value();
    std::cout << " user.name : ";
    if(user.name) std::cout << user.name.value() << std::endl; 
    else          std::cout << "null" << std::endl;     
}
```


# Install

Have a look at the [Installation document](https://akisute514.github.io/active_record_cpp/en/installation/cmake.html) to build and install Active Record C++.

# Documentation

See below documentation. English is full documentation. Also, Japanese is available.

* [English](https://akisute514.github.io/active_record_cpp/en)
* [Japanese](https://akisute514.github.io/active_record_cpp/ja)

# Developing Status

Completed
* SQLite3 support
* PostgreSQL support

WIP
* Documentaion
* Translation of documents
* Tests
* Example codes

Waiting
* Public Relations


# License
This software is released under the Apache-2.0 License, see [LICENSE](LICENSE).

Active Record C++ depends on third party libraries. To use these subcomponents is subject to the terms and conditions of the following licenses.

* libsqlite3 ([https://www.sqlite.org/copyright.html](https://www.sqlite.org/copyright.html))
* libpq ([https://www.postgresql.org/about/licence/](https://www.postgresql.org/about/licence/))