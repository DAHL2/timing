/**
 * @file GlobalNode.hpp
 *
 * GlobalNode is a class providing an interface
 * to the master global firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_GLOBALNODE_HPP_
#define TIMING_INCLUDE_TIMING_GLOBALNODE_HPP_

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
class GlobalNode : public TimingNode
{
  UHAL_DERIVEDNODE(GlobalNode)
public:
  explicit GlobalNode(const uhal::Node& node);
  virtual ~GlobalNode();

  bool in_spill() const;
  bool tx_error() const;

  uint32_t readTimeStamp() const;      // NOLINT(build/unsigned)
  uint32_t read_spill_counter() const; // NOLINT(build/unsigned)

  void select_partition() const;
  void lock_partition() const;

  /**
   * @brief     Enable the upstream endpoint.
   */
  void enable_upstream_endpoint(uint32_t timeout = 500); // NOLINT(build/unsigned)

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_GLOBALNODE_HPP_