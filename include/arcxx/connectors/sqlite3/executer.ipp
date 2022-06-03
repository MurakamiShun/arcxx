#pragma once
/*
 * ARCXX: https://github.com/akisute514/arcxx
 * Copyright (c) 2021 akisute514
 * 
 * Released under the MIT License.
 */

namespace arcxx {
    template<typename ResultType>
    class sqlite3_connector::executer{
    private:
        ::sqlite3_stmt* stmt;
    public:
        executer() = delete;
        executer(const executer&) = delete;
        executer(executer&&);

        executer(::sqlite3_stmt*);
        ~executer();
        
        struct sentinel{};
        class iterator{
        private:
            int latest_status;
            ResultType buffer;
            ::sqlite3_stmt* stmt;

            void extract_column_data();
        public:
            iterator() = delete;
            iterator(const iterator&) = delete;
            iterator(iterator&&) = delete;
            iterator(::sqlite3_stmt*);
            ~iterator();
            bool operator==(sentinel);
            arcxx::expected<std::reference_wrapper<const ResultType>, arcxx::string> operator*();
            iterator& operator++();
        };

        iterator begin();
        sentinel end();
    };
    template<>
    class sqlite3_connector::executer<void>{
    private:
        ::sqlite3_stmt* stmt;
    public:
        executer() = delete;
        executer(const executer&) = delete;
        executer(executer&&);

        executer(::sqlite3_stmt*);
        ~executer();

        arcxx::expected<void, arcxx::string> execute();
    };

    template<typename ResultType>
    inline sqlite3_connector::executer<ResultType>::executer(::sqlite3_stmt* s){
        stmt = s;
    }
    template<typename ResultType>
    inline sqlite3_connector::executer<ResultType>::executer(executer<ResultType>&& src){
        stmt = src.stmt;
        src.stmt = nullptr;
    }
    template<typename ResultType>
    inline sqlite3_connector::executer<ResultType>::~executer(){
        sqlite3_finalize(stmt);
    }

    inline sqlite3_connector::executer<void>::executer(::sqlite3_stmt* s){
        stmt = s;
    }
    inline sqlite3_connector::executer<void>::executer(executer<void>&& src){
        stmt = src.stmt;
        src.stmt = nullptr;
    }
    inline sqlite3_connector::executer<void>::~executer(){
        sqlite3_finalize(stmt);
    }
    inline arcxx::expected<void, arcxx::string> sqlite3_connector::executer<void>::execute(){
        const auto status = sqlite3_step(stmt);
        if(status != SQLITE_DONE){
            return arcxx::make_unexpected(arcxx::string{ sqlite3_errmsg(sqlite3_db_handle(stmt)) });
        }
        else{
            return arcxx::expected<void, arcxx::string>{};
        }
    }


    template<typename ResultType>
    inline sqlite3_connector::executer<ResultType>::iterator sqlite3_connector::executer<ResultType>::begin(){
        return iterator{ stmt };
    }
    template<typename ResultType>
    inline sqlite3_connector::executer<ResultType>::sentinel sqlite3_connector::executer<ResultType>::end(){
        return sentinel{};
    }

    template<typename ResultType>
    inline sqlite3_connector::executer<ResultType>::iterator::iterator(::sqlite3_stmt* s){
        stmt = s;
        operator++();
    }
    template<typename ResultType>
    inline sqlite3_connector::executer<ResultType>::iterator::~iterator(){
        sqlite3_reset(stmt);
    }

    template<typename ResultType>
    inline bool sqlite3_connector::executer<ResultType>::iterator::operator==(sentinel){
        return latest_status == SQLITE_DONE;
    }
    template<typename ResultType>
    inline arcxx::expected<std::reference_wrapper<const ResultType>, arcxx::string> sqlite3_connector::executer<ResultType>::iterator::operator*(){
        if(latest_status == SQLITE_DONE) throw std::out_of_range{ "sqlite3_connector::executer::iterator" };
        else if(latest_status != SQLITE_ROW){
            return arcxx::make_unexpected(arcxx::string{ sqlite3_errmsg(sqlite3_db_handle(stmt)) });
        }
        else return buffer;
    }
    template<typename ResultType>
    inline sqlite3_connector::executer<ResultType>::iterator& sqlite3_connector::executer<ResultType>::iterator::operator++(){
        latest_status = sqlite3_step(stmt);

        if(latest_status == SQLITE_ROW) extract_column_data();

        return *this;
    }

    namespace sqlite3::detail{
        template<is_attribute Attr>
        requires std::same_as<typename Attr::value_type, arcxx::string>
        inline bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
            const auto type = sqlite3_column_type(stmt, static_cast<int>(idx));
            if(type == SQLITE_TEXT){
                auto text_ptr = sqlite3_column_text(stmt, static_cast<int>(idx));
                attr = arcxx::string{ reinterpret_cast<const arcxx::string::value_type*>(text_ptr) };
                return true;
            }
            else if(type == SQLITE_NULL){
                attr = std::nullopt;
                return true;
            }
            else{
                return false;
            }
        }
        template<is_attribute Attr>
        requires std::integral<typename Attr::value_type>
        inline bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
            const auto type = sqlite3_column_type(stmt, static_cast<int>(idx));
            if(type == SQLITE_INTEGER){
                if constexpr(sizeof(typename Attr::value_type) == sizeof(int64_t)){
                    attr = sqlite3_column_int64(stmt, static_cast<int>(idx));
                }
                else{
                    attr = sqlite3_column_int(stmt, static_cast<int>(idx));
                }
                return true;
            }
            else if(type == SQLITE_NULL){
                attr = std::nullopt;
                return true;
            }
            else{
                return false;
            }
        }
        template<is_attribute Attr>
        requires std::floating_point<typename Attr::value_type>
        inline bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
            const auto type = sqlite3_column_type(stmt, static_cast<int>(idx));
            if(type == SQLITE_FLOAT){
                attr = sqlite3_column_double(stmt, static_cast<int>(idx));
                return true;
            }
            else if(type == SQLITE_NULL){
                attr = std::nullopt;
                return true;
            }
            else{
                return false;
            }
        }
        template<is_attribute Attr>
        requires regarded_as_clock<typename Attr::value_type>
        inline bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
            const auto type = sqlite3_column_type(stmt, static_cast<int>(idx));
            if(type == SQLITE_TEXT){
                auto text_ptr = sqlite3_column_text(stmt, static_cast<int>(idx));
                from_string<sqlite3_connector>(attr, reinterpret_cast<const arcxx::string::value_type*>(text_ptr));
                return true;
            }
            else if(type == SQLITE_NULL){
                attr = std::nullopt;
                return true;
            }
            else{
                return false;
            }
        }
        template<is_attribute Attr>
        requires std::same_as<typename Attr::value_type, std::vector<std::byte>>
        inline bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, Attr& attr){
            const auto type = sqlite3_column_type(stmt, static_cast<int>(idx));
            if(type == SQLITE_BLOB){
                auto ptr = sqlite3_column_blob(stmt, static_cast<int>(idx));
                attr = std::vector<std::byte>{};
                const auto size = sqlite3_column_bytes(stmt, static_cast<int>(idx));
                attr.value().resize(size);
                std::copy_n(attr.value().begin(), ptr, size);
                return true;
            }
            else if(type == SQLITE_NULL){
                attr = std::nullopt;
                return true;
            }
            else{
                return false;
            }
        }
        template<typename T>
        requires (!is_attribute<T>)
        inline bool set_column_data(sqlite3_stmt* stmt, const std::size_t idx, T& attr){
            const auto type = sqlite3_column_type(stmt, static_cast<int>(idx));
            if constexpr(std::is_floating_point_v<T>){
                if(type == SQLITE_FLOAT) {
                    attr = sqlite3_column_double(stmt, static_cast<int>(idx));
                    return true;
                }
            }
            else if constexpr(std::is_integral_v<T>){
                if(type == SQLITE_INTEGER) {
                    if constexpr(sizeof(T) == sizeof(int64_t)){
                        attr = sqlite3_column_int64(stmt, static_cast<int>(idx));
                    }
                    else{
                        attr = sqlite3_column_int(stmt, static_cast<int>(idx));
                    }
                    return true;
                }
            }
            return false;
        }
    }

    template<typename ResultType>
    inline void sqlite3_connector::executer<ResultType>::iterator::extract_column_data(){
        if constexpr(is_model<ResultType>){
            tuptup::indexed_apply_each(
                [this]<std::size_t N, typename Attr>(Attr& attr){ sqlite3::detail::set_column_data(this->stmt, N, attr); },
                buffer.attributes_as_tuple()
            );
        }
        else if constexpr(specialized_from<ResultType, std::tuple>){
            tuptup::indexed_apply_each(
                [this]<std::size_t N, typename Attr>(Attr& attr){ sqlite3::detail::set_column_data(this->stmt, N, attr); },
                buffer
            );
        }
        else if constexpr(specialized_from<ResultType, std::pair>){
            // key
            sqlite3::detail::set_column_data(stmt, 0, buffer.first);
            // value
            if constexpr(specialized_from<decltype(buffer.second), std::tuple>){
                tuptup::indexed_apply_each(
                    [this]<std::size_t N, typename Attr>(Attr& attr){ sqlite3::detail::set_column_data(this->stmt, N+1, attr); },
                    buffer.second
                );
            }
            else{
                sqlite3::detail::set_column_data(stmt, 1, buffer.second);
            }
        }
        else{ // scalar
            sqlite3::detail::set_column_data(stmt, 0, buffer);
        }
    }
}