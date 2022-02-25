/**
 * @file FrequencyCounterNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/FrequencyCounterNode.hpp"
#include "timing/toolbox.hpp"
#include "logging/Logging.hpp"

#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(FrequencyCounterNode)

//-----------------------------------------------------------------------------
FrequencyCounterNode::FrequencyCounterNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
FrequencyCounterNode::~FrequencyCounterNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
FrequencyCounterNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto subnodes = read_sub_nodes(getNode("csr.ctrl"));
  status << format_reg_table(subnodes, "Freq counter state");
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<double>
FrequencyCounterNode::measure_frequencies(uint8_t number_of_clocks) const // NOLINT(build/unsigned)
{
  std::vector<double> frequencies;

  for (uint8_t i = 0; i < number_of_clocks; ++i) { // NOLINT(build/unsigned)
    getNode("ctrl.chan_sel").write(i);
    getNode("ctrl.en_crap_mode").write(0);
    getClient().dispatch();

    millisleep(2000);

    uhal::ValWord<uint32_t> frequency = getNode("freq.count").read();      // NOLINT(build/unsigned)
    uhal::ValWord<uint32_t> frequency_valid = getNode("freq.valid").read(); // NOLINT(build/unsigned)
    getClient().dispatch();

    if (frequency_valid.value()) {
      double freq = frequency.value() * 119.20928 / 1000000;
      frequencies.push_back(freq);
    } else {
      frequencies.push_back(-1);
    }
  }
  return frequencies;
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq