/**
 * @file FLCmdGeneratorNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/FLCmdGeneratorNode.hpp"
#include "timing/toolbox.hpp"

#include "logging/Logging.hpp"

#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(FLCmdGeneratorNode)

//-----------------------------------------------------------------------------
FLCmdGeneratorNode::FLCmdGeneratorNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
FLCmdGeneratorNode::~FLCmdGeneratorNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
FLCmdGeneratorNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "FL Cmd gen state");
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::send_fl_cmd(FixedLengthCommandType command,
                                uint32_t channel, // NOLINT(build/unsigned)
                                const TimestampGeneratorNode& timestamp_gen_node) const
{
  getNode("sel").write(channel);

  reset_sub_nodes(getNode("chan_ctrl"));

  getNode("chan_ctrl.type").write(to_underlying(command));
  getNode("chan_ctrl.force").write(0x1);
  auto timestamp = timestamp_gen_node.read_raw_timestamp(false);

  getClient().dispatch();

  getNode("chan_ctrl.force").write(0x0);
  getClient().dispatch();
  TLOG() << "Command sent " << str(command) << "(" << format_reg_value(to_underlying(command)) << ") from generator "
         << format_reg_value(channel) << " @time " << std::hex << std::showbase << tstamp2int(timestamp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::enable_fake_trigger(uint32_t channel,  // NOLINT(build/unsigned)
                                        uint32_t divisor,  // NOLINT(build/unsigned)
                                        uint32_t prescale, // NOLINT(build/unsigned)
                                        bool poisson) const
{
  uint32_t trigger_cmd = 0x8 + channel; // NOLINT(build/unsigned)

  getNode("sel").write(channel);

  getNode("chan_ctrl.type").write(trigger_cmd);
  getNode("chan_ctrl.rate_div_d").write(divisor);
  getNode("chan_ctrl.rate_div_p").write(prescale);
  getNode("chan_ctrl.patt").write(poisson);
  getNode("chan_ctrl.en").write(1); // Start the command stream
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::disable_fake_trigger(uint32_t channel) const // NOLINT(build/unsigned)
{
  // Clear the internal trigger generator.
  getNode("sel").write(channel);
  reset_sub_nodes(getNode("chan_ctrl"));
  TLOG() << "Fake trigger generator " << format_reg_value(channel) << " configuration cleared";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string
FLCmdGeneratorNode::get_cmd_counters_table(bool print_out) const
{
  std::stringstream counters_table;
  auto accepted_counters = getNode("actrs").readBlock(getNode("actrs").getSize());
  auto rejected_counters = getNode("rctrs").readBlock(getNode("actrs").getSize());
  getClient().dispatch();

  std::vector<uhal::ValVector<uint32_t>> counters_container = { accepted_counters, rejected_counters }; // NOLINT(build/unsigned)

  counters_table << format_counters_table(counters_container,
                                          { "Accept counters", "Reject counters" },
                                          "Cmd gen counters",
                                          { "0x0", "0x1", "0x2", "0x3", "0x4" },
                                          "Chan");

  if (print_out)
    TLOG() << counters_table.str();
  return counters_table.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::get_info(opmonlib::InfoCollector& ic, int /*level*/) const
{
  auto accepted_counters = getNode("actrs").readBlock(getNode("actrs").getSize());
  auto rejected_counters = getNode("rctrs").readBlock(getNode("actrs").getSize());
  getClient().dispatch();

  uint number_of_channels = 5;

  for (uint i = 0; i < number_of_channels; ++i) { // NOLINT(build/unsigned)

    timingfirmwareinfo::TimingFLCmdCounter cmd_counter;
    opmonlib::InfoCollector cmd_counter_ic;

    cmd_counter.accepted = accepted_counters.at(i);
    cmd_counter.rejected = rejected_counters.at(i);

    std::string channel = "fl_cmd_channel_" + std::to_string(i);

    cmd_counter_ic.add(cmd_counter);
    ic.add(channel, cmd_counter_ic);
  }
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
