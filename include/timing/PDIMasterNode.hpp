/**
 * @file PDIMasterNode.hpp
 *
 * PDIMasterNode is a class providing an interface
 * to the PD-I master firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_PDIMASTERNODE_HPP_
#define TIMING_INCLUDE_TIMING_PDIMASTERNODE_HPP_

// PDT Headers
#include "timing/definitions.hpp"
#include "timing/FLCmdGeneratorNode.hpp"
#include "timing/MasterNode.hpp"
#include "timing/SpillInterfaceNode.hpp"

#include "timing/timingfirmwareinfo/InfoNljs.hpp"
#include "timing/timingfirmwareinfo/InfoStructs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for PD-I master timing nodes.
 */
class PDIMasterNode : public MasterNode
{
  UHAL_DERIVEDNODE(PDIMasterNode)
public:
  explicit PDIMasterNode(const uhal::Node& node);
  virtual ~PDIMasterNode();

  /**
   * @brief     Print the status of the timing node.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief     Print the status of the timing node.
   */
  std::string get_status_with_date(uint32_t clock_frequency_hz, bool print_out = false) const; // NOLINT(build/unsigned)

  /**
   * @brief     Control the tx line of endpoint sfp
   */
  void switch_endpoint_sfp(uint32_t address, bool turn_on) const override; // NOLINT(build/unsigned)

  /**
   * @brief     Enable RTT endpoint
   */
  void enable_upstream_endpoint() const override;

  /**
   * @brief      Measure the endpoint round trip time.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t measure_endpoint_rtt(uint32_t address, bool control_sfp = true) const override; // NOLINT(build/unsigned)

  /**
   * @brief     Apply delay to endpoint
   */
  void apply_endpoint_delay(uint32_t address,      // NOLINT(build/unsigned)
                            uint32_t coarse_delay, // NOLINT(build/unsigned)
                            uint32_t fine_delay,   // NOLINT(build/unsigned)
                            uint32_t phase_delay,  // NOLINT(build/unsigned)
                            bool measure_rtt = false,
                            bool control_sfp = true) const override;

  using MasterNode::apply_endpoint_delay;

  /**
   * @brief     Send a fixed length command
   */
  void send_fl_cmd(FixedLengthCommandType command,
                   uint32_t channel,                                // NOLINT(build/unsigned)
                   uint32_t number_of_commands = 1) const override; // NOLINT(build/unsigned)

  /**
   * @brief     Configure fake trigger generator
   */
  void enable_fake_trigger(uint32_t channel, double rate, bool poisson, uint32_t clock_frequency_hz) const; // NOLINT(build/unsigned)

  /**
   * @brief     Clear fake trigger configuration
   */
  void disable_fake_trigger(uint32_t channel) const; // NOLINT(build/unsigned)

  /**
   * @brief     Enable spill interface
   */
  void enable_spill_interface() const;

  /**
   * @brief     Configure and enable fake spill generator
   */
  void enable_fake_spills(uint32_t cycle_length = 16, uint32_t spill_length = 8) const; // NOLINT(build/unsigned)

  /**
   * @brief     Read whether we are in spill or not
   */
  bool read_in_spill() const;

  /**
   * @brief     Set timestamp to current machine time
   */
  void sync_timestamp(uint32_t clock_frequency_hz) const; // NOLINT(build/unsigned)

  /**
   * @brief     Fill the PD-I master monitoring structure.
   */
  void get_info(timingfirmwareinfo::PDIMasterMonitorData& mon_data) const;

  /**
   * @brief    Give info to collector.
   */
  void get_info(opmonlib::InfoCollector& ic, int level) const override;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_PDIMASTERNODE_HPP_