/**
 * @file definitions.hpp
 *
 * Timing definitions and types.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_DEFINITIONS_HPP_
#define TIMING_INCLUDE_TIMING_DEFINITIONS_HPP_

#include "TimingIssues.hpp"

#include "timing/definitions/Structs.hpp"

#include "logging/Logging.hpp"

#include <cmath>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

typedef definitions::FixedLengthCommandType FixedLengthCommandType;

typedef std::map<std::string, uint32_t> Snapshot; // NOLINT(build/unsigned)

enum BoardType
{
  kBoardFMC,
  kBoardSim,
  kBoardPC059,
  kBoardMicrozed,
  kBoardTLU,
  kBoardFIB,
  kBoardUnknown
};
enum CarrierType
{
  kCarrierEnclustraA35,
  kCarrierKC705,
  kCarrierMicrozed,
  kCarrierATFC,
  kCarrierAFC,
  kCarrierNexusVideo,
  kCarrierUnknown
};
enum DesignType
{
  kDesignMaster,
  kDesignOuroborosSim,
  kDesignOuroboros,
  kDesignTest,
  kDesignEndpoint,
  kDesignFanout,
  kDesignOverlord,
  kDesignEndpoBICRT,
  kDesignChronos,
  kDesignBoreas,
  kDesignUnknown
};
enum BoardRevision
{
  kFMCRev1,
  kFMCRev2,
  kFMCRev3,
  kFMCRev4,
  kPC059Rev1,
  kTLURev1,
  kSIMRev1,
  kFIBRev1,
};

const std::vector<BoardType> g_library_supported_boards = {
    kBoardFMC, kBoardPC059, kBoardTLU, kBoardSim, kBoardFIB
};

const uint32_t g_event_size = 6;             // NOLINT(build/unsigned)
const uint32_t g_hsi_event_size = 5;         // NOLINT(build/unsigned)

const int g_required_major_master_firmware_version = 6;
const int g_required_minor_master_firmware_version = 2;
const int g_required_patch_master_firmware_version = 0;

const int g_required_major_endpoint_firmware_version = 6;
const int g_required_minor_endpoint_firmware_version = 2;
const int g_required_patch_endpoint_firmware_version = 0;

const std::map<BoardType, std::string> g_board_type_map = { { kBoardFMC, "fmc" },
                                                            { kBoardSim, "sim" },
                                                            { kBoardPC059, "pc059" },
                                                            { kBoardMicrozed, "microzed" },
                                                            { kBoardTLU, "tlu" },
                                                            { kBoardFIB, "fib" }, };

const std::map<CarrierType, std::string> g_carrier_type_map = { { kCarrierEnclustraA35, "enclustra-a35" },
                                                                { kCarrierKC705, "kc705" },
                                                                { kCarrierMicrozed, "microzed" },
                                                                { kCarrierATFC, "atfc" },
                                                                { kCarrierAFC, "afc" },
                                                                { kCarrierNexusVideo, "nexus-video" },

};

const std::map<DesignType, std::string> g_design_type_map = {
  { kDesignMaster, "master" },
  { kDesignOuroboros, "ouroboros" },
  { kDesignOuroborosSim, "ouroboros-sim" },
  { kDesignTest, "test-design" },
  { kDesignEndpoint, "endpoint" },
  { kDesignFanout, "fanout" },
  { kDesignOverlord, "overlord" },
  { kDesignEndpoBICRT, "endpoint-bi-crt" },
  { kDesignChronos, "chronos" },
  { kDesignBoreas, "boreas" },
};

const std::vector<DesignType> g_library_supported_designs = {
    kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignOverlord, kDesignEndpoBICRT, kDesignChronos, kDesignBoreas
};

const std::map<BoardRevision, std::string> g_board_revision_map = {
  { kFMCRev1, "kFMCRev1" },     { kFMCRev2, "kFMCRev2" }, { kFMCRev3, "kFMCRev3" },
  { kPC059Rev1, "kPC059Rev1" }, { kTLURev1, "kTLURev1" }, { kSIMRev1, "kSIMRev1" },
  { kFIBRev1, "kFIBRev1" }, { kFMCRev4, "kFMCRev4" },
};

// NOLINTNEXTLINE(build/unsigned)
const std::map<uint64_t, BoardRevision> g_board_uid_revision_map = {
  { 0xd880395e720b, kFMCRev1 },
  { 0xd880395e501a, kFMCRev1 },
  { 0xd880395e50b8, kFMCRev1 },
  { 0xd880395e501b, kFMCRev1 },
  { 0xd880395e7201, kFMCRev1 },
  { 0xd880395e4fcc, kFMCRev1 },
  { 0xd880395e5069, kFMCRev1 },
  { 0xd880395e7206, kFMCRev1 },
  { 0xd880395e1c86, kFMCRev2 },
  { 0xd880395e2630, kFMCRev2 },
  { 0xd880395e262b, kFMCRev2 },
  { 0xd880395e2b38, kFMCRev2 },
  { 0xd880395e1a6a, kFMCRev2 },
  { 0xd880395e36ae, kFMCRev2 },
  { 0xd880395e2b2e, kFMCRev2 },
  { 0xd880395e2b33, kFMCRev2 },
  { 0xd880395e1c81, kFMCRev2 },
  { 0x049162b67ce6, kFMCRev3 },
  { 0x049162b62947, kFMCRev3 },
  { 0x049162b67cdf, kFMCRev3 },
  { 0x49162b62050, kFMCRev3 },
  { 0x49162b62951, kFMCRev3 },
  { 0x49162b675e3, kFMCRev3 },
  { 0xd88039d980cf, kPC059Rev1 },
  { 0xd88039d98adf, kPC059Rev1 },
  { 0xd88039d92491, kPC059Rev1 },
  { 0xd88039d9248e, kPC059Rev1 },
  { 0xd88039d98ae9, kPC059Rev1 },
  { 0xd88039d92498, kPC059Rev1 },
  { 0x5410ecbba408, kTLURev1 },
  { 0x5410ecbb9426, kTLURev1 },
  { 0x801f12f5ce48, kFIBRev1 },
  { 0x801f12f5e9ae, kFIBRev1 },
  { 0x49162b65025, kFMCRev3 },
  { 0x49162b62948, kFMCRev3 },
  { 0x49162b675ea, kFMCRev3 },
  { 0x49162b645cd, kFMCRev3 },
  { 0xd880395dab52, kFMCRev4 },
  { 0xd880395d99b3, kFMCRev4 },
  { 0xd880395df010, kFMCRev4 },
  { 0xd880395df00f, kFMCRev4 },
  { 0xd880395de4a4, kFMCRev4 },
  { 0xd880395d99a9, kFMCRev4 },
  { 0xd880395d99b0, kFMCRev4 },
  { 0xd880395de452, kFMCRev4 },
  { 0xd880395da48e, kFMCRev4 },
  { 0xd880395dbcee, kFMCRev4 },
  { 0x5410ecbb6845, kTLURev1 },
};

const std::map<std::string, std::string> g_clock_config_map = {
  // 62.5 MHz mappings
  { "kFMCRev1_endpoint", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt" },
  { "kFMCRev2_endpoint", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt" },
  { "kFMCRev3_endpoint", "devel/Si5394-053endptr_62-5MHz_4kHz-Registers.txt" },
  { "kFMCRev4_endpoint", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt" },
  
  { "kFMCRev1_endpoint-bi-crt", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt" },
  { "kFMCRev2_endpoint-bi-crt", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt" },
  { "kFMCRev3_endpoint-bi-crt", "devel/Si5394-053endptr_62-5MHz_4kHz-Registers.txt" },
  { "kFMCRev4_endpoint-bi-crt", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt" },

  { "kFMCRev1_chronos", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt" },
  { "kFMCRev2_chronos", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt" },
  { "kFMCRev3_chronos", "devel/Si5394-053endptr_62-5MHz_4kHz-Registers.txt" },
  { "kFMCRev4_chronos", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt" },

  { "kFMCRev1_ouroboros", "devel/Si5344-053master_312.5_mhz-Registers.txt" },
  { "kFMCRev2_ouroboros", "devel/Si5344-053master_312.5_mhz-Registers.txt" },
  { "kFMCRev3_ouroboros", "devel/Si5394-RevA-94mst625-Registers.txt" },
  { "kFMCRev4_ouroboros", "devel/Si5344-053master_312.5_mhz-Registers.txt" },

  { "kFMCRev1_master", "devel/Si5344-053master_312.5_mhz-Registers.txt" },
  { "kFMCRev2_master", "devel/Si5344-053master_312.5_mhz-Registers.txt" },
  { "kFMCRev3_master", "devel/Si5394-RevA-94mst625-Registers.txt" },
  { "kFMCRev4_master", "devel/Si5344-053master_312.5_mhz-Registers.txt" },

  { "kFMCRev1_overlord", "devel/Si5344-053master_312.5_mhz-Registers.txt" },
  { "kFMCRev2_overlord", "devel/Si5344-053master_312.5_mhz-Registers.txt" },
  { "kFMCRev3_overlord", "devel/Si5394-RevA-94mst625-Registers.txt" },
  { "kFMCRev4_overlord", "devel/Si5344-053master_312.5_mhz-Registers.txt" },

  { "kFMCRev1_boreas", "devel/Si5344-053master_312.5_mhz-Registers.txt" },
  { "kFMCRev2_boreas", "devel/Si5344-053master_312.5_mhz-Registers.txt" },
  { "kFMCRev3_boreas", "devel/Si5394-RevA-94mst625-Registers.txt" },
  { "kFMCRev4_boreas", "devel/Si5344-053master_312.5_mhz-Registers.txt" },

  { "kTLURev1_overlord", "devel/DUNE_TLU-DUNTLU09-Registers_62.5_mhz.txt" },
  { "kTLURev1_boreas", "devel/DUNE_TLU-DUNTLU09-Registers_62.5_mhz.txt" },

  { "kPC059Rev1_ouroboros", "devel/pc059_standalone_312_mhz-059_1_62-Registers.txt" },
  { "kPC059Rev1_fanout_mode0",
    "devel/pc059_sfp_in_312_mhz-059_0s62-Registers.txt" }, // fanout mode, assuming sfp is the upstream input
  { "kPC059Rev1_fanout_mode1",
    "devel/pc059_standalone_312_mhz-059_1_62-Registers.txt" }, // stand-alone mode
  { "kPC059Rev1_fanout",
    "devel/pc059_standalone_312_mhz-059_1_62-Registers.txt" }, // stand-alone mode

  { "kFIBRev1_fanout_mode0",
    "devel/Si5395-RevA-FIB_fanout-65_FA_31-Registers.txt" }, // fanout mode, data and clock from backplane
  { "kFIBRev1_fanout_mode1",
    "devel/Si5395-RevA-FIB_ouroboros-65_SA_31-Registers.txt" }, // stand-alone mode
  { "kFIBRev1_fanout",
    "devel/Si5395-RevA-FIB_ouroboros-65_SA_31-Registers.txt" }, // stand-alone mode  

  { "kFIBRev1_ouroboros", "devel/Si5395-RevA-FIB_ouroboros-65_SA_31-Registers.txt" },
  { "kFIBRev1_ouroboros", "devel/Si5395-RevA-FIB_ouroboros-65_SA_31-Registers.txt" },

  // 50 MHz mappings
  { "kFMCRev1_endpoint_50_mhz", "devel/ENDPOINT-Si5344-50MHzRef.txt"},
  { "kFMCRev2_endpoint_50_mhz", "devel/ENDPOINT-Si5344-50MHzRef.txt"},
  { "kFMCRev3_endpoint_50_mhz", "devel/ENDPOINT-Si5344-50MHzRef.txt"},
  { "kFMCRev4_endpoint_50_mhz", "devel/ENDPOINT-Si5344-50MHzRef.txt"},

  { "kFMCRev1_endpoint-bi-crt_50_mhz", "devel/Si5344-PDTSCRT1NoZdm-RevD-400HzBW-Registers.txt" },
  { "kFMCRev2_endpoint-bi-crt_50_mhz", "devel/Si5344-PDTSCRT1NoZdm-RevD-400HzBW-Registers.txt" },
  { "kFMCRev3_endpoint-bi-crt_50_mhz", "devel/Si5344-PDTSCRT1NoZdm-RevD-400HzBW-Registers.txt" },
  { "kFMCRev4_endpoint-bi-crt_50_mhz", "devel/Si5344-PDTSCRT1NoZdm-RevD-400HzBW-Registers.txt" },

  { "kFMCRev1_chronos_50_mhz", "devel/ENDPOINT-Si5344-50MHzRef.txt" },
  { "kFMCRev2_chronos_50_mhz", "devel/ENDPOINT-Si5344-50MHzRef.txt" },
  { "kFMCRev3_chronos_50_mhz", "devel/ENDPOINT-Si5344-50MHzRef.txt" },
  { "kFMCRev4_chronos_50_mhz", "devel/ENDPOINT-Si5344-50MHzRef.txt" },

  { "kFMCRev1_ouroboros_50_mhz", "SI5344/PDTS0000.txt" },
  { "kFMCRev2_ouroboros_50_mhz", "SI5344/PDTS0003.txt" },
  { "kFMCRev3_ouroboros_50_mhz", "SI5344/PDTS0003.txt" },
  { "kFMCRev4_ouroboros_50_mhz", "SI5344/PDTS0003.txt" },

  { "kFMCRev1_master_50_mhz", "SI5344/PDTS0000.txt" },
  { "kFMCRev2_master_50_mhz", "SI5344/PDTS0003.txt" },
  { "kFMCRev3_master_50_mhz", "SI5344/PDTS0003.txt" },
  { "kFMCRev4_master_50_mhz", "SI5344/PDTS0003.txt" },

  { "kFMCRev1_overlord_50_mhz", "SI5344/PDTS0000.txt" },
  { "kFMCRev2_overlord_50_mhz", "SI5344/PDTS0003.txt" },
  { "kFMCRev3_overlord_50_mhz", "SI5344/PDTS0003.txt" },
  { "kFMCRev4_overlord_50_mhz", "SI5344/PDTS0003.txt" },

  { "kFMCRev1_boreas_50_mhz", "SI5344/PDTS0000.txt" },
  { "kFMCRev2_boreas_50_mhz", "SI5344/PDTS0003.txt" },
  { "kFMCRev3_boreas_50_mhz", "SI5344/PDTS0003.txt" },
  { "kFMCRev4_boreas_50_mhz", "SI5344/PDTS0003.txt" },

  { "kTLURev1_overlord_50_mhz", "wr/TLU_EXTCLK_10MHZ_NOZDM.txt" },
  { "kTLURev1_boreas_50_mhz", "wr/TLU_EXTCLK_10MHZ_NOZDM.txt" },
  
  { "kPC059Rev1_ouroboros_50_mhz", "SI5345/PDTS0005.txt" },
  { "kPC059Rev1_fanout_mode0_50_mhz", "wr/FANOUT_PLL_WIDEBW_SFPIN.txt" }, // fanout mode
  { "kPC059Rev1_fanout_mode1_50_mhz", "devel/PDTS_PC059_FANOUT.txt" },    // stand-alone mode
  { "kPC059Rev1_fanout_50_mhz", "devel/PDTS_PC059_FANOUT.txt" },    // stand-alone mode
};

// NOLINTNEXTLINE(build/unsigned)
const std::map<FixedLengthCommandType, std::string> g_command_map = {
  { FixedLengthCommandType::TimeSync, "TimeSync" }, 
  { FixedLengthCommandType::Echo, "Echo" }, 
  { FixedLengthCommandType::SpillStart, "SpillStart" }, 
  { FixedLengthCommandType::SpillStop, "SpillStop" }, 
  { FixedLengthCommandType::RunStart, "RunStart" },
  { FixedLengthCommandType::RunStop, "RunStop" }, 
  { FixedLengthCommandType::WibCalib, "WibCalib" }, 
  { FixedLengthCommandType::SSPCalib, "SSPCalib" }, 
  { FixedLengthCommandType::FakeTrig0, "FakeTrig0" }, 
  { FixedLengthCommandType::FakeTrig1, "FakeTrig1" },
  { FixedLengthCommandType::FakeTrig2, "FakeTrig2" }, 
  { FixedLengthCommandType::FakeTrig3, "FakeTrig3" }, 
  { FixedLengthCommandType::BeamTrig, "BeamTrig" }, 
  { FixedLengthCommandType::NoBeamTrig, "NoBeamTrig" }, 
  { FixedLengthCommandType::ExtFakeTrig, "ExtFakeTrig" }
};

const uint32_t g_command_number = g_command_map.size(); // NOLINT(build/unsigned)

// NOLINTNEXTLINE(build/unsigned)
const std::map<uint32_t, std::string> g_endpoint_state_map = {
  { 0x0, "Standing by (0x0)" },                              // 0b0000 when W_RST, -- Starting state after reset
  { 0x1, "Waiting for SFP signal (0x1)" },                   // 0b0001 when W_LINK, -- Waiting for SFP LOS to go low
  { 0x2, "Waiting for good frequency check (0x2)" },         // 0b0010 when W_FREQ, -- Waiting for good frequency check
  { 0x3, "Waiting for phase adjustment to complete (0x3)" }, // 0b0011 when W_ADJUST, -- Waiting for phase adjustment to
                                                             // complete
  { 0x4, "Waiting for comma alignment (0x4)" }, // 0b0100 when W_ALIGN, -- Waiting for comma alignment, stable 62.5 (50)
                                                // MHz phase
  { 0x5, "Waiting for 8b10 decoder good packet (0x5)" }, // 0b0101 when W_LOCK, -- Waiting for 8b10 decoder good packet
  { 0x6, "Waiting for phase adjustment command (0x6)" }, // 0b0110 when W_PHASE, -- Waiting for phase adjustment command
  { 0x7, "Waiting for time stamp initialisation (0x7)" }, // 0b0111 when W_RDY, -- Waiting for time stamp initialisation
  { 0x8, "Ready (0x8)" },                                 // 0b1000 when RUN, -- Good to go
  { 0xc, "Error in Rx (0xc)" },                           // 0b1100 when ERR_R, -- Error in rx
  { 0xd, "Error in time stamp check (0xd)" },             // 0b1101 when ERR_T; -- Error in time stamp check
  { 0xe, "Error in physical layer after lock (0xe)" },    // 0b1110 when ERR_P; -- Physical layer error after lock
};

struct FakeTriggerConfig
{
  double requested_rate;
  uint32_t divisor;  // NOLINT(build/unsigned)
  uint32_t prescale; // NOLINT(build/unsigned)
  double actual_rate;

  explicit FakeTriggerConfig(double rate, uint32_t clock_frequency_hz) // NOLINT
    : requested_rate(rate)
  {
    // Rate =  (clock_frequency_hz / 2^(d+8)) / p where n in [0,15] and p in [1,256]

    // DIVIDER (int): Frequency divider.

    // The division from clock_frequency_hz to the desired rate is done in three steps:
    // a) A pre-division by 256
    // b) Division by a power of two set by n = 2 ^ rate_div_d (ranging from 2^0 -> 2^15)
    // c) 1-in-n prescaling set by n = rate_div_p

    double div = ceil(log(clock_frequency_hz / (requested_rate * 256 * 256)) / log(2));
    if (div < 0) {
      divisor = 0;
    } else if (div > 15) {
      divisor = 15;
    } else {
      divisor = div;
    }

    uint32_t ps = (uint32_t)((clock_frequency_hz / (rate * 256 * (1 << divisor))) + 0.5); // NOLINT(build/unsigned)
    if (ps == 0 || ps > 256) {
      throw BadRequestedFakeTriggerRate(ERS_HERE, rate, ps);
    } else {
      prescale = ps;
    }
    actual_rate = static_cast<double>(clock_frequency_hz) / (256 * prescale * (1 << divisor));
  }

  void print() const
  {
    TLOG() << "Requested rate, actual rate: " << requested_rate << ", " << actual_rate;
    TLOG() << "prescale, divisor: " << prescale << ", " << divisor;
  }
};

struct ActiveEndpointConfig
{
  std::string id;
  uint32_t adr; // NOLINT(build/unsigned)
  int32_t fanout;
  uint32_t mux; // NOLINT(build/unsigned)
  bool active;
  uint32_t cdelay; // NOLINT(build/unsigned)
  uint32_t fdelay; // NOLINT(build/unsigned)
  uint32_t pdelay; // NOLINT(build/unsigned)

  ActiveEndpointConfig(std::string aId,
                       uint32_t aAdr, // NOLINT(build/unsigned)
                       int32_t aFanout = -1,
                       uint32_t aMux = 0,           // NOLINT(build/unsigned)
                       uint32_t coarse_delayay = 0, // NOLINT(build/unsigned)
                       uint32_t fine_delayay = 0)   // NOLINT(build/unsigned)
    : id(aId)
    , adr(aAdr)
    , fanout(aFanout)
    , mux(aMux)
    , active(true)
    , cdelay(coarse_delayay)
    , fdelay(fine_delayay)
    , pdelay(0)
  {}
};

struct EndpointRTTResult
{
  std::string id;
  uint32_t adr; // NOLINT(build/unsigned)
  int32_t fanout;
  uint32_t mux; // NOLINT(build/unsigned)
  int32_t measuredRTT;

  EndpointRTTResult(ActiveEndpointConfig ept_config, uint32_t aMeasuredRTT) // NOLINT(build/unsigned)
    : id(ept_config.id)
    , adr(ept_config.adr)
    , fanout(ept_config.fanout)
    , mux(ept_config.mux)
    , measuredRTT(aMeasuredRTT)
  {}
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_DEFINITIONS_HPP_
