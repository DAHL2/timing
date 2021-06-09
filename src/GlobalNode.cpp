/**
 * @file GlobalNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/GlobalNode.hpp"

#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(GlobalNode)

//-----------------------------------------------------------------------------
GlobalNode::GlobalNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
GlobalNode::~GlobalNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
GlobalNode::in_spill() const
{
  return false;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
GlobalNode::tx_error() const
{
  return false;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
GlobalNode::readTimeStamp() const
{
  return 0;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
GlobalNode::read_spill_counter() const
{
  return 0;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GlobalNode::select_partition() const
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GlobalNode::lock_partition() const
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
GlobalNode::get_status(bool print_out) const
{
  std::stringstream lStatus;
  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  lStatus << format_reg_table(subnodes, "Global state");
  if (print_out)
    TLOG() << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GlobalNode::enable_upstream_endpoint(uint32_t timeout) // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.ep_en").write(0x0);
  getClient().dispatch();
  getNode("csr.ctrl.ep_en").write(0x1);
  getClient().dispatch();

  TLOG() << "Upstream endpoint reset, waiting for lock";

  auto start = std::chrono::high_resolution_clock::now();

  std::chrono::milliseconds msSinceStart(0);

  uhal::ValWord<uint32_t> lEptStat; // NOLINT(build/unsigned)
  uhal::ValWord<uint32_t> lEptRdy;  // NOLINT(build/unsigned)

  // Wait for the endpoint to be happy
  while (msSinceStart.count() < timeout) {
    auto now = std::chrono::high_resolution_clock::now();
    msSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

//    millisleep(50);

    lEptStat = getNode("csr.stat.ep_stat").read();
    lEptRdy = getNode("csr.stat.ep_rdy").read();
    
    auto ept_edge = getNode("csr.stat.ep_edge").read();
    auto ept_fdel = getNode("csr.stat.ep_fdel").read();
    
    getClient().dispatch();

    TLOG_DEBUG(1) << "ept state: 0x" << std::hex << lEptStat.value() << " ept ready: 0x" << lEptRdy.value() << " ept edge: 0x" << ept_edge.value() << " ept fdel: 0x" << ept_fdel.value();

    if (lEptRdy.value() && lEptStat.value() == 0x8) {
      TLOG_DEBUG(1) << "Endpoint locked: state= " << format_reg_value(lEptStat);
      return;
    }
  }

  if (!lEptRdy.value() || lEptStat.value() != 0x8) {
    throw UpstreamEndpointFailedToLock(ERS_HERE, format_reg_value(lEptStat));
  } else {
    TLOG_DEBUG(1) << "Endpoint locked: state= " << format_reg_value(lEptStat);
  }
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
