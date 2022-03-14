/**
 * @file top_designs.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PDIMasterNode.hpp"

#include "timing/BoreasDesign.hpp"
#include "timing/ChronosDesign.hpp"
#include "timing/CRTDesign.hpp"
#include "timing/FanoutDesign.hpp"
#include "timing/OuroborosDesign.hpp"
#include "timing/OuroborosMuxDesign.hpp"
#include "timing/OverlordDesign.hpp"
#include "timing/EndpointDesign.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_top_designs(py::module& m)
{

  // Overlord
  py::class_<timing::OverlordDesign, uhal::Node>(m, "OverlordDesign")
    .def("read_firmware_version", &timing::OverlordDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::OverlordDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::OverlordDesign::sync_timestamp)
    .def("get_status", &timing::OverlordDesign::get_status)
    .def("enable_periodic_fl_cmd",
         &timing::OverlordDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", 
          &timing::OverlordDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt", 
          &timing::OverlordDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("get_external_triggers_endpoint_node",
         &timing::OverlordDesign::get_external_triggers_endpoint_node)
    .def("get_endpoint_node", &timing::OverlordDesign::get_endpoint_node);

  // Boreas
  py::class_<timing::BoreasDesign, uhal::Node>(m, "BoreasDesign")
    .def("read_firmware_version", &timing::BoreasDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::BoreasDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::BoreasDesign::sync_timestamp)
    .def("get_status", &timing::BoreasDesign::get_status)
    .def("enable_periodic_fl_cmd",
         &timing::BoreasDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", 
          &timing::BoreasDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt", 
          &timing::BoreasDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("get_hsi_node", &timing::BoreasDesign::get_hsi_node)
    .def("configure_hsi", 
         &timing::BoreasDesign::configure_hsi,
         py::arg("src"),
         py::arg("re_mask"),
         py::arg("fe_mask"),
         py::arg("inv_mask"),
         py::arg("rate"),
         py::arg("dispatch") = true);

  // PD-I fanout design on fib
  py::class_<timing::FanoutDesign, uhal::Node>(m, "FanoutDesign")
    .def("read_firmware_version", &timing::FanoutDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::FanoutDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::FanoutDesign::sync_timestamp)
    .def("enable_periodic_fl_cmd",
         &timing::FanoutDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("switch_sfp_mux_channel", &timing::FanoutDesign::switch_sfp_mux_channel)
    .def("apply_endpoint_delay", 
          &timing::FanoutDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt", 
          &timing::FanoutDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("scan_sfp_mux", &timing::FanoutDesign::scan_sfp_mux);

  // PD-I ouroboros design on fib
  py::class_<timing::OuroborosMuxDesign, uhal::Node>(
    m, "OuroborosMuxDesign")
    .def("read_firmware_version", &timing::OuroborosMuxDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::OuroborosMuxDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::OuroborosMuxDesign::sync_timestamp)
    .def("enable_periodic_fl_cmd",
         &timing::OuroborosMuxDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("switch_sfp_mux_channel", &timing::OuroborosMuxDesign::switch_sfp_mux_channel)
    .def("apply_endpoint_delay", 
          &timing::OuroborosMuxDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt", 
          &timing::OuroborosMuxDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("scan_sfp_mux", &timing::OuroborosMuxDesign::scan_sfp_mux);

  // Ouroboros on FMC
  py::class_<timing::OuroborosDesign, uhal::Node>(m, "OuroborosDesign")
    .def("read_firmware_version", &timing::OuroborosDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::OuroborosDesign::validate_firmware_version)
    .def("sync_timestamp", &timing::OuroborosDesign::sync_timestamp)
    .def("get_status", &timing::OuroborosDesign::get_status)
    .def("enable_periodic_fl_cmd",
         &timing::OuroborosDesign::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"))
    .def("apply_endpoint_delay", 
          &timing::OuroborosDesign::apply_endpoint_delay,
          py::arg("address"),
          py::arg("coarse_delay"),
          py::arg("fine_delay"),
          py::arg("phase_delay"),
          py::arg("measure_rtt") = false,
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1)
    .def("measure_endpoint_rtt", 
          &timing::OuroborosDesign::measure_endpoint_rtt,
          py::arg("address"),
          py::arg("control_sfp") = true,
          py::arg("sfp_mux") = -1);

  // Endpoint on FMC
  py::class_<timing::EndpointDesign, uhal::Node>(m, "EndpointDesign")
    .def("read_firmware_version", &timing::EndpointDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::EndpointDesign::validate_firmware_version)
    .def("get_status", &timing::EndpointDesign::get_status)
    .def("get_endpoint_node", &timing::EndpointDesign::get_endpoint_node);

  // Chronos on FMC
  py::class_<timing::ChronosDesign, uhal::Node>(m, "ChronosDesign")
    .def("read_firmware_version", &timing::ChronosDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::ChronosDesign::validate_firmware_version)
    .def("get_status", &timing::ChronosDesign::get_status)
    .def("get_hsi_node", &timing::ChronosDesign::get_hsi_node)
    .def("configure_hsi", 
         &timing::ChronosDesign::configure_hsi,
         py::arg("src"),
         py::arg("re_mask"),
         py::arg("fe_mask"),
         py::arg("inv_mask"),
         py::arg("rate"),
         py::arg("dispatch") = true);
    
  // CRT on FMC
  py::class_<timing::CRTDesign, uhal::Node>(m, "CRTDesign")
    .def("read_firmware_version", &timing::CRTDesign::read_firmware_version)
    .def("validate_firmware_version", &timing::CRTDesign::validate_firmware_version)
    .def("get_status", &timing::CRTDesign::get_status)
    .def("get_crt_node", &timing::CRTDesign::get_crt_node);
} // NOLINT

} // namespace python
} // namespace timing
} // namespace dunedaq