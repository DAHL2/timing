/**
 * @file EchoMonitorNode.hpp
 *
 * EchoMonitorNode is a class providing an interface
 * to the echo monitor firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_ECHOMONITORNODE_HPP_
#define TIMING_INCLUDE_TIMING_ECHOMONITORNODE_HPP_

// PDT Headers
#include "timing/TimingNode.hpp"
#include "timing/toolbox.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for master global node.
 */
class EchoMonitorNode : public TimingNode
{
  UHAL_DERIVEDNODE(EchoMonitorNode)
public:
  explicit EchoMonitorNode(const uhal::Node& node);
  virtual ~EchoMonitorNode();

  /**
   * @brief      Send echo and measure the round-trip time between master and endpoint
   *
   * @return     { description_of_the_return_value }
   */
  uint64_t send_echo_and_measure_delay(int64_t timeout = 500) const; // NOLINT(build/unsigned)

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_ECHOMONITORNODE_HPP_