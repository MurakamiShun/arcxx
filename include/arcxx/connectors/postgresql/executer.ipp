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
        ~executer() = default;

        struct sentinel{};
        class iterator{
        private:
            ::PGresult* pg_result;
            ExecStatusType latest_result;
            const int pq_tuples_count;
            int pq_current_tuple;
            ResultType buffer;
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
        std::function<::PGresult* ()> exec_func;
    public:
        executer() = delete;
        executer(const executer&) = delete;
        executer(executer&&) noexcept;
        template<typename F>
        requires std::is_invocable_r_v<::PGresult*, F>
        executer(F&& f) noexcept;
        ~executer() = default;

        arcxx::expected<void, arcxx::string> execute();
    };

    template<typename ResultType>
    inline postgresql_connector::executer<ResultType>::executer(executer&& src) noexcept
        : exec_func(std::move(src.exec_func)){
    }

    template<typename ResultType>
    template<typename F>
    requires std::is_invocable_r_v<::PGresult*, F>
    inline postgresql_connector::executer<ResultType>::executer(F&& f) noexcept
        : exec_func(std::forward<F>(f)){
    }

    template<typename ResultType>
    inline postgresql_connector::executer<ResultType>::iterator postgresql_connector::executer<ResultType>::begin(){
        return iterator{exec_func()};
    }
    template<typename ResultType>
    inline postgresql_connector::executer<ResultType>::sentinel postgresql_connector::executer<ResultType>::end() const noexcept {
        return sentinel{};
    }

    inline postgresql_connector::executer<void>::executer(executer&& src) noexcept
        : exec_func(std::move(src.exec_func)){
    }
    template<typename F>
    requires std::is_invocable_r_v<::PGresult*, F>
    inline postgresql_connector::executer<void>::executer(F&& f) noexcept
        : exec_func(std::forward<F>(f)){
    }

    inline arcxx::expected<void, arcxx::string> postgresql_connector::executer<void>::execute(){
        ::PGresult* result = exec_func();
        if (const auto stat = PQresultStatus(result); stat == PGRES_COMMAND_OK || stat == PGRES_NONFATAL_ERROR){
            PQclear(result);
            return {};
        }
        else{
            arcxx::string error_message = PQresultErrorMessage(result);
            PQclear(result);
            return arcxx::make_unexpected(std::move(error_message));
        }
    }

    namespace detail{
        template<is_attribute Attr>
        inline bool set_column_data(PGresult* res, int col, int field, Attr& attr) {
            if(PQgetisnull(res, col, field)){
                attr = std::nullopt;
                return true;
            }
            const char* const value_text = PQgetvalue(res, col, field);
            if (value_text == nullptr) return false;
            from_string<postgresql_connector>(attr, arcxx::string_view{ value_text });
            return true;
        }
        template<typename T>
        requires (!is_attribute<T>)
        inline bool set_column_data(PGresult* res, int col, int field, T& result) {
            const char* const text_ptr = PQgetvalue(res, col, field);
            if (text_ptr == nullptr) return false;
            const auto value_text = arcxx::string{ text_ptr };
            if constexpr(std::is_same_v<std::remove_cvref_t<T>, bool>){
                int result_tmp = 0;
                std::from_chars(&(value_text.front()), &(value_text.back()) + 1, result_tmp);
                result = static_cast<bool>(result_tmp);
            }
            else{
                std::from_chars(&(value_text.front()), &(value_text.back()) + 1, result);
            }
            return true;
        }
    }

    template<typename ResultType>
    inline postgresql_connector::executer<ResultType>::iterator::iterator(::PGresult* pg_r)
    : pg_result{pg_r},
    latest_result{PQresultStatus(pg_r)},
    pq_tuples_count{[pg_r, res = this->latest_result]{
        if(pg_r == nullptr || res != PGRES_TUPLES_OK) return 1; // error message
        else return PQntuples(pg_r);
    }()},
    pq_current_tuple{0},
    buffer{}{
    }
    template<typename ResultType>
    inline postgresql_connector::executer<ResultType>::iterator::~iterator(){
        if(pg_result != nullptr) PQclear(pg_result);
    }
    template<typename ResultType>
    inline bool postgresql_connector::executer<ResultType>::iterator::operator==(sentinel){
        return pq_current_tuple >= pq_tuples_count;
    }
    template<typename ResultType>
    inline arcxx::expected<std::reference_wrapper<const ResultType>, arcxx::string> postgresql_connector::executer<ResultType>::iterator::operator*(){
        if(latest_result != PGRES_TUPLES_OK){
            return arcxx::make_unexpected(PQresultErrorMessage(pg_result));
        }
        else{
            if constexpr(is_model<ResultType>){
                tuptup::indexed_apply_each(
                    [this]<std::size_t N, typename Attr>(Attr& attr){ detail::set_column_data(this->pg_result, this->pq_current_tuple, N, attr); },
                    buffer.attributes_as_tuple()
                );
            }
            else if constexpr(specialized_from<ResultType, std::tuple>){
                tuptup::indexed_apply_each(
                    [this]<std::size_t N, typename Attr>(Attr& attr){ detail::set_column_data(this->pg_result, this->pq_current_tuple, N, attr); },
                    buffer
                );
            }
            else if constexpr(specialized_from<ResultType, std::pair>){
                // key
                detail::set_column_data(this->pg_result, this->pq_current_tuple, 0, buffer.first);
                // value
                if constexpr(specialized_from<decltype(buffer.second), std::tuple>){
                    tuptup::indexed_apply_each(
                        [this]<std::size_t N, typename Attr>(Attr& attr){ detail::set_column_data(this->pg_result, this->pq_current_tuple, N+1, attr); },
                        buffer.second
                    );
                }
                else{
                    detail::set_column_data(this->pg_result, this->pq_current_tuple, 1, buffer.second);
                }
            }
            else{
                detail::set_column_data(this->pg_result, this->pq_current_tuple, 0, buffer);
            }
            return std::cref(buffer);
        }
    }
    template<typename ResultType>
    inline postgresql_connector::executer<ResultType>::iterator& postgresql_connector::executer<ResultType>::iterator::operator++(){
        ++pq_current_tuple;
        return *this;
    }
}