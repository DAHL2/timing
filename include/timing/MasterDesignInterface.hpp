/**
 * @file MasterDesignInterface.hpp
 *
 * MasterDesignInterface is a base class providing an interface
 * to for top level master firmware designs.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MASTERDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_MASTERDESIGNINTERFACE_HPP_

// PDT Headers
#include "timing/TopDesign.hpp"
#include "timing/MasterNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing master designs.
 */
class MasterDesignInterface : virtual public TopDesignInterface
{

public:
  explicit MasterDesignInterface(const uhal::Node& node)
  : TopDesignInterface(node)
    {}
  virtual ~MasterDesignInterface() {}

  /**
   * @brief      Read the current timestamp.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint64_t read_master_timestamp() const = 0; // NOLINT(build/unsigned)

  /**
   * @brief      Sync timestamp to current machine value.
   *
   */
  virtual void sync_timestamp() const = 0;
  
  /**
   * @brief      Measure the endpoint round trip time.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t measure_endpoint_rtt(uint32_t address, // NOLINT(build/unsigned)
                                        bool control_sfp = true,
                                        int sfp_mux = -1) const = 0;
  /**
   * @brief      Apply delay to endpoint
   */
  virtual void apply_endpoint_delay(uint32_t address,      // NOLINT(build/unsigned)
                                    uint32_t coarse_delay, // NOLINT(build/unsigned)
                                    uint32_t fine_delay,   // NOLINT(build/unsigned)
                                    uint32_t phase_delay,  // NOLINT(build/unsigned)
                                    bool measure_rtt = false,
                                    bool control_sfp = true,
                                    int sfp_mux = -1) const = 0;
  /**
   * @brief     Configure fake trigger generator
   */
  virtual void enable_periodic_fl_cmd(uint32_t channel, double rate, bool poisson = false) const = 0; // NOLINT(build/unsigned)

  /**
   * @brief      Get master node pointer
   */
  virtual const MasterNode* get_master_node_plain() const = 0;

  template<class MST>
  const MST* get_master_node() const
  { 
    return dynamic_cast<const MST*>(get_master_node_plain()); 
  }

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_MASTERDESIGNINTERFACE_HPP_