/**
 * @file TopDesign.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#include "timing/MasterDesign.hpp"
#include "timing/MasterMuxDesign.hpp"
#include "timing/FanoutDesign.hpp"

namespace dunedaq {
namespace timing {
// In leiu of UHAL_REGISTER_DERIVED_NODE

// Master
uhal::RegistrationHelper<MasterDesign> MasterDesign_RegistrationHelper("MasterDesign");
uhal::RegistrationHelper<MasterMuxDesign> MasterMuxDesign_RegistrationHelper("MasterMuxDesign");

} // namespace timing
} // namespace dunedaq