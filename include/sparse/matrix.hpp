#pragma once
#include <map>
#include <tuple>
#include <type_traits>
#include <cstddef>

/**
 * @file matrix.hpp
 * @brief Бесконечная 2D разреженная матрица с Proxy (Заместитель).
 *
 * Хранит только занятые элементы; чтение пустой ячейки возвращает значение
 * по умолчанию Default. Присваивание Default освобождает ячейку.
 *
 * Особенности:
 *  - Индексация matrix[x][y] через прокси-объекты RowProxy / CellProxy.
 *  - Подсчёт занятых ячеек: size().
 *  - Итерация по занятым ячейкам : for (auto t : matrix) { tie(x,y,v)=t; }.
 *  - Каноническая форма operator= для CellProxy:
 *      ((matrix[100][100] = 314) = 0) = 217;
 */

namespace sparse {

template <class T, T Default, class Index = std::int64_t>
class Matrix {
    struct Key {
        Index x{};
        Index y{};
        bool operator<(const Key& rhs) const noexcept {
            return (x < rhs.x) || ((x == rhs.x) && (y < rhs.y));
        }
    };

    using Map = std::map<Key, T>;
    Map data_;

    // низкоуровневые операции хранения
    void set(Index x, Index y, const T& v) {
        if (v == Default) {
            auto it = data_.find({x,y});
            if (it != data_.end()) data_.erase(it);
        } else {
            data_[{x,y}] = v;
        }
    }
    const T* find(Index x, Index y) const noexcept {
        auto it = data_.find({x,y});
        return (it == data_.end()) ? nullptr : &it->second;
    }

public:
    // Proxy 1 строка 
    class RowProxy {
        Matrix& m_;
        Index   x_;
    public:
        RowProxy(Matrix& m, Index x) : m_(m), x_(x) {}

        // Proxy 2 конкретная ячейка
        class CellProxy {
            Matrix& m_;
            Index   x_;
            Index   y_;
        public:
            CellProxy(Matrix& m, Index x, Index y) : m_(m), x_(x), y_(y) {}

            // чтение implicit-конверсия в T. Не меняет матрицу
            operator T() const {
                if (const T* p = m_.find(x_, y_)) return *p;
                return Default;
            }

            // присваивание записать/освободить ячейку
            CellProxy& operator=(const T& v) {
                m_.set(x_, y_, v);
                return *this;
            }

            // разрешим и перемещающее присваивание для полноты
            CellProxy& operator=(T&& v) {
                m_.set(x_, y_, v);
                return *this;
            }
        };

        CellProxy operator[](Index y)             { return CellProxy(m_, x_, y); }
        const CellProxy operator[](Index y) const { return CellProxy(const_cast<Matrix&>(m_), x_, y); }
    };

    // индексация первая скобка по X, возвращаем строковый прокси
    RowProxy       operator[](Index x)       { return RowProxy(*this, x); }
    const RowProxy operator[](Index x) const { return RowProxy(const_cast<Matrix&>(*this), x); }

    // сколько реально занято
    std::size_t size() const noexcept { return data_.size(); }

    // итерация по занятым ячейкам
    class iterator {
        using Inner = typename Map::const_iterator;
        Inner it_;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = std::tuple<Index, Index, T>;
        using difference_type   = std::ptrdiff_t;

        iterator() = default;
        explicit iterator(Inner it) : it_(it) {}

        value_type operator*() const {
            return std::make_tuple(it_->first.x, it_->first.y, it_->second);
        }
        iterator& operator++()    { ++it_; return *this; }
        iterator  operator++(int) { iterator tmp(*this); ++(*this); return tmp; }
        bool operator==(const iterator& r) const { return it_ == r.it_; }
        bool operator!=(const iterator& r) const { return it_ != r.it_; }
    };

    iterator begin() const { return iterator(data_.cbegin()); }
    iterator end()   const { return iterator(data_.cend()); }
};

} // namespace sparse
