/**
 * @file BoreasDesign.hpp
 *
 * BoreasDesign is a base class providing an interface
 * to top level master with integrated HSI firmware designs.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_BOREASDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_BOREASDESIGN_HPP_

// Timing Headers
#include "timing/HSINode.hpp"
#include "timing/HSIDesignInterface.hpp"
#include "timing/MasterDesign.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for timing master with integrated HSI designs.
 */
class BoreasDesign : public MasterDesign, public HSIDesignInterface
{
  UHAL_DERIVEDNODE(BoreasDesign)
public:
  explicit BoreasDesign(const uhal::Node& node);
  virtual ~BoreasDesign();

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
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_BOREASDESIGN_HPP_