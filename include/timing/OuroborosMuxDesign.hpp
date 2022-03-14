/**
 * @file OuroborosMuxDesign.hpp
 *
 * OuroborosMuxDesign is a class providing an interface
 * to the Ouroboros firmware design on a board with MUX.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_OUROBOROSMUXDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_OUROBOROSMUXDESIGN_HPP_

// PDT Headers
#include "timing/MasterMuxDesign.hpp"
#include "timing/EndpointDesignInterface.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for PDI ouroboros timing master design 
 */
class OuroborosMuxDesign
  : public MasterMuxDesign, public PlainEndpointDesignInterface
{
  UHAL_DERIVEDNODE(OuroborosMuxDesign)
public:
  explicit OuroborosMuxDesign(const uhal::Node& node);
  virtual ~OuroborosMuxDesign();

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

#endif // TIMING_INCLUDE_TIMING_OUROBOROSMUXDESIGN_HPP_