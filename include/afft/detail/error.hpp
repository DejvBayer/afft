/*
  This file is part of afft library.

  Copyright (c) 2024 David Bayer

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef AFFT_DETAIL_ERROR_HPP
#define AFFT_DETAIL_ERROR_HPP

#include <string>
#include <string_view>
#include <version>

#ifdef __cpp_lib_source_location
# include <source_location>
#endif

#include "utils.hpp"

namespace afft::detail
{
#if defined(AFFT_DEBUG) && defined(__cpp_lib_source_location)
  /**
   * @brief Creates an exception with source location information. Only available in debug mode when C++20
   *        source_location is supported.
   * @tparam E Exception type.
   * @param msg Message.
   * @param loc Source location.
   * @return Exception.
   */
  template<typename E>
  [[nodiscard]] E makeException(std::string_view msg, std::source_location loc = std::source_location::current())
  {
    static_assert(std::is_base_of_v<std::exception, E>, "E must be derived from std::exception");

    return E{cformat("%s (%s:" PRIuLEAST32 ")", msg.data(), loc.file_name(), loc.line())};
  }
#else
  /**
   * @brief Creates an exception.
   * @tparam E Exception type.
   * @param msg Message.
   * @return Exception.
   */
  template<typename E>
  [[nodiscard]] E makeException(std::string_view msg)
  {
    static_assert(std::is_base_of_v<std::exception, E>, "E must be derived from std::exception");

    return E{msg.data()};
  }
#endif

  /**
   * @struct Error
   * @brief Class for return value error checking.
   */
  struct Error
  {
# if defined(AFFT_DEBUG) && defined(__cpp_lib_source_location)
    /**
     * @brief Checks the return value and throws an exception if it is not OK. Only available in debug mode when C++20
     *        source_location is supported.
     * @tparam R Checked return type.
     * @param result Return value.
     * @param loc Source location.
     */
    template<typename R>
    static void check(R result, std::source_location loc = std::source_location::current())
    {
      if (!isOk(result))
      {
        throw makeException<std::runtime_error>(makeErrorMessage(result), loc);
      }
    }
# else
    /**
     * @brief Checks the return value and throws an exception if it is not OK.
     * @tparam R Checked return type.
     * @param result Return value.
     */
    template<typename R>
    static void check(R result)
    {
      if (!isOk(result))
      {
        throw makeException<std::runtime_error>(makeErrorMessage(result));
      }
    }
# endif

    /**
     * @brief Checks the return value. Should be implemented by each backend.
     * @tparam R Checked return type.
     * @param result Return value.
     * @return true if the return value is OK, false otherwise.
     */
    template<typename R>
    [[nodiscard]] static constexpr bool isOk(R result);

    /**
     * @brief Creates an error message. Should be implemented by each backend.
     * @tparam R Checked return type.
     * @param result Return value.
     * @return Error message.
     */
    template<typename R>
    [[nodiscard]] static std::string makeErrorMessage(R result);
  };

  /**
   * @brief Checks if a value is valid using a function. If not, it throws a invalid_argument.
   * @tparam isValidFn Function to check the value.
   * @param value Value to check.
   * @param msg Message to display in the exception.
   * @throws std::invalid_argument if the value is not valid.
   */
  template<auto isValidFn, typename T>
  constexpr void checkValid(const T& value, std::string_view msg = {})
  {
    static_assert(std::is_invocable_r_v<bool, decltype(isValidFn), T>, "isValidFn must return a bool");

    if (!isValidFn(value))
    {
      throw std::invalid_argument(msg.empty() ? "Invalid value" : msg.data());
    }
  }

  /**
   * @brief Checks if a span of values are valid using a function. If not, it throws a invalid_argument.
   * @tparam isValidFn Function to check the value.
   * @tparam T type of the values.
   * @param values Values to check.
   * @param msg Message to display in the exception.
   * @throws std::invalid_argument if any value is not valid.
   */
  template<auto isValidFn, typename T>
  constexpr void checkValid(Span<const T> values, std::string_view msg = {})
  {
    static_assert(std::is_invocable_r_v<bool, decltype(isValidFn), T>, "isValidFn must return a bool");

    for (const auto& value : values)
    {
      checkValid<isValidFn>(value, msg);
    }
  }
} // namespace afft::detail

#endif /* AFFT_DETAIL_ERROR_HPP */
