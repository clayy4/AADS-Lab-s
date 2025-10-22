#ifndef GRAYSCALE_HPP
#define GRAYSCALE_HPP

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "utils.hpp"

template <typename T>
class GrScImage {
private:
    size_t _rows, _cols;
    T _max_type;
    T** _data;

    T clamp_value(double value) const;

public:
    GrScImage(const size_t rows, const size_t cols, const bool is_fill) : _rows(rows), _cols(cols) {
        _data = new T * [_rows];
        for (size_t r = 0; r < _rows; r++) {
            _data[r] = new T[_cols];
        }

        if constexpr (std::is_same_v<T, bool>) {
            _max_type = 1;
        }
        else if constexpr (std::is_same_v<T, float>) {
            _max_type = 1.0f;
        }
        else if constexpr (std::is_same_v<T, char>) {
            _max_type = 127;
        }
        else if constexpr (std::is_same_v<T, short>) {
            _max_type = 32767;
        }
        else {
            throw std::logic_error("Undefined type");
        }

        Dice<T> dice(0, _max_type);

        if (is_fill) {
            for (size_t r = 0; r < _rows; r++) {
                for (size_t c = 0; c < _cols; c++) {
                    _data[r][c] = dice.roll();
                }
            }
        }
        else {
            for (size_t r = 0; r < _rows; r++) {
                for (size_t c = 0; c < _cols; c++) {
                    _data[r][c] = 0;
                }
            }
        }
    }

    ~GrScImage() {
        for (size_t r = 0; r < _rows; r++) {
            delete[] _data[r];
        }
        delete[] _data;
    }

    size_t get_rows() const;
    size_t get_cols() const;
    T& operator()(size_t row, size_t col) const;
    T& operator()(size_t row, size_t col);
    GrScImage<T>& operator!();
    bool operator==(const GrScImage<T>& other) const;
    GrScImage<T> operator*(const GrScImage<T>& other);
    GrScImage<T> operator*(const T num);
    GrScImage<T> operator+(const GrScImage<T>& other);
    GrScImage<T> operator+(const T num);

    double fillability();
    void draw_circle(size_t center_x, size_t center_y, size_t radius, T cell_value);

    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const GrScImage<U>& img);
};

template <typename T>
T GrScImage<T>::clamp_value(double value) const {
    if (value > static_cast<double>(_max_type)) return _max_type;
    if (value < 0) return 0;
    return static_cast<T>(value);
}

template <typename T>
size_t GrScImage<T>::get_rows() const {
    return _rows;
}

template <typename T>
size_t GrScImage<T>::get_cols() const {
    return _cols;
}

template <typename T>
T& GrScImage<T>::operator()(size_t row, size_t col) const {
    if (row >= _rows || col >= _cols) {
        throw std::out_of_range("Out of bounds");
    }
    return _data[row][col];
}

template <typename T>
T& GrScImage<T>::operator()(size_t row, size_t col) {
    if (row >= _rows || col >= _cols) {
        throw std::out_of_range("Out of bounds");
    }
    return _data[row][col];
}

template <typename T>
GrScImage<T>& GrScImage<T>::operator!() {
    for (int r = 0; r < _rows; r++) {
        for (int c = 0; c < _cols; c++) {
            _data[r][c] = _max_type - _data[r][c];
        }
    }
    return *this;
}

template <typename T>
bool GrScImage<T>::operator==(const GrScImage<T>& other) const{
    if ((_rows != other.get_rows()) || (_cols != other.get_cols())) {
        return 0;
    }

    for (int r = 0; r < _rows; r++) {
        for (int c = 0; c < _cols; c++) {
            if (other(r,c) != _data[r][c]) {
                return 0;
            }
        }
    }
    return 1;
}

template <typename T>
GrScImage<T> GrScImage<T>::operator*(const GrScImage<T>& other) {
    constexpr bool is_bool = std::is_same_v<T, bool>;

    size_t max_rows = _rows > other.get_rows() ? _rows : other.get_rows();
    size_t max_cols = _cols > other.get_cols() ? _cols : other.get_cols();

    GrScImage<T> res(max_rows, max_cols, 0);
    for (size_t r = 0; r < max_rows; r++) {
        for (size_t c = 0; c < max_cols; c++) {
            T val1 = (r < _rows && c < _cols) ? _data[r][c] : T{};
            T val2 = (r < other.get_rows() && c < other.get_cols()) ? other(r, c) : T{};

            if constexpr (is_bool) {
                res(r, c) = val1 && val2;
            }
            else {
                double result = static_cast<double>(val1) * static_cast<double>(val2);
                res(r, c) = clamp_value(result);
            }
        }
    }
    return res;
}

template <typename T>
GrScImage<T> GrScImage<T>::operator*(const T num) {
    constexpr bool is_bool = std::is_same_v<T, bool>;

    GrScImage<T> res(_rows, _cols, 0);

    for (size_t r = 0; r < _rows; r++) {
        for (size_t c = 0; c < _cols; c++) {
            if constexpr (is_bool) {
                res(r, c) = _data[r][c] && num;
            }
            else {
                double result = static_cast<double>(_data[r][c]) * static_cast<double>(num);
                res(r, c) = clamp_value(result);
            }
        }
    }
    return res;
}

template <typename T>
GrScImage<T> GrScImage<T>::operator+(const GrScImage<T>& other) {
    constexpr bool is_bool = std::is_same_v<T, bool>;

    size_t max_rows = _rows > other.get_rows() ? _rows : other.get_rows();
    size_t max_cols = _cols > other.get_cols() ? _cols : other.get_cols();

    GrScImage<T> res(max_rows, max_cols, 0);
    for (size_t r = 0; r < max_rows; r++) {
        for (size_t c = 0; c < max_cols; c++) {
            T val1 = (r < _rows && c < _cols) ? _data[r][c] : T{};
            T val2 = (r < other.get_rows() && c < other.get_cols()) ? other(r, c) : T{};

            if constexpr (is_bool) {
                res(r, c) = val1 || val2;
            }
            else {
                double result = static_cast<double>(val1) + static_cast<double>(val2);
                res(r, c) = clamp_value(result);
            }
        }
    }
    return res;
}

template <typename T>
GrScImage<T> GrScImage<T>::operator+(const T num) {
    constexpr bool is_bool = std::is_same_v<T, bool>;

    GrScImage<T> res(_rows, _cols, 0);

    for (size_t r = 0; r < _rows; r++) {
        for (size_t c = 0; c < _cols; c++) {
            if constexpr (is_bool) {
                res(r, c) = _data[r][c] || num;
            }
            else {
                double result = static_cast<double>(_data[r][c]) + static_cast<double>(num);
                res(r, c) = clamp_value(result);
            }
        }
    }
    return res;
}

template <typename T>
double GrScImage<T>::fillability() {
    double sum_cells = 0;

    for (size_t i = 0; i < _rows; i++) {
        for (size_t j = 0; j < _cols; j++) {
            sum_cells += _data[i][j];
        }
    }

    double res = (sum_cells) / (_rows * _cols * _max_type);
    return res;
}

template <typename T>
void GrScImage<T>::draw_circle(size_t center_x, size_t center_y, size_t radius, T cell_value) {
    long long radius_square = static_cast<long long>(radius) * static_cast<long long>(radius);
    for (size_t x = 0; x < _rows; x++) {
        for (size_t y = 0; y < _cols; y++) {
            double dx = static_cast<long long>(x) - static_cast<long long>(center_x);
            double dy = static_cast<long long>(y) - static_cast<long long>(center_y);

            double distance_square = (dx * dx) + (dy * dy);

            if (distance_square <= radius_square) {
                _data[x][y] = cell_value;
            }
        }
    }
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const GrScImage<T>& img) {
    for (size_t r = 0; r < img._rows; r++) {
        for (size_t c = 0; c < img._cols; c++) {
            os << img._data[r][c] << " ";
        }
        os << "\n";
    }
    return os;
}

#endif  // !GRAYSCALE_HPP