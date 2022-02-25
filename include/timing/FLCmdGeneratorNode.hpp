/**
 * @file FLCmdGeneratorNode.hpp
 *
 * FLCmdGeneratorNode is a class providing an interface
 * to the fixed length command generator firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_FLCMDGENERATORNODE_HPP_
#define TIMING_INCLUDE_TIMING_FLCMDGENERATORNODE_HPP_

// PDT Headers
#include "timing/definitions.hpp"
#include "timing/TimestampGeneratorNode.hpp"
#include "timing/TimingNode.hpp"
#include "timing/timingfirmwareinfo/InfoStructs.hpp"
#include "timing/timingfirmwareinfo/InfoNljs.hpp"
#include "timing/definitions/Structs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include <nlohmann/json.hpp>

#include <string>

namespace dunedaq {
namespace timing {

typedef definitions::FixedLengthCommandType FixedLengthCommandType;

/**
 * @brief      Class for master global node.
 */
class FLCmdGeneratorNode : public TimingNode
{
  UHAL_DERIVEDNODE(FLCmdGeneratorNode)
public:
  explicit FLCmdGeneratorNode(const uhal::Node& node);
  virtual ~FLCmdGeneratorNode();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief     Send a fixed length command
   */
  void send_fl_cmd(FixedLengthCommandType command,
                   uint32_t channel, // NOLINT(build/unsigned)
                   const TimestampGeneratorNode& timestamp_gen_node) const;

  /**
   * @brief     Configure fake trigger
   */
  void enable_fake_trigger(uint32_t channel,  // NOLINT(build/unsigned)
                           uint32_t divisor,  // NOLINT(build/unsigned)
                           uint32_t prescale, // NOLINT(build/unsigned)
                           bool poisson) const;

  /**
   * @brief     Clear fake trigger configuration
   */
  void disable_fake_trigger(uint32_t channel) const; // NOLINT(build/unsigned)

  /**
   * @brief     Get command counters status string
   */
  std::string get_cmd_counters_table(bool print_out = false) const;

  /**
   * @brief    Give info to collector.
   */
  void get_info(opmonlib::InfoCollector& ic, int level) const override;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_FLCMDGENERATORNODE_HPP_