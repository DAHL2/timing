/**
 * @file master.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PDIMasterNode.hpp"
#include "timing/TriggerReceiverNode.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_master(py::module& m)
{
  py::class_<timing::PDIMasterNode, uhal::Node>(m, "PDIMasterNode")
    .def(py::init<const uhal::Node&>())
    .def<void (timing::PDIMasterNode::*)(uint32_t, uint32_t, uint32_t, uint32_t, bool, bool) // NOLINT(build/unsigned)
         const>("apply_endpoint_delay",
                &timing::PDIMasterNode::apply_endpoint_delay,
                py::arg("address"),
                py::arg("coarse_delay"),
                py::arg("fine_delay"),
                py::arg("phase_delay"),
                py::arg("measure_rtt") = false,
                py::arg("control_sfp") = true)
    .def("measure_endpoint_rtt",
         &timing::PDIMasterNode::measure_endpoint_rtt,
         py::arg("address"),
         py::arg("control_sfp") = true)
    .def("switch_endpoint_sfp", &timing::PDIMasterNode::switch_endpoint_sfp)
    .def("enable_upstream_endpoint", &timing::PDIMasterNode::enable_upstream_endpoint)
    .def("send_fl_cmd",
         &timing::PDIMasterNode::send_fl_cmd,
         py::arg("command"),
         py::arg("channel"),
         py::arg("number_of_commands") = 1)
    .def("enable_periodic_fl_cmd",
         &timing::PDIMasterNode::enable_periodic_fl_cmd,
         py::arg("channel"),
         py::arg("rate"),
         py::arg("poisson"),
         py::arg("clock_frequency_hz"))
    .def("disable_periodic_fl_cmd", &timing::PDIMasterNode::disable_periodic_fl_cmd)
    .def("enable_spill_interface", &timing::PDIMasterNode::enable_spill_interface)
    .def("enable_fake_spills",
         &timing::PDIMasterNode::enable_fake_spills,
         py::arg("cycle_length") = 16,
         py::arg("spill_length") = 8)
    .def("get_status", &timing::PDIMasterNode::get_status, py::arg("print_out") = false)
    .def("get_status_with_date", &timing::PDIMasterNode::get_status_with_date, py::arg("clock_frequency_hz"), py::arg("print_out") = false)
    .def("sync_timestamp", &timing::PDIMasterNode::sync_timestamp);

  py::class_<timing::TriggerReceiverNode, uhal::Node>(m, "TriggerReceiverNode")
    .def(py::init<const uhal::Node&>())
    .def("enable", &timing::TriggerReceiverNode::enable)
    .def("disable", &timing::TriggerReceiverNode::disable)
    .def("reset", &timing::TriggerReceiverNode::reset)
    .def("enable_triggers", &timing::TriggerReceiverNode::enable_triggers)
    .def("disable_triggers", &timing::TriggerReceiverNode::disable_triggers)
    .def("get_status", &timing::TriggerReceiverNode::get_status, py::arg("print_out") = false);
}

} // namespace python
} // namespace timing
} // namespace dunedaq