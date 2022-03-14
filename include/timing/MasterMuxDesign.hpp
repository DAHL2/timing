/**
 * @file MasterMuxDesign.hpp
 *
 * MasterMuxDesign is a class providing an interface
 * to the Ouroboros firmware design on a board with MUX.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MASTERMUXDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_MASTERMUXDESIGN_HPP_

// PDT Headers
#include "timing/MuxDesignInterface.hpp"
#include "timing/MasterDesign.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for PDI timing master design on mux board
 */
class MasterMuxDesign
  : virtual public MuxDesignInterface, public MasterDesign
{

public:
  explicit MasterMuxDesign(const uhal::Node& node);
  virtual ~MasterMuxDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;


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
                                    bool measure_rtt = false,
                                    bool control_sfp = true,
                                    int sfp_mux = -1) const override; 

  /**
   * @brief     Switch the SFP mux channel
   */
  void switch_sfp_mux_channel(uint32_t sfp_id, bool wait_for_rtt_ept_lock) const override; // NOLINT(build/unsigned)

  /**
   * @brief     Scan SFP for alive timing transmitters
   */
  std::vector<uint32_t> scan_sfp_mux() const override; // NOLINT(build/unsigned)
    
  // In leiu of UHAL_DERIVEDNODE
protected:
 // virtual uhal::Node* clone() const;
  
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_MASTERMUXDESIGN_HPP_