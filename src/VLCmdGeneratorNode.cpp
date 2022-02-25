/**
 * @file VLCmdGeneratorNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/VLCmdGeneratorNode.hpp"
#include "timing/toolbox.hpp"

#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(VLCmdGeneratorNode)

//-----------------------------------------------------------------------------
VLCmdGeneratorNode::VLCmdGeneratorNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
VLCmdGeneratorNode::~VLCmdGeneratorNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
VLCmdGeneratorNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "VL Cmd gen state");

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
VLCmdGeneratorNode::switch_endpoint_sfp(uint32_t address, bool enable) const // NOLINT(build/unsigned)
{
  reset_sub_nodes(getNode("csr.ctrl"));
  getNode("csr.ctrl.addr").write(address);
  getNode("csr.ctrl.tx_en").write(enable);
  getNode("csr.ctrl.go").write(0x1);
  getNode("csr.ctrl.go").write(0x0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
VLCmdGeneratorNode::apply_endpoint_delay(uint32_t address,           // NOLINT(build/unsigned)
                                         uint32_t coarse_delay,      // NOLINT(build/unsigned)
                                         uint32_t fine_delay,        // NOLINT(build/unsigned)
                                         uint32_t phase_delay) const // NOLINT(build/unsigned)
{
  reset_sub_nodes(getNode("csr.ctrl"), false);
  getNode("csr.ctrl.tx_en").write(0x0);
  getNode("csr.ctrl.addr").write(address);
  getNode("csr.ctrl.cdel").write(coarse_delay);
  getNode("csr.ctrl.fdel").write(fine_delay);
  getNode("csr.ctrl.pdel").write(phase_delay);
  getNode("csr.ctrl.update").write(0x1);
  getNode("csr.ctrl.go").write(0x1);
  getNode("csr.ctrl.go").write(0x0);
  getClient().dispatch();

  TLOG_DEBUG(6) << "Coarse delay " << format_reg_value(coarse_delay) << " applied";
  TLOG_DEBUG(6) << "Fine delay   " << format_reg_value(fine_delay) << " applied";
  TLOG_DEBUG(6) << "Phase delay  " << format_reg_value(phase_delay) << " applied";
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
