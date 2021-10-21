/**
 * @file CRTDesign.hpp
 *
 * CRTDesign is a class providing an interface
 * to the top level CRT firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_CRTDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_CRTDESIGN_HPP_

// Timing Headers
#include "timing/CRTNode.hpp"
#include "timing/EndpointDesign.hpp"

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
template<class IO>
class CRTDesign : public EndpointDesign<IO>
{

public:
  explicit CRTDesign(const uhal::Node& node);
  virtual ~CRTDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  template<class T>
  void get_info(T& data) const;

  /**
   * @brief      Get the HSI node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual const CRTNode& get_crt_node() const { return uhal::Node::getNode<CRTNode>("endpoint0"); }

  /**
   * @brief      Read endpoint firmware version.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t read_firmware_version() const override {return 0;} // current crt firmware does not store firmware version

  /**
   * @brief      Validate endpoint firmware version.
   *
   */
  void validate_firmware_version() const override {} // current crt firmware does not store firmware version

  using TopDesign<IO>::get_io_node;
  
  // In leiu of UHAL_DERIVEDNODE
protected:
  virtual uhal::Node* clone() const;
  //
};

} // namespace timing
} // namespace dunedaq

#include "timing/detail/CRTDesign.hxx"

#endif // TIMING_INCLUDE_TIMING_CRTDesign_HPP_