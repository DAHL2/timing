/**
 * @file CRTNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/CRTNode.hpp"

#include "logging/Logging.hpp"

#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(CRTNode)

//-----------------------------------------------------------------------------
CRTNode::CRTNode(const uhal::Node& node)
  : EndpointNodeInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
CRTNode::~CRTNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
CRTNode::enable(uint32_t partition, uint32_t /*address*/) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.tgrp").write(partition);
  getNode("pulse.ctrl.en").write(0x1);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
CRTNode::enable(uint32_t partition, FixedLengthCommandType command) const // NOLINT(build/unsigned)
{

  getNode("csr.ctrl.tgrp").write(partition);
  getNode("pulse.ctrl.cmd").write(to_underlying(command));
  getNode("pulse.ctrl.en").write(0x1);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
CRTNode::disable() const
{
  getNode("pulse.ctrl.en").write(0x0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
CRTNode::reset(uint32_t partition, uint32_t address) const // NOLINT(build/unsigned)
{
  getNode("pulse.ctrl.en").write(0x0);
  enable(partition, address);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
CRTNode::reset(uint32_t partition, FixedLengthCommandType command) const // NOLINT(build/unsigned)
{
  getNode("pulse.ctrl.en").write(0x0);
  enable(partition, command);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
CRTNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto crt_registers = read_sub_nodes(getNode(""));
  status << format_reg_table(crt_registers, "CRT state", { "", "" }) << std::endl;

  const uint64_t last_pulse_timestamp =                                                       // NOLINT(build/unsigned)
    ((uint64_t)crt_registers.at("pulse.ts_h").value() << 32) + crt_registers.at("pulse.ts_l").value(); // NOLINT(build/unsigned)
  status << "Last Pulse Timestamp: 0x" << std::hex << last_pulse_timestamp << std::endl;

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
CRTNode::read_last_pulse_timestamp() const
{

  auto timestamp_reg_low = getNode("pulse.ts_l").read();
  auto timestamp_reg_high = getNode("pulse.ts_h").read();
  getClient().dispatch();

  return ((uint64_t)timestamp_reg_high.value() << 32) + timestamp_reg_low.value(); // NOLINT(build/unsigned)
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq