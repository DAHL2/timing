/**
 * @file DUNEEndpointNode.hpp
 *
 * DUNEEndpointNode is a class providing an interface
 * to the endpoint wrapper firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_ENDPOINTNODE_HPP_
#define TIMING_INCLUDE_TIMING_ENDPOINTNODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/FrequencyCounterNode.hpp"
#include "timing/EndpointNodeInterface.hpp"

#include "timing/timingendpointinfo/InfoNljs.hpp"
#include "timing/timingendpointinfo/InfoStructs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <nlohmann/json.hpp>

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing IO nodes.
 */
class DUNEEndpointNode : public EndpointNodeInterface
{
  UHAL_DERIVEDNODE(DUNEEndpointNode)
public:
  explicit DUNEEndpointNode(const uhal::Node& node);
  virtual ~DUNEEndpointNode();

  /**
   * @brief     Print the status of the timing node.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Enable the endpoint
   *
   * @return     { description_of_the_return_value }
   */
  void enable(uint32_t address = 0, uint32_t partition = 0) const override; // NOLINT(build/unsigned)
  
  /**
   * @brief      Disable the endpoint
   *
   * @return     { description_of_the_return_value }
   */
  void disable() const override;

  /**
   * @brief      Reset the endpoint
   *
   * @return     { description_of_the_return_value }
   */
  void reset(uint32_t address = 0, uint32_t partition = 0) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Read the current timestamp word.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint64_t read_timestamp() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the number of words in the data buffer.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_buffer_count() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the contents of the endpoint data buffer.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uhal::ValVector<uint32_t> read_data_buffer(bool read_all = false) const; // NOLINT(build/unsigned)

  /**
   * @brief      Print the contents of the endpoint data buffer.
   *
   * @return     { description_of_the_return_value }
   */
  virtual std::string get_data_buffer_table(bool read_all = false, bool print_out = false) const;

  /**
   * @brief      Read the endpoint clock frequency.
   *
   * @return     { description_of_the_return_value }
   */
  virtual double read_clock_frequency() const;

  /**
   * @brief      Read the endpoint wrapper version
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_version() const; // NOLINT(build/unsigned)

  /**
   * @brief     Collect monitoring information for timing endpoint
   *
   */
  virtual void get_info(timingendpointinfo::TimingEndpointInfo& mon_data) const;

  /**
   * @brief    Give info to collector.
   */
  void get_info(opmonlib::InfoCollector& ci, int level) const override;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_ENDPOINTNODE_HPP_