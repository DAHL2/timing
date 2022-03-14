/**
 * @file DUNEMasterNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/DUNEMasterNode.hpp"
#include "timing/DUNEMasterGlobalNode.hpp"

#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(DUNEMasterNode)

//-----------------------------------------------------------------------------
DUNEMasterNode::DUNEMasterNode(const uhal::Node& node)
  : MasterNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
DUNEMasterNode::~DUNEMasterNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
DUNEMasterNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto raw_timestamp = getNode<TimestampGeneratorNode>("tstamp").read_raw_timestamp();
  status << "Timestamp: 0x" << std::hex << tstamp2int(raw_timestamp) << std::endl << std::endl;
  status << getNode<FLCmdGeneratorNode>("scmd_gen").get_cmd_counters_table();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
DUNEMasterNode::get_status_with_date(uint32_t clock_frequency_hz, bool print_out) const // NOLINT(build/unsigned)
{
  std::stringstream status;
  auto raw_timestamp = getNode<TimestampGeneratorNode>("tstamp").read_raw_timestamp();
  status << "Timestamp: 0x" << std::hex << tstamp2int(raw_timestamp) << " -> " << format_timestamp(raw_timestamp, clock_frequency_hz) << std::endl
          << std::endl;
  status << getNode<FLCmdGeneratorNode>("scmd_gen").get_cmd_counters_table();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
DUNEMasterNode::switch_endpoint_sfp(uint32_t address, bool turn_on) const // NOLINT(build/unsigned)
{

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
DUNEMasterNode::enable_upstream_endpoint() const
{
  auto global = getNode<DUNEMasterGlobalNode>("global");
  global.enable_upstream_endpoint();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t                                                                      // NOLINT(build/unsigned)
DUNEMasterNode::measure_endpoint_rtt(uint32_t address, bool control_sfp) const // NOLINT(build/unsigned)
{

  auto vl_cmd_node = getNode<VLCmdGeneratorNode>("acmd");
  auto global = getNode<DUNEMasterGlobalNode>("global");
  auto echo = getNode<EchoMonitorNode>("echo");

  if (control_sfp)
  {
    // Switch off all TX SFPs
    vl_cmd_node.switch_endpoint_sfp(0x0, false);

    // Turn on the current target
    vl_cmd_node.switch_endpoint_sfp(address, true);

    millisleep(100);
  }

  try
  {
    global.enable_upstream_endpoint();
  }
  catch (const timing::UpstreamEndpointFailedToLock& e)
  {
    if (control_sfp) {
      vl_cmd_node.switch_endpoint_sfp(address, false);
    }
    throw e;
  }
  uint32_t endpoint_rtt = echo.send_echo_and_measure_delay(); // NOLINT(build/unsigned)

  if (control_sfp)
    vl_cmd_node.switch_endpoint_sfp(address, false);

  return endpoint_rtt;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
DUNEMasterNode::apply_endpoint_delay(uint32_t address,      // NOLINT(build/unsigned)
                                    uint32_t coarse_delay, // NOLINT(build/unsigned)
                                    uint32_t fine_delay,   // NOLINT(build/unsigned)
                                    uint32_t phase_delay,  // NOLINT(build/unsigned)
                                    bool measure_rtt,
                                    bool control_sfp) const
{

  auto vl_cmd_node = getNode<VLCmdGeneratorNode>("acmd");
  auto global = getNode<DUNEMasterGlobalNode>("global");
  auto echo = getNode<EchoMonitorNode>("echo");

  if (measure_rtt) {

    if (control_sfp) {
      // Switch off all TX SFPs
      vl_cmd_node.switch_endpoint_sfp(0x0, false);

      // Turn on the current target
      vl_cmd_node.switch_endpoint_sfp(address, true);

      millisleep(100);
    }

    try
    {
      global.enable_upstream_endpoint();
    }
    catch (const timing::UpstreamEndpointFailedToLock& e)
    {
      if (control_sfp) {
        vl_cmd_node.switch_endpoint_sfp(address, false);
      }
      throw e;
    }

    uint64_t endpoint_rtt = echo.send_echo_and_measure_delay(); // NOLINT(build/unsigned)
    TLOG() << "Pre delay adjustment RTT:  " << format_reg_value(endpoint_rtt);
  }

  vl_cmd_node.apply_endpoint_delay(address, coarse_delay, fine_delay, phase_delay);

  if (measure_rtt) {
    millisleep(100);

    try
    {
      global.enable_upstream_endpoint();
    }
    catch (const timing::UpstreamEndpointFailedToLock& e)
    {
      if (control_sfp)
      {
        vl_cmd_node.switch_endpoint_sfp(address, false);
      }
      throw e;
    }

    uint64_t endpoint_rtt = echo.send_echo_and_measure_delay(); // NOLINT(build/unsigned)
    TLOG() << "Post delay adjustment RTT: " << format_reg_value(endpoint_rtt);

    if (control_sfp)
      vl_cmd_node.switch_endpoint_sfp(address, false);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
DUNEMasterNode::sync_timestamp(uint32_t clock_frequency_hz) const // NOLINT(build/unsigned)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
DUNEMasterNode::read_timestamp() const
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
DUNEMasterNode::set_timestamp(uint64_t timestamp) const // NOLINT(build/unsigned)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//void
//DUNEMasterNode::get_info(timingfirmwareinfo::PDIMasterMonitorData& mon_data) const
//{
//
//}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
DUNEMasterNode::get_info(opmonlib::InfoCollector& ic, int level) const
{
//  timingfirmwareinfo::PDIMasterMonitorData mon_data;
//  this->get_info(mon_data);
//  ic.add(mon_data);

  getNode<FLCmdGeneratorNode>("scmd_gen").get_info(ic, level);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
