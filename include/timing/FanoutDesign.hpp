/**
 * @file FanoutDesign.hpp
 *
 * FanoutDesign is a class providing an interface
 * to the fanout firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_FANOUTDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_FANOUTDESIGN_HPP_

// PDT Headers
#include "timing/MasterMuxDesign.hpp"
#include "timing/EndpointDesignInterface.hpp"
#include "timing/SwitchyardNode.hpp"

#include "TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for timing fanout designs.
 */
class FanoutDesign : public MasterMuxDesign, public PlainEndpointDesignInterface
{
  UHAL_DERIVEDNODE(FanoutDesign)
public:
  explicit FanoutDesign(const uhal::Node& node);
  virtual ~FanoutDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief    Give info to collector.
   */  
  void get_info(opmonlib::InfoCollector& ci, int level) const override;

  /**
   * @brief      Prepare the timing fanout for data taking.
   *
   */
  void configure() const override;

  ///**
  // * @brief      Reset timing fanout node.
  // */
  // void reset(const std::string& clock_config_file="") const override;

  /**
   * @brief      Reset timing fanout node.
   */
  void reset_io(int32_t fanout_mode, const std::string& clock_config_file = "") const override; // NOLINT(build/unsigned)

  /**
   * @brief      Measure the endpoint round trip time.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t measure_endpoint_rtt(uint32_t address, // NOLINT(build/unsigned)
                                        bool control_sfp = true,
                                        int sfp_mux = -1) const override;

  /**
   * @brief      Apply delay to endpoint
   */
  void apply_endpoint_delay(uint32_t address,      // NOLINT(build/unsigned)
                            uint32_t coarse_delay, // NOLINT(build/unsigned)
                            uint32_t fine_delay,   // NOLINT(build/unsigned)
                            uint32_t phase_delay,  // NOLINT(build/unsigned)
                            bool measure_rtt,
                            bool control_sfp,
                            int sfp_mux) const override;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_FANOUTDESIGN_HPP_