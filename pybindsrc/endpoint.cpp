/**
 * @file endpoint.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/CRTNode.hpp"
#include "timing/EndpointNode.hpp"
#include "timing/HSINode.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_endpoint(py::module& m)
{
  py::class_<timing::EndpointNode, uhal::Node>(m, "EndpointNode")
    .def(py::init<const uhal::Node&>())
    .def("disable", &timing::EndpointNode::disable)
    .def("enable", &timing::EndpointNode::enable, py::arg("address") = 0, py::arg("partition") = 0)
    .def("reset", &timing::EndpointNode::reset, py::arg("address") = 0, py::arg("partition") = 0)
    .def("read_buffer_count", &timing::EndpointNode::read_buffer_count)
    .def("read_data_buffer", &timing::EndpointNode::read_data_buffer, py::arg("read_all") = false)
    .def("get_data_buffer_table",
         &timing::EndpointNode::get_data_buffer_table,
         py::arg("read_all") = false,
         py::arg("print_out") = false)
    .def("read_version", &timing::EndpointNode::read_version)
    .def("read_timestamp", &timing::EndpointNode::read_timestamp)
    .def("read_clock_frequency", &timing::EndpointNode::read_clock_frequency);

  py::class_<timing::CRTNode, uhal::Node>(m, "CRTNode")
    .def(py::init<const uhal::Node&>())
    .def("disable", &timing::CRTNode::disable)
    .def("enable", py::overload_cast<uint32_t, FixedLengthCommandType>(&timing::CRTNode::enable, py::const_)) // NOLINT(build/unsigned)
    .def("get_status", &timing::CRTNode::get_status, py::arg("print_out") = false)
    .def("read_last_pulse_timestamp", &timing::CRTNode::read_last_pulse_timestamp);

  py::class_<timing::HSINode, uhal::Node>(m, "HSINode")
    .def(py::init<const uhal::Node&>())
    .def("disable", &timing::HSINode::disable)
    .def("enable", &timing::HSINode::enable)
    .def("reset", &timing::HSINode::reset, py::arg("address") = 0, py::arg("partition") = 0)
    .def("get_status", &timing::HSINode::get_status, py::arg("print_out") = false)
    .def("configure_hsi",
         &timing::HSINode::configure_hsi,
         py::arg("src"),
         py::arg("re_mask"),
         py::arg("fe_mask"),
         py::arg("inv_mask"),
         py::arg("rate"),
         py::arg("clock_frequency_hz"),
         py::arg("dispatch") = true)
    .def("start_hsi", &timing::HSINode::start_hsi, py::arg("dispatch") = true)
    .def("stop_hsi", &timing::HSINode::stop_hsi, py::arg("dispatch") = true)
    .def("reset_hsi", &timing::HSINode::reset_hsi, py::arg("dispatch") = true)
    .def("get_data_buffer_table",
         &timing::HSINode::get_data_buffer_table,
         py::arg("read_all") = false,
         py::arg("print_out") = false)
    .def("read_buffer_warning", &timing::HSINode::reset_hsi)
    .def("read_buffer_error", &timing::HSINode::reset_hsi);
}

} // namespace python
} // namespace timing
} // namespace dunedaq