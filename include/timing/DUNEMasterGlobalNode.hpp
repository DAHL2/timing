/**
 * @file DUNEMasterGlobalNode.hpp
 *
 * DUNEMasterGlobalNode is a class providing an interface
 * to the master global firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_DUNEMASTERGLOBALNODE_HPP_
#define TIMING_INCLUDE_TIMING_DUNEMASTERGLOBALNODE_HPP_

// PDT Headers
#include "timing/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for master global node.
 */
class DUNEMasterGlobalNode : public TimingNode
{
  UHAL_DERIVEDNODE(DUNEMasterGlobalNode)
public:
  explicit DUNEMasterGlobalNode(const uhal::Node& node);
  virtual ~DUNEMasterGlobalNode();
  
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

#endif // TIMING_INCLUDE_TIMING_DUNEMASTERGLOBALNODE_HPP_