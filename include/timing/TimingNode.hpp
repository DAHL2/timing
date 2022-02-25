/**
 * @file TimingNode.hpp
 *
 * TimingNode is a base class for timing node
 * inteface classes.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_TIMINGNODE_HPP_
#define TIMING_INCLUDE_TIMING_TIMINGNODE_HPP_

// timing Headers
#include "TimingIssues.hpp"
#include "timing/definitions.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include "opmonlib/InfoCollector.hpp"
#include "ers/Issue.hpp"

// C++ Headers
#include <chrono>
#include <map>
#include <string>
#include <typeinfo>

namespace dunedaq {
namespace timing {
/**
 * @brief      Base class for timing nodes.
 */
class TimingNode : public uhal::Node
{

public:
  explicit TimingNode(const uhal::Node& node);
  virtual ~TimingNode();

  /**
   * @brief     Get the status string of the timing node. Optionally print it
   */
  virtual std::string get_status(bool print_out = false) const = 0;

  /**
   * @brief     Read subnodes.
   */
  std::map<std::string, uhal::ValWord<uint32_t>> read_sub_nodes(const uhal::Node& node, // NOLINT(build/unsigned)
                                                                bool dispatch = true) const;

  /**
   * @brief     Reset subnodes.
   */
  void reset_sub_nodes(const uhal::Node& node,
                       uint32_t aValue = 0x0, // NOLINT(build/unsigned)
                       bool dispatch = true) const;

  /**
   * @brief    Give info to collector.
   */
  virtual void get_info(opmonlib::InfoCollector&, int) const {}
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_TIMINGNODE_HPP_