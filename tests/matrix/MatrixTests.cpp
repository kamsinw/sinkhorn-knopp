#include "atlas/matrix/Matrix.hpp"

#include <cstddef>
#include <exception>
#include <iostream>
#include <limits>
#include <span>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

namespace {

int failure_count = 0;

void expect(bool condition, std::string_view description)
{
    if (!condition) {
        std::cerr << "FAIL: " << description << '\n';
        ++failure_count;
    }
}

template <typename ExpectedException, typename Function>
void expect_throws(Function&& function, std::string_view description)
{
    try {
        std::forward<Function>(function)();
    } catch (const ExpectedException&) {
        return;
    } catch (const std::exception& exception) {
        std::cerr << "FAIL: " << description << " (unexpected exception: "
                  << exception.what() << ")\n";
        ++failure_count;
        return;
    } catch (...) {
        std::cerr << "FAIL: " << description << " (unexpected non-standard exception)\n";
        ++failure_count;
        return;
    }

    std::cerr << "FAIL: " << description << " (no exception thrown)\n";
    ++failure_count;
}

void test_construction_and_initialization()
{
    const atlas::Matrix zero_initialized(2, 3);
    expect(zero_initialized.rows() == 2, "constructor stores row count");
    expect(zero_initialized.cols() == 3, "constructor stores column count");
    expect(zero_initialized.size() == 6, "constructor allocates rows * cols elements");

    for (double value : zero_initialized.data()) {
        expect(value == 0.0, "default initial value is zero");
    }

    const atlas::Matrix initialized(2, 2, 3.5);
    for (double value : initialized.data()) {
        expect(value == 3.5, "custom initial value fills every element");
    }
}

void test_row_major_element_access()
{
    atlas::Matrix matrix(2, 3);
    matrix(0, 0) = 1.0;
    matrix(0, 1) = 2.0;
    matrix(0, 2) = 3.0;
    matrix(1, 0) = 4.0;
    matrix(1, 1) = 5.0;
    matrix(1, 2) = 6.0;

    const auto storage = matrix.data();
    for (std::size_t index = 0; index < storage.size(); ++index) {
        expect(storage[index] == static_cast<double>(index + 1),
               "operator() maps coordinates to row-major storage");
    }

    const atlas::Matrix& read_only = matrix;
    expect(read_only(0, 2) == 3.0, "const operator() reads an element");
    expect(read_only(1, 0) == 4.0, "const operator() preserves row-major mapping");
}

void test_span_access()
{
    atlas::Matrix matrix(2, 2, 1.0);
    auto storage = matrix.data();
    storage[2] = 9.0;

    expect(matrix(1, 0) == 9.0, "mutable span aliases matrix storage");

    const atlas::Matrix& read_only = matrix;
    const auto const_storage = read_only.data();
    expect(const_storage.size() == matrix.size(), "const span covers all elements");
    expect(const_storage[2] == 9.0, "const span reads matrix storage");
}

void test_invalid_dimensions_and_overflow()
{
    expect_throws<std::invalid_argument>(
        [] { (void)atlas::Matrix(0, 3); },
        "constructor rejects zero rows");
    expect_throws<std::invalid_argument>(
        [] { (void)atlas::Matrix(3, 0); },
        "constructor rejects zero columns");
    expect_throws<std::invalid_argument>(
        [] { (void)atlas::Matrix(0, 0); },
        "constructor rejects two zero dimensions");
    expect_throws<std::length_error>(
        [] {
            (void)atlas::Matrix(std::numeric_limits<std::size_t>::max(), 2);
        },
        "constructor rejects overflowing element count");
}

void test_bounds_checks()
{
    atlas::Matrix matrix(2, 3);
    const atlas::Matrix& read_only = matrix;

    expect_throws<std::out_of_range>(
        [&matrix] { (void)matrix(2, 0); },
        "mutable access rejects row equal to row count");
    expect_throws<std::out_of_range>(
        [&matrix] { (void)matrix(0, 3); },
        "mutable access rejects column equal to column count");
    expect_throws<std::out_of_range>(
        [&read_only] { (void)read_only(2, 0); },
        "const access checks row bounds");
    expect_throws<std::out_of_range>(
        [&read_only] { (void)read_only(0, 3); },
        "const access checks column bounds");
}

void test_value_semantics()
{
    atlas::Matrix original(1, 2, 1.0);
    atlas::Matrix copy = original;
    copy(0, 0) = 8.0;

    expect(original(0, 0) == 1.0, "copy owns independent storage");
    expect(copy(0, 0) == 8.0, "copy can mutate its own storage");

    atlas::Matrix moved_to = std::move(copy);
    expect(moved_to.rows() == 1, "move preserves row count in destination");
    expect(moved_to.cols() == 2, "move preserves column count in destination");
    expect(moved_to(0, 0) == 8.0, "move transfers element storage to destination");
}

static_assert(std::is_same_v<
              decltype(std::declval<atlas::Matrix&>()(0, 0)),
              double&>);
static_assert(std::is_same_v<
              decltype(std::declval<const atlas::Matrix&>()(0, 0)),
              const double&>);
static_assert(std::is_same_v<
              decltype(std::declval<atlas::Matrix&>().data()),
              std::span<double>>);
static_assert(std::is_same_v<
              decltype(std::declval<const atlas::Matrix&>().data()),
              std::span<const double>>);

}  // namespace

int main()
{
    test_construction_and_initialization();
    test_row_major_element_access();
    test_span_access();
    test_invalid_dimensions_and_overflow();
    test_bounds_checks();
    test_value_semantics();

    if (failure_count != 0) {
        std::cerr << failure_count << " matrix test(s) failed\n";
        return 1;
    }

    std::cout << "All matrix tests passed\n";
    return 0;
}
