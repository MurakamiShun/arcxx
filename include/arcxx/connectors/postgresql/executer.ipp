#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */

namespace arcxx {
    template<typename ResultType>
    class postgresql_connector::executer{
    private:
        std::function<::PGresult* ()> exec_func;
    public:
        executer() = delete;
        executer(const executer&) = delete;
        executer(executer&&) noexcept;
        template<typename F>
        requires std::is_invocable_r_v<::PGresult*, F>
        executer(F&& f) noexcept;
        ~executer();

        struct sentinel{};
        class iterator{
        private:
            ::ExecStatusType latest_status;
            ResultType buffer;
            ::PGresult* pg_result;

            void extract_column_data();
        public:
            iterator() = delete;
            iterator(const iterator&) = delete;
            iterator(iterator&&) = delete;
            iterator(::PGresult*);
            ~iterator();
            bool operator==(sentinel);
            arcxx::expected<std::reference_wrapper<const ResultType>, arcxx::string> operator*();
            iterator& operator++();
        };

        iterator begin();
        sentinel end() const noexcept;
    };
    template<>
    class postgresql_connector::executer<void>{
    private:
        std::function<PGresult* ()> exec_func;
    public:
        executer() = delete;
        executer(const executer&) = delete;
        executer(executer&&) noexcept;
        template<typename F>
        requires std::is_invocable_r_v<::PGresult*, F>
        executer(F&& f) noexcept;
        ~executer();

        arcxx::expected<void, arcxx::string> execute();
    };
}