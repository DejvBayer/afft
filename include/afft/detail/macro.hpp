#ifndef AFFT_DETAIL_MACRO_HPP
#define AFFT_DETAIL_MACRO_HPP

/// @brief Expand variadic arguments 256 times.
#define AFFT_DETAIL_EXPAND(...) \
  AFFT_DETAIL_EXPAND4(AFFT_DETAIL_EXPAND4(AFFT_DETAIL_EXPAND4(AFFT_DETAIL_EXPAND4(__VA_ARGS__))))
#define AFFT_DETAIL_EXPAND4(...) \
  AFFT_DETAIL_EXPAND3(AFFT_DETAIL_EXPAND3(AFFT_DETAIL_EXPAND3(AFFT_DETAIL_EXPAND3(__VA_ARGS__))))
#define AFFT_DETAIL_EXPAND3(...) \
  AFFT_DETAIL_EXPAND2(AFFT_DETAIL_EXPAND2(AFFT_DETAIL_EXPAND2(AFFT_DETAIL_EXPAND2(__VA_ARGS__))))
#define AFFT_DETAIL_EXPAND2(...) \
  AFFT_DETAIL_EXPAND1(AFFT_DETAIL_EXPAND1(AFFT_DETAIL_EXPAND1(AFFT_DETAIL_EXPAND1(__VA_ARGS__))))
#define AFFT_DETAIL_EXPAND1(...) \
  __VA_ARGS__

/// @brief Macro expanding to parentheses. Useful for expanding variadic arguments.
#define AFFT_DETAIL_PARENS  ()

/// @brief Empty delimiter macro implementation.
#define AFFT_DETAIL_DELIM_EMPTY()
/// @brief Comma delimiter macro implementation.
#define AFFT_DETAIL_DELIM_COMMA() ,

/// @brief For each macro with delimiter implementation.
#define AFFT_DETAIL_FOR_EACH_WITH_DELIM(macro, delimMacro, ...) \
  __VA_OPT__(AFFT_DETAIL_EXPAND(AFFT_DETAIL_FOR_EACH_WITH_DELIM_HELPER(macro, delimMacro, __VA_ARGS__)))
#define AFFT_DETAIL_FOR_EACH_WITH_DELIM_HELPER(macro, delimMacro, elem, ...) \
  macro(elem) \
  __VA_OPT__(delimMacro AFFT_DETAIL_PARENS \
             AFFT_DETAIL_FOR_EACH_WITH_DELIM_HELPER_AGAIN AFFT_DETAIL_PARENS (macro, delimMacro, __VA_ARGS__))
#define AFFT_DETAIL_FOR_EACH_WITH_DELIM_HELPER_AGAIN() AFFT_DETAIL_FOR_EACH_WITH_DELIM_HELPER

/// @brief Variadic bit-or macro implementation.
#define AFFT_DETAIL_BITOR(...) \
  (0 __VA_OPT__(| AFFT_DETAIL_EXPAND(AFFT_DETAIL_BITOR_HELPER(__VA_ARGS__))))
#define AFFT_DETAIL_BITOR_HELPER(elem, ...) \
  elem __VA_OPT__(| AFFT_DETAIL_BITOR_HELPER_AGAIN AFFT_DETAIL_PARENS (__VA_ARGS__))
#define AFFT_DETAIL_BITOR_HELPER_AGAIN() AFFT_DETAIL_BITOR_HELPER

#endif /* AFFT_DETAIL_MACRO_HPP */
