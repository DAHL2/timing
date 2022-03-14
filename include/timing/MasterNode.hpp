/**
 * @file MasterNode.hpp
 *
 * MasterNode is a base class providing an interface
 * for the master type firmware blocks.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MASTERNODE_HPP_
#define TIMING_INCLUDE_TIMING_MASTERNODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/EchoMonitorNode.hpp"
#include "timing/FLCmdGeneratorNode.hpp"
#include "timing/TimestampGeneratorNode.hpp"
#include "timing/TimingNode.hpp"
#include "timing/VLCmdGeneratorNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing IO nodes.
 */
class MasterNode : public TimingNode
{
public:
  explicit MasterNode(const uhal::Node& node);
  virtual ~MasterNode();

  /**
   * @brief      Read the current timestamp word.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint64_t read_timestamp() const = 0; // NOLINT(build/unsigned)

  /**
   * @brief      Set the timestamp to current time.
   */
  virtual void set_timestamp(uint64_t timestamp) const = 0; // NOLINT(build/unsigned)

  /**
   * @brief     Set timestamp to current machine time
   */
  virtual void sync_timestamp(uint32_t clock_frequency_hz) const = 0; // NOLINT(build/unsigned)

  /**
   * @brief     Control the tx line of endpoint sfp
   */
  virtual void switch_endpoint_sfp(uint32_t address, bool turn_on) const = 0; // NOLINT(build/unsigned)

  /**
   * @brief     Enable RTT endpoint
   */
  virtual void enable_upstream_endpoint() const = 0;

  /**
   * @brief      Measure the endpoint round trip time.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t measure_endpoint_rtt(uint32_t address, bool control_sfp = true) const = 0; // NOLINT(build/unsigned)

  /**
   * @brief     Apply delay to endpoint
   */
  virtual void apply_endpoint_delay(uint32_t address,      // NOLINT(build/unsigned)
                                    uint32_t coarse_delay, // NOLINT(build/unsigned)
                                    uint32_t fine_delay,   // NOLINT(build/unsigned)
                                    uint32_t phase_delay,  // NOLINT(build/unsigned)
                                    bool measure_rtt = false,
                                    bool control_sfp = true) const = 0;

  /**
   * @brief     Apply delay to endpoint
   */
  virtual void apply_endpoint_delay(const ActiveEndpointConfig& ept_config, bool measure_rtt = false) const;

  /**
   * @brief     Send a fixed length command
   */
  virtual void send_fl_cmd(FixedLengthCommandType command,
                           uint32_t channel,                        // NOLINT(build/unsigned)
                           uint32_t number_of_commands = 1) const ; // NOLINT(build/unsigned)

  /**
   * @brief     Configure fake trigger generator
   */
  virtual void enable_periodic_fl_cmd(uint32_t channel, double rate, bool poisson, uint32_t clock_frequency_hz) const; // NOLINT(build/unsigned)

  /**
   * @brief     Clear fake trigger configuration
   */
  virtual void disable_periodic_fl_cmd(uint32_t channel) const; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_MASTERNODE_HPP_