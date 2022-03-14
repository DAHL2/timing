#include "timing/MasterMuxDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
//template<class MST>
//uhal::Node*
//MasterMuxDesign::clone() const
//{
//  return new MasterMuxDesign(static_cast<const MasterMuxDesign&>(*this));
//}
////-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MasterMuxDesign::MasterMuxDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MuxDesignInterface(node)
  , MasterDesignInterface(node)
  , MasterDesign(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MasterMuxDesign::~MasterMuxDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MasterMuxDesign::get_status(bool print_out) const
{
  std::stringstream status;
  status << get_io_node_plain()->get_pll_status();
  status << get_master_node_plain()->get_status();
  // TODO mux specific status
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t
MasterMuxDesign::measure_endpoint_rtt(uint32_t address, bool control_sfp, int sfp_mux) const
{

  if (sfp_mux > 0) {
    if (control_sfp) {
      this->get_master_node_plain()->switch_endpoint_sfp(0x0, false);
      this->get_master_node_plain()->switch_endpoint_sfp(address, true);
    }

    // set fanout rtt mux channel, and do not wait for fanout rtt ept to be in a good state
    switch_sfp_mux_channel(sfp_mux, false);

    // sleep for a short time, otherwise the rtt endpoint will not get state to 0x8 in time
    millisleep(200);

    // gets master rtt ept in a good state, and sends echo command (due to second argument endpoint sfp is not controlled
    // in this call, already done above)
    uint32_t rtt = this->get_master_node_plain()->measure_endpoint_rtt(address, false);

    if (control_sfp)
      this->get_master_node_plain()->switch_endpoint_sfp(address, false);
    return rtt;
  } else {
    return this->get_master_node_plain()->measure_endpoint_rtt(address, control_sfp);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterMuxDesign::apply_endpoint_delay(uint32_t address,
                                               uint32_t coarse_delay,
                                               uint32_t fine_delay,
                                               uint32_t phase_delay,
                                               bool measure_rtt,
                                               bool control_sfp,
                                               int sfp_mux) const
{

  if (sfp_mux > 0) {
    if (measure_rtt) {
    if (control_sfp) {
      this->get_master_node_plain()->switch_endpoint_sfp(0x0, false);
      this->get_master_node_plain()->switch_endpoint_sfp(address, true);
    }

    // set fanout rtt mux channel, and wait for fanout rtt ept to be in a good state, don't bother waiting for a good
    // rtt endpoint, the next method call takes care of that
    switch_sfp_mux_channel(sfp_mux, false);
  }

  this->get_master_node_plain()->apply_endpoint_delay(address, coarse_delay, fine_delay, phase_delay, measure_rtt, false);

  if (measure_rtt && control_sfp)
    this->get_master_node_plain()->switch_endpoint_sfp(address, false);  
  } else {
    this->get_master_node_plain()->apply_endpoint_delay(address, coarse_delay, fine_delay, phase_delay, measure_rtt, control_sfp);
  }
  
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterMuxDesign::switch_sfp_mux_channel(uint32_t sfp_id, bool wait_for_rtt_ept_lock) const
{
  TopDesignInterface::get_io_node<timing::FanoutIONode>()->switch_sfp_mux_channel(sfp_id);
  if (wait_for_rtt_ept_lock) {
    this->get_master_node_plain()->enable_upstream_endpoint();
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<uint32_t>
MasterMuxDesign::scan_sfp_mux() const 
{
  std::vector<uint32_t> locked_channels;

  // TODO will this be right for every fanout board, need to check the IO board
  uint32_t number_of_mux_channels = 8;
  for (uint32_t i = 0; i < number_of_mux_channels; ++i) {
    TLOG_DEBUG(0) << "Scanning slot " << i;

    try {
      switch_sfp_mux_channel(i, true);
    } catch (...) {
      TLOG_DEBUG(0) << "Slot " << i << " not locked";
    }
    // TODO catch right except

    TLOG_DEBUG(0) << "Slot " << i << " locked";
    locked_channels.push_back(i);
  }

  if (locked_channels.size()) {
    TLOG() << "Slots locked: " << vec_fmt(locked_channels);
  } else {
    TLOG() << "No slots locked";
  }
  return locked_channels;
}
//-----------------------------------------------------------------------------
}