//
// Created by Kamsi Nwabueze on 8/2/26.
//

#include "atlas/matrix/Matrix.hpp"

#include <limits>
#include <stdexcept>

namespace {

    [[nodiscard]] std::size_t checked_element_count( std::size_t rows, std::size_t cols) {
        if (rows == 0 || cols == 0) {
            throw std::invalid_argument("Matrix dimensions must be nonzero");
        }
        constexpr std::size_t maximum = std::numeric_limits<std::size_t>::max();
        // or constexpr std::size_t maxiumum = static_cast<std::size_t>(-1) wrapp around
         if (rows > maximum / cols) {
             throw std::length_error("Matrix element count overflows size_t");
        }
        return rows * cols;

    }
}

namespace atlas {
    Matrix::Matrix(std::size_t rows, std::size_t cols, double initial_value) :
    rows_(rows),
    cols_(cols),
    data_(checked_element_count(rows,cols), initial_value) {}

    std::size_t Matrix::rows() const noexcept {
        return rows_;
    }
    std::size_t Matrix::cols() const noexcept {
        return cols_;
    }
    std::size_t Matrix::size() const noexcept {
        return data_.size();
    }
    double& Matrix::operator()(std::size_t row, std::size_t col)
    {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range("Matrix index out of range");
        }

        const std::size_t index = row * cols_ + col;
        return data_[index];
    }
    const double& Matrix::operator()(std::size_t row, std::size_t col) const
    {
        if (row >= rows_ || col >= cols_) {
            throw std::out_of_range("Matrix index out of range");
        }

        const std::size_t index = row * cols_ + col;
        return data_[index];
    }
    std::span<double> Matrix::data() noexcept
    {
        return std::span<double>{data_.data(), data_.size()};
    }
    std::span<const double> Matrix::data() const noexcept {
        return std::span<const double>{data_.data(), data_.size()};
    }


}



