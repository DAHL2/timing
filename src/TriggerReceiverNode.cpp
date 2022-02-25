/**
 * @file TriggerReceiverNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/TriggerReceiverNode.hpp"
#include "timing/toolbox.hpp"

#include "logging/Logging.hpp"

#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(TriggerReceiverNode)

//-----------------------------------------------------------------------------
TriggerReceiverNode::TriggerReceiverNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TriggerReceiverNode::~TriggerReceiverNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
TriggerReceiverNode::get_status(bool print_out) const
{
  std::stringstream status;

  auto state = read_sub_nodes(getNode("csr.stat"), false);
  auto controls = read_sub_nodes(getNode("csr.ctrl"), false);
  auto counters = getNode("ctrs").readBlock(0x10);
  getClient().dispatch();

  status << format_reg_table(state, "Trigger rx state");
  status << format_reg_table(controls, "Trigger rx controls");

  std::vector<uhal::ValVector<uint32_t>> counters_container = { counters }; // NOLINT(build/unsigned)
  status << format_counters_table(counters_container, { "Counters" }, "Trig rx counters");

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TriggerReceiverNode::enable() const
{
  getNode("csr.ctrl.ep_en").write(0x1);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TriggerReceiverNode::disable() const
{
  getNode("csr.ctrl.ep_en").write(0x0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TriggerReceiverNode::reset() const
{
  getNode("csr.ctrl.ep_en").write(0x0);
  getNode("csr.ctrl.ep_en").write(0x1);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TriggerReceiverNode::enable_triggers() const
{
  getNode("csr.ctrl.ext_trig_en").write(0x1);
  getClient().dispatch();
}
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TriggerReceiverNode::disable_triggers() const
{
  getNode("csr.ctrl.ext_trig_en").write(0x0);
  getClient().dispatch();
}
//------------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq