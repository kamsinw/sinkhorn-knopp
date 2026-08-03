#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace atlas {

class Matrix {
public:
    Matrix(std::size_t rows, std::size_t cols, double initial_value = 0.0);

    [[nodiscard]] std::size_t rows() const noexcept;
    [[nodiscard]] std::size_t cols() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;

    double& operator()(std::size_t row, std::size_t col);
    const double& operator()(std::size_t row, std::size_t col) const;

    [[nodiscard]] std::span<double> data() noexcept;
    [[nodiscard]] std::span<const double> data() const noexcept;

private:
    std::size_t rows_;
    std::size_t cols_;
    std::vector<double> data_;
};

}  // namespace atlas
