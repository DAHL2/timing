#include "timing/FanoutDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(FanoutDesign)

//-----------------------------------------------------------------------------
FanoutDesign::FanoutDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MuxDesignInterface(node)
  , MasterDesignInterface(node)
  , EndpointDesignInterface(node)
  , MasterMuxDesign(node)
  , PlainEndpointDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
FanoutDesign::~FanoutDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
FanoutDesign::get_status(bool print_out) const
{
  std::stringstream status;
  status << get_io_node_plain()->get_pll_status();
  status << MasterDesign::get_master_node_plain()->get_status();
  // TODO fanout specific status
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FanoutDesign::configure() const
{
  // fanout mode hard-coded, to be passed in as parameter in future
  uint32_t fanout_mode = 0;

  // Hard reset
  this->reset_io(fanout_mode);

  if (!fanout_mode) {
    // Set timestamp to current time
    this->sync_timestamp();
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FanoutDesign::reset_io(int32_t fanout_mode, const std::string& clock_config_file) const
{
  get_io_node_plain()->reset(fanout_mode, clock_config_file);
  // 0 - fanout mode, outgoing data comes from sfp
  // 1 - standalone mode, outgoing data comes from local master
  if (fanout_mode < 0) {
    TLOG() << "Fanout mode not supplied, defaulting to: 1 (standalone - built in master)";
    fanout_mode=1;
  }
  uhal::Node::getNode<SwitchyardNode>("switch").configure_master_source(fanout_mode);

  // temporary? 
  // TODO : discuss MIB firmware interface
  if (convert_value_to_board_type(get_io_node_plain()->read_board_type()) == kBoardMIB)
  {
    getNode("switch.csr.ctrl.amc_out").write(0xfff);
    getNode("switch.csr.ctrl.amc_in").write(0x0);
    getNode("switch.csr.ctrl.usfp_src").write(0x0);
    uhal::Node::getClient().dispatch();
  }
}
//-----------------------------------------------------------------------------

////-----------------------------------------------------------------------------
// template<class MST>
// void FanoutDesign::reset(const std::string& clock_config_file) const {
//	this->reset(0, clock_config_file);
//}
////-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
FanoutDesign::measure_endpoint_rtt(uint32_t address, bool control_sfp, int sfp_mux) const
{
  auto fanout_mode = uhal::Node::getNode("switch.csr.ctrl.master_src").read();
  uhal::Node::getClient().dispatch();

  if (!fanout_mode.value()) {
    std::ostringstream message;
    message << " Fanout unit " << uhal::Node::getId()
         << " is in fanout mode. Measure endpoint RTT should be called from master device.";
    throw UnsupportedFunction(ERS_HERE, message.str());
  }
  return MasterMuxDesign::measure_endpoint_rtt(address, control_sfp, sfp_mux);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FanoutDesign::apply_endpoint_delay(uint32_t address,
                                            uint32_t coarse_delay,
                                            uint32_t fine_delay,
                                            uint32_t phase_delay,
                                            bool measure_rtt,
                                            bool control_sfp,
                                            int sfp_mux) const
{
  auto fanout_mode = uhal::Node::getNode("switch.csr.ctrl.master_src").read();
  uhal::Node::getClient().dispatch();

  if (!fanout_mode.value()) {
    std::ostringstream message;
    message << "Fanout unit " << uhal::Node::getId()
         << " is in fanout mode. Apply endpoint delay should be called from master device.";
    throw UnsupportedFunction(ERS_HERE, message.str());
  }
  MasterMuxDesign::apply_endpoint_delay(
    address, coarse_delay, fine_delay, phase_delay, measure_rtt, control_sfp, sfp_mux);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FanoutDesign::get_info(opmonlib::InfoCollector& ci, int level) const
{ 
  opmonlib::InfoCollector master_collector;
  this->get_master_node_plain()->get_info(master_collector, level);
  ci.add("master", master_collector);

  opmonlib::InfoCollector hardware_collector;
  this->get_io_node_plain()->get_info(hardware_collector, level);
  ci.add("io", hardware_collector);
}
//-----------------------------------------------------------------------------

}