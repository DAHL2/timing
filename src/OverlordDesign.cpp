/**
 * @file OverlordDesign.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/OverlordDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(OverlordDesign)

//-----------------------------------------------------------------------------
OverlordDesign::OverlordDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MasterDesignInterface(node)
  , EndpointDesignInterface(node)
  , MasterDesign(node)
  , OverlordDesignInterface(node)
  , PlainEndpointDesignInterface(node)

{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
OverlordDesign::~OverlordDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
OverlordDesign::get_status(bool print_out) const
{
  std::stringstream status;
  status << get_io_node_plain()->get_pll_status();
  status << get_master_node_plain()->get_status();
  status << get_external_triggers_endpoint_node().get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
OverlordDesign::configure() const
{

  // Hard resets
  reset_io();

  // Set timestamp to current time
  sync_timestamp();

  // Enable spill interface
  get_master_node<PDIMasterNode>()->enable_spill_interface();

  // Trigger interface configuration
  reset_external_triggers_endpoint();
  enable_external_triggers();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
OverlordDesign::get_info(opmonlib::InfoCollector& ci, int level) const
{ 
  opmonlib::InfoCollector master_collector;
  get_master_node_plain()->get_info(master_collector, level);
  ci.add("master", master_collector);

  opmonlib::InfoCollector hardware_collector;
  get_io_node_plain()->get_info(hardware_collector, level);
  ci.add("io", hardware_collector);

  // TODO full trix info
  //auto trig_interface_enabled = uhal::Node::getNode("trig_rx.csr.ctrl.ext_trig_en").read();
  //uhal::Node::getClient().dispatch();
  //data.trig_interface_enabled = trig_interface_enabled.value();
}
//-----------------------------------------------------------------------------

} // namespace dunedaq::timing  