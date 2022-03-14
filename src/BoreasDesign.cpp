/**
 * @file BoreasDesign.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/BoreasDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(BoreasDesign)

//-----------------------------------------------------------------------------
BoreasDesign::BoreasDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MasterDesignInterface(node)
  , EndpointDesignInterface(node)
  , MasterDesign(node)
  , HSIDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
BoreasDesign::~BoreasDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
BoreasDesign::get_status(bool print_out) const
{
  std::stringstream status;
  status << get_io_node_plain()->get_pll_status();
  status << get_master_node_plain()->get_status();
  status << get_hsi_node().get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
BoreasDesign::configure() const
{

  // Hard resets
  reset_io();

  // Set timestamp to current time
  sync_timestamp();

  // configure hsi
  // get_his_node().
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
BoreasDesign::get_info(opmonlib::InfoCollector& ci, int level) const
{ 
  opmonlib::InfoCollector master_collector;
  get_master_node_plain()->get_info(master_collector, level);
  ci.add("master", master_collector);

  opmonlib::InfoCollector hardware_collector;
  get_io_node_plain()->get_info(hardware_collector, level);
  ci.add("io", hardware_collector);

  opmonlib::InfoCollector hsi_collector;
  get_hsi_node().get_info(hsi_collector, level);
  ci.add("hsi", hsi_collector);
}
//-----------------------------------------------------------------------------
} // namespace dunedaq::timing