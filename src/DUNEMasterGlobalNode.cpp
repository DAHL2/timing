/**
 * @file DUNEMasterGlobalNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/DUNEMasterGlobalNode.hpp"

#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(DUNEMasterGlobalNode)

//-----------------------------------------------------------------------------
DUNEMasterGlobalNode::DUNEMasterGlobalNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
DUNEMasterGlobalNode::~DUNEMasterGlobalNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
DUNEMasterGlobalNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "DUNE master global state");
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
DUNEMasterGlobalNode::enable_upstream_endpoint(uint32_t timeout) // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.resync").write(0x1);
  getClient().dispatch();

  TLOG() << "Upstream endpoint reset, waiting for lock";

  auto start = std::chrono::high_resolution_clock::now();

  std::chrono::milliseconds ms_since_start(0);

  uhal::ValWord<uint32_t> ept_ready;  // NOLINT(build/unsigned)

  // Wait for the endpoint to be happy
  while (ms_since_start.count() < timeout) {
    auto now = std::chrono::high_resolution_clock::now();
    ms_since_start = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

    millisleep(50);

    ept_ready = getNode("csr.stat.rx_rdy").read();    
    getClient().dispatch();

    TLOG_DEBUG(1) << "ept ready: 0x" << ept_ready.value();

    if (ept_ready.value()) {
      TLOG_DEBUG(1) << "Master endpoint ready!";
      return;
    }
  }

  if (!ept_ready.value()) {
    throw UpstreamEndpointFailedToLock(ERS_HERE, format_reg_value(ept_ready));
  } else {
    TLOG_DEBUG(1) << "Master endpoint ready";
  }
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq