/**
 * @file toolbox.hpp
 *
 * Timing utility functions
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_TOOLBOX_HPP_
#define TIMING_INCLUDE_TIMING_TOOLBOX_HPP_

#include "definitions.hpp"

#include "TimingIssues.hpp"
#include "timing/definitions/Structs.hpp"

#include "ers/Issue.hpp"
#include "logging/Logging.hpp"

// uHAL Headers
#include <uhal/Node.hpp>

// Boost Headers
#include <boost/format.hpp>
#include <boost/format/group.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/unordered_map.hpp>

// C++ Headers
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <istream>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

// Wrappers to be used by lexical_cast
template<typename T>
struct stol;
template<typename T>
struct stoul;

template<typename T>
std::string
to_string(const T&);

template<typename M>
bool
map_value_comparator(typename M::value_type& p1, typename M::value_type& p2);

template<typename T>
std::vector<T>
sanitize(const std::vector<T>& vec);

template<typename T, typename U>
T
safe_enum_cast(const U& value, const std::vector<T>& valid);

//! Walk & read the node structure.
Snapshot
snapshot(const uhal::Node& node);

//! Walk & read the sub-nodes whose IDs match this regex.
Snapshot
snapshot(const uhal::Node& node, const std::string& regex);

/**
 * Sleeps for a given number of milliseconds
 *
 * @param      time_in_milliseconds  Number of milliseconds to sleep
 */
void
millisleep(const double& time_in_milliseconds);

/**
 * Formats a std::string in printf fashion
 *
 * @param[in]      aFmt   Format string
 * @param[in]      ...   List of arguments
 *
 * @return     A formatted string
 */
std::string
strprintf(const char* fmt, ...); // NOLINT

/**
 * Expand the path
 * @param path path to expand
 * @return vector of expanded paths
 */
std::vector<std::string>
shell_expand_paths(const std::string& path);

/**
 * Performs variable subsitutition on path
 * @param path: Path to expand
 * @return Expanded path
 */
std::string
shellExpandPath(const std::string& path);

/**
 * Checks that the input path corresponds to an existing filesystem item which
 * is a file
 * @param path: Path to file
 */
void
throw_if_not_file(const std::string& path);

/**
 * @brief      Converts a vector of strings in a delimiter-separated string.
 *
 * @param[in]  aStrings    Collection of strings
 * @param[in]  aDelimiter  A delimiter
 *
 * @tparam     C           { description }
 *
 * @return     { description_of_the_return_value }
 */
template<typename C>
std::string
join(const C& strings, const std::string& delimiter = " ");

/**
 * Expand the path
 * @param path path to expand
 * @return vector of expanded paths
 */
uint8_t                                                 // NOLINT(build/unsigned)
dec_rng(uint8_t word, uint8_t ibit, uint8_t nbits = 1); // NOLINT(build/unsigned)

/**
 * ""
 * @return
 */
uint64_t                                             // NOLINT(build/unsigned)
tstamp2int(uhal::ValVector<uint32_t> raw_timestamp); // NOLINT(build/unsigned)

/**
 * Format reg-value
 * @return
 */
template<class T>
std::string
format_reg_value(T reg_value, uint32_t base = 16); // NOLINT(build/unsigned)

/**
 * @brief     Format reg-value table
 * @return
 */
template<class T>
std::string
format_reg_table(T data, std::string title = "", std::vector<std::string> headers = { "Register", "Value" });

/**
 * ""
 * @return
 */
int64_t
get_seconds_since_epoch();

/**
 * ""
 * @return
 */
std::string
format_timestamp(uhal::ValVector<uint32_t> raw_timestamp, uint32_t clock_frequency_hz); // NOLINT(build/unsigned)

/**
 * ""
 * @return
 */
std::string
format_timestamp(uint64_t raw_timestamp, uint32_t clock_frequency_hz); // NOLINT(build/unsigned)

/**
 * @brief     Format reg-value table
 * @return
 */
template<class T>
std::string
format_counters_table(std::vector<T> counters,
                      std::vector<std::string> counter_node_titles = {},
                      std::string table_title = "",
                      std::vector<std::string> counter_labels = {},
                      std::string counter_labels_header = "Cmd");

/**
 * ""
 * @return
 */
double
convert_bits_to_float(uint64_t bits, bool is_double_precision = false); // NOLINT(build/unsigned)

BoardType
convert_value_to_board_type(uint32_t Board_type); // NOLINT(build/unsigned)
CarrierType
convert_value_to_carrier_type(uint32_t darrier_type); // NOLINT(build/unsigned)
DesignType
convert_value_to_design_type(uint32_t design_type); // NOLINT(build/unsigned)

template<typename T>
std::string
vec_fmt(const std::vector<T>& vec);

template<typename T>
std::string
short_vec_fmt(const std::vector<T>& vec);

std::string
format_firmware_version(uint32_t firmware_version); // NOLINT(build/unsigned)

} // namespace timing
} // namespace dunedaq

#include "detail/toolbox.hxx"

#endif // TIMING_INCLUDE_TIMING_TOOLBOX_HPP_
