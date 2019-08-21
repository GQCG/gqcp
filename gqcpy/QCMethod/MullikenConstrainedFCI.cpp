// This file is part of GQCG-gqcp.
// 
// Copyright (C) 2017-2019  the GQCG developers
// 
// GQCG-gqcp is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// GQCG-gqcp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with GQCG-gqcp.  If not, see <http://www.gnu.org/licenses/>.
// 
#include "QCMethod/MullikenConstrainedFCI.hpp"

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace py = pybind11;


namespace gqcpy {


void bindMullikenConstrainedFCI(py::module& module) {
    py::class_<GQCP::QCMethod::MullikenConstrainedFCI>(module, "MullikenConstrainedFCI", "A class that solves the FCI Hamiltonian given a perturbation in the form of a langragian multiplier and the Mulliken operator for a pre-specified set of basis functions")
        .def(py::init<const GQCP::Molecule& , const std::string&, const std::vector<size_t>&, size_t>(), py::arg("molecule"), py::arg("basis_set"),  py::arg("basis_targets"), py::arg("frozencores") = 0)
        .def("solveMullikenDavidson", (void (GQCP::QCMethod::MullikenConstrainedFCI::*)(const double, const GQCP::VectorX<double>&)) &GQCP::QCMethod::MullikenConstrainedFCI::solveMullikenDavidson, "Solve the eigenvalue problem for a multiplier with the davidson algorithm")
        .def("solveMullikenDavidson", (void (GQCP::QCMethod::MullikenConstrainedFCI::*)(const double)) &GQCP::QCMethod::MullikenConstrainedFCI::solveMullikenDavidson, "Solve the eigenvalue problem for a multiplier with the davidson algorithm, davidson guess will be the previously stored solution if none is available the Hartree Fock expansion will be used instead")
        .def("solveMullikenDense", &GQCP::QCMethod::MullikenConstrainedFCI::solveMullikenDense)
        .def("get_energy", &GQCP::QCMethod::MullikenConstrainedFCI::get_energy, py::arg("index") = 0)
        .def("get_population", &GQCP::QCMethod::MullikenConstrainedFCI::get_population, py::arg("index") = 0)
        .def("get_lambda", &GQCP::QCMethod::MullikenConstrainedFCI::get_lambda, py::arg("index") = 0)
        .def("get_entropy", &GQCP::QCMethod::MullikenConstrainedFCI::get_entropy, py::arg("index") = 0)
        .def("get_A_fragment_energy", &GQCP::QCMethod::MullikenConstrainedFCI::get_A_fragment_energy, py::arg("index") = 0)
        .def("get_A_fragment_self_energy", &GQCP::QCMethod::MullikenConstrainedFCI::get_A_fragment_self_energy, py::arg("index") = 0)
        .def("get_B_fragment_energy", &GQCP::QCMethod::MullikenConstrainedFCI::get_B_fragment_energy, py::arg("index") = 0)
        .def("get_B_fragment_self_energy", &GQCP::QCMethod::MullikenConstrainedFCI::get_B_fragment_self_energy, py::arg("index") = 0)
        .def("get_interaction_energy", &GQCP::QCMethod::MullikenConstrainedFCI::get_interaction_energy, py::arg("index") = 0)
        .def("all_properties", &GQCP::QCMethod::MullikenConstrainedFCI::all_properties, "Get all properties from the most recent solve.");
}



}  // namespace gqcpy