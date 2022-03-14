/**
 * @file MasterNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/MasterNode.hpp"

#include <string>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
MasterNode::MasterNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MasterNode::~MasterNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterNode::apply_endpoint_delay(const ActiveEndpointConfig& ept_config, bool measure_rtt) const
{
  std::string lEptIdD = ept_config.id;
  uint32_t ept_address = ept_config.adr;    // NOLINT(build/unsigned)
  uint32_t coarse_Delay = ept_config.cdelay; // NOLINT(build/unsigned)
  uint32_t fine_delay = ept_config.fdelay; // NOLINT(build/unsigned)
  uint32_t phase_delay = ept_config.pdelay; // NOLINT(build/unsigned)
  apply_endpoint_delay(ept_address, coarse_Delay, fine_delay, phase_delay, measure_rtt);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterNode::send_fl_cmd(FixedLengthCommandType command,
                           uint32_t channel,                  // NOLINT(build/unsigned)
                           uint32_t number_of_commands) const // NOLINT(build/unsigned)
{
  for (uint32_t i = 0; i < number_of_commands; i++) { // NOLINT(build/unsigned)
    getNode<FLCmdGeneratorNode>("scmd_gen").send_fl_cmd(command, channel, getNode<TimestampGeneratorNode>("tstamp"));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterNode::enable_periodic_fl_cmd(uint32_t channel, double rate, bool poisson, uint32_t clock_frequency_hz) const // NOLINT(build/unsigned)
{

  // Configures the internal command generator to produce triggers at a defined frequency.
  // Rate =  (clock_frequency_hz / 2^(d+8)) / p where n in [0,15] and p in [1,256]

  // DIVIDER (int): Frequency divider.

  // The division from clock_frequency_hz to the desired rate is done in three steps:
  // a) A pre-division by 256
  // b) Division by a power of two set by n = 2 ^ rate_div_d (ranging from 2^0 -> 2^15)
  // c) 1-in-n prescaling set by n = rate_div_p

  FakeTriggerConfig fake_trigger_config(rate, clock_frequency_hz);

  fake_trigger_config.print();
  std::stringstream trig_stream;
  trig_stream << "> Trigger rate for FakeTrig" << channel << " (" << std::showbase << std::hex << 0x8 + channel
              << ") set to " << std::setprecision(3) << std::scientific << fake_trigger_config.actual_rate << " Hz";
  TLOG() << trig_stream.str();

  std::stringstream trigger_mode_stream;
  trigger_mode_stream << "> Trigger mode: ";

  if (poisson) {
    trigger_mode_stream << "poisson";
  } else {
    trigger_mode_stream << "periodic";
  }
  TLOG() << trigger_mode_stream.str();
  getNode<FLCmdGeneratorNode>("scmd_gen").enable_fake_trigger(channel, fake_trigger_config.divisor, fake_trigger_config.prescale, poisson);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterNode::disable_periodic_fl_cmd(uint32_t channel) const // NOLINT(build/unsigned)
{
  getNode<FLCmdGeneratorNode>("scmd_gen").disable_fake_trigger(channel);
}
//------------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq