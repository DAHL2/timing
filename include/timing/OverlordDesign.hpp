/**
 * @file OverlordDesign.hpp
 *
 * OverlordDesign is a class providing an interface
 * to the Overlord firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_OVERLORDDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_OVERLORDDESIGN_HPP_

// PDT Headers
#include "timing/EndpointDesign.hpp"
#include "timing/FMCIONode.hpp"
#include "timing/OverlordDesignInterface.hpp"
#include "timing/MasterDesign.hpp"
#include "timing/PDIMasterNode.hpp"
#include "timing/SIMIONode.hpp"
#include "timing/TLUIONode.hpp"
#include "timing/TriggerReceiverNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include "uhal/Node.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for PDI timing master design (known as overlord).
 */
template<class IO>
class OverlordDesign
  : public OverlordDesignInterface, public MasterDesign<IO, PDIMasterNode>
{

public:
  explicit OverlordDesign(const uhal::Node& node);
  virtual ~OverlordDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Prepare the timing master for data taking.
   *
   */
  void configure() const override;

  /**
   * @brief    Give info to collector.
   */  
  void get_info(opmonlib::InfoCollector& ci, int level) const override;

  using TopDesign<IO>::get_io_node;
  using MasterDesign<IO, PDIMasterNode>::get_master_node;
  
  // In leiu of UHAL_DERIVEDNODE
protected:
  virtual uhal::Node* clone() const;
  //
};

} // namespace timing
} // namespace dunedaq

#include "timing/detail/OverlordDesign.hxx"

#endif // TIMING_INCLUDE_TIMING_OVERLORDDESIGN_HPP_