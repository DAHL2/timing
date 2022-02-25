/**
 * @file TimestampGeneratorNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/TimestampGeneratorNode.hpp"
#include "timing/toolbox.hpp"

#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(TimestampGeneratorNode)

//-----------------------------------------------------------------------------
TimestampGeneratorNode::TimestampGeneratorNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TimestampGeneratorNode::~TimestampGeneratorNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
TimestampGeneratorNode::get_status(bool print_out) const
{
  std::stringstream status;
  status << "Timestamp: 0x" << std::hex << read_timestamp() << std::endl;
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uhal::ValVector<uint32_t> // NOLINT(build/unsigned)
TimestampGeneratorNode::read_raw_timestamp(bool dispatch) const
{
  auto timestamp = getNode("ctr.val").readBlock(2);
  if (dispatch)
    getClient().dispatch();
  return timestamp;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
TimestampGeneratorNode::read_timestamp() const
{
  return tstamp2int(read_raw_timestamp());
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TimestampGeneratorNode::set_timestamp(uint64_t timestamp) const // NOLINT(build/unsigned)
{
  // Take the timestamp and split it up
  uint32_t now_high = (timestamp >> 32) & ((1UL << 32) - 1); // NOLINT(build/unsigned)
  uint32_t now_low = (timestamp >> 0) & ((1UL << 32) - 1);  // NOLINT(build/unsigned)
  getNode("ctr.set").writeBlock({ now_low, now_high });
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq