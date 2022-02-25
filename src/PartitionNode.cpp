/**
 * @file PartitionNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PartitionNode.hpp"
#include "timing/toolbox.hpp"
#include "timing/definitions/Structs.hpp"

#include "logging/Logging.hpp"

#include <chrono>
#include <string>
#include <thread>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(PartitionNode)

// Static data member initialization
const uint32_t PartitionNode::kWordsPerEvent = 6; // NOLINT(build/unsigned)

//-----------------------------------------------------------------------------
PartitionNode::PartitionNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PartitionNode::~PartitionNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::enable(bool enable, bool dispatch) const
{
  getNode("csr.ctrl.part_en").write(enable);

  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// void
// PartitionNode::writeTriggerMask( uint32_t aMask) const { // NOLINT(build/unsigned)
//     getNode("csr.ctrl.trig_mask").write(aMask);
//     getClient().dispatch();
// }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::configure(uint32_t trigger_mask, // NOLINT(build/unsigned)
                         bool enable_spill_gate,
                         bool rate_control_enabled) const
{
  getNode("csr.ctrl.rate_ctrl_en").write(rate_control_enabled);
  getNode("csr.ctrl.trig_mask").write(trigger_mask);
  getNode("csr.ctrl.spill_gate_en").write(enable_spill_gate);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::configure_rate_ctrl(bool rate_control_enabled) const
{
  getNode("csr.ctrl.rate_ctrl_en").write(rate_control_enabled);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::enable_triggers(bool enable) const
{
  // Disable the buffer
  getNode("csr.ctrl.trig_en").write(enable);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
PartitionNode::read_trigger_mask() const
{
  uhal::ValWord<uint32_t> mask = getNode("csr.ctrl.trig_mask").read(); // NOLINT(build/unsigned)
  getClient().dispatch();

  return mask;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
PartitionNode::read_buffer_word_count() const
{
  uhal::ValWord<uint32_t> words = getNode("buf.count").read(); // NOLINT(build/unsigned)
  getClient().dispatch();

  return words;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
PartitionNode::num_events_in_buffer() const
{
  return read_buffer_word_count() / kWordsPerEvent;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
PartitionNode::read_rob_warning_overflow() const
{
  uhal::ValWord<uint32_t> word = getNode("csr.stat.buf_warn").read(); // NOLINT(build/unsigned)
  getClient().dispatch();

  return word.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
PartitionNode::read_rob_error() const
{
  uhal::ValWord<uint32_t> word = getNode("csr.stat.buf_err").read(); // NOLINT(build/unsigned)
  getClient().dispatch();

  return word.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<uint32_t> // NOLINT(build/unsigned)
PartitionNode::read_events(size_t number_of_events) const
{

  uint32_t events_in_buffer = num_events_in_buffer(); // NOLINT(build/unsigned)

  uint32_t events_to_read = (number_of_events == 0 ? events_in_buffer : number_of_events); // NOLINT(build/unsigned)

  if (events_in_buffer < events_to_read) {
    throw EventReadError(ERS_HERE, number_of_events, events_in_buffer);
  }

  uhal::ValVector<uint32_t> raw_events = // NOLINT(build/unsigned)
    getNode("buf.data").readBlock(events_to_read * kWordsPerEvent);
  getClient().dispatch();

  return raw_events.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::reset() const
{
  // Disable partition
  getNode("csr.ctrl.part_en").write(0);
  // disable trigger
  getNode("csr.ctrl.trig_en").write(0);
  // Disable buffer in partition 0
  getNode("csr.ctrl.buf_en").write(0);
  // stop run
  getNode("csr.ctrl.run_req").write(0);
  // Reset trigger counter
  getNode("csr.ctrl.trig_ctr_rst").write(1);
  // Release trigger counter
  getNode("csr.ctrl.trig_ctr_rst").write(0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::start(uint32_t timeout /*milliseconds*/) const // NOLINT(build/unsigned)
{

  // Disable triggers (just in case)
  getNode("csr.ctrl.trig_en").write(0);
  // Disable the buffer
  getNode("csr.ctrl.buf_en").write(0);
  getClient().dispatch();
  // Re-enable the buffer (flushes it)
  getNode("csr.ctrl.buf_en").write(1);
  getClient().dispatch();

  // Set the run bit and wait for it to be acknowledged
  getNode("csr.ctrl.run_req").write(1);
  getClient().dispatch();

  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

  while (true) {
    auto in_run = getNode("csr.stat.in_run").read();
    getClient().dispatch();

    if (in_run)
      break;

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    if ((end - start) > std::chrono::milliseconds(timeout)) {
      throw RunRequestTimeoutExpired(ERS_HERE, timeout);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::stop(uint32_t timeout /*milliseconds*/) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.run_req").write(0);
  getClient().dispatch();

  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

  while (true) {

    auto in_run = getNode("csr.stat.in_run").read();
    getClient().dispatch();

    if (!in_run)
      break;

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    if ((end - start) > std::chrono::milliseconds(timeout)) {
      throw RunRequestTimeoutExpired(ERS_HERE, timeout);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PartitionCounts
PartitionNode::read_command_counts() const
{

  const uhal::Node& accepted_counters = getNode("actrs");
  const uhal::Node& rejected_counters = getNode("rctrs");

  uhal::ValVector<uint32_t> accepted = accepted_counters.readBlock(accepted_counters.getSize()); // NOLINT(build/unsigned)
  uhal::ValVector<uint32_t> rejected = rejected_counters.readBlock(rejected_counters.getSize()); // NOLINT(build/unsigned)
  getClient().dispatch();

  return { accepted.value(), rejected.value() };
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PartitionNode::get_status(bool print_out) const
{
  std::stringstream status;

  auto controls = read_sub_nodes(getNode("csr.ctrl"), false);
  auto state = read_sub_nodes(getNode("csr.stat"), false);

  auto event_counter = getNode("evtctr").read();
  auto buffer_count = getNode("buf.count").read();

  auto accepted_counters = getNode("actrs").readBlock(getNode("actrs").getSize());
  auto rejected_counters = getNode("rctrs").readBlock(getNode("actrs").getSize());

  getClient().dispatch();

  std::string partition_id = getId();
  std::string partition_number = partition_id.substr(partition_id.find("partition") + 9);

  status << "=> Partition " << partition_number << std::endl;
  status << std::endl;

  status << format_reg_table(controls, "Controls") << std::endl;
  status << format_reg_table(state, "State") << std::endl;

  status << "Event Counter: " << event_counter.value() << std::endl;
  std::string buffer_status_string = !state.find("buf_err")->second.value() ? "OK" : "Error";
  status << "Buffer status: " << buffer_status_string << std::endl;
  status << "Buffer occupancy: " << buffer_count.value() << std::endl;

  status << std::endl;

  std::vector<uhal::ValVector<uint32_t>> counters_container = { accepted_counters, rejected_counters }; // NOLINT(build/unsigned)

  status << format_counters_table(counters_container, { "Accept counters", "Reject counters" });

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------
void
PartitionNode::get_info(timingfirmwareinfo::TimingPartitionMonitorData& mon_data) const
{
  auto controls = read_sub_nodes(getNode("csr.ctrl"), false);
  auto state = read_sub_nodes(getNode("csr.stat"), false);

  auto event_counter = getNode("evtctr").read();
  auto buffer_count = getNode("buf.count").read();

  getClient().dispatch();

  mon_data.enabled = controls.at("part_en").value();
  mon_data.spill_interface_enabled = controls.at("spill_gate_en").value();
  mon_data.trig_enabled = controls.at("trig_en").value();
  mon_data.trig_mask = controls.at("trig_mask").value();
  mon_data.rate_ctrl_enabled = controls.at("rate_ctrl_en").value();
  mon_data.frag_mask = controls.at("frag_mask").value();
  mon_data.buffer_enabled = controls.at("buf_en").value();

  mon_data.in_run = state.at("in_run").value();
  mon_data.in_spill = state.at("in_spill").value();

  mon_data.buffer_warning = state.at("buf_warn").value();
  mon_data.buffer_error = state.at("buf_err").value();
  mon_data.buffer_occupancy = buffer_count.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::get_info(opmonlib::InfoCollector& ic, int /*level*/) const
{
  timingfirmwareinfo::TimingPartitionMonitorData mon_data;
  this->get_info(mon_data);
  ic.add(mon_data);

  auto accepted_counters = getNode("actrs").readBlock(getNode("actrs").getSize());
  auto rejected_counters = getNode("rctrs").readBlock(getNode("actrs").getSize());
  getClient().dispatch();


  for (uint i=0; i < g_command_number; ++i) { // NOLINT(build/unsigned)
    timingfirmwareinfo::TimingFLCmdCounter cmd_counter;
    opmonlib::InfoCollector cmd_counter_ic;

    cmd_counter.accepted = accepted_counters.at(i);
    cmd_counter.rejected = rejected_counters.at(i);

    cmd_counter_ic.add(cmd_counter);
    ic.add(str(static_cast<definitions::FixedLengthCommandType>(i)), cmd_counter_ic);
  }
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
