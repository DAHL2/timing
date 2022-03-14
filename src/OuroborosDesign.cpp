/**
 * @file OuroborosDesign.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/OuroborosDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(OuroborosDesign)

//-----------------------------------------------------------------------------
OuroborosDesign::OuroborosDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MasterDesignInterface(node)
  , EndpointDesignInterface(node)
  , MasterDesign(node)
  , PlainEndpointDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
OuroborosDesign::~OuroborosDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
OuroborosDesign::get_status(bool print_out) const
{
  std::stringstream status;
  status << get_io_node_plain()->get_pll_status();
  status << get_master_node_plain()->get_status();
  status << this->get_endpoint_node(0).get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
OuroborosDesign::configure() const
{

  // Hard resets
  this->reset_io();

  // Set timestamp to current time
  this->sync_timestamp();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
OuroborosDesign::get_info(opmonlib::InfoCollector& ci, int level) const
{ 
  opmonlib::InfoCollector master_collector;
  this->get_master_node_plain()->get_info(master_collector, level);
  ci.add("master", master_collector);

  opmonlib::InfoCollector hardware_collector;
  this->get_io_node_plain()->get_info(hardware_collector, level);
  ci.add("io", hardware_collector);

  opmonlib::InfoCollector endpoint_collector;
  this->get_endpoint_node(0).get_info(endpoint_collector, level);
  ci.add("endpoint", endpoint_collector);
}
//-----------------------------------------------------------------------------
} // namespace dunedaq::timing  