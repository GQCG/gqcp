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
#include "Processing/Properties/properties.hpp"
#include "Processing/Properties/RHFElectricalResponseSolver.hpp"

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace gqcpy {


void bindRHFElectricalResponseSolver(py::module& module) {

    py::class_<GQCP::RHFElectricalResponseSolver>(module, "RHFElectricalResponseSolver", "A class that is able to solve the electrical CPHF-equations for RHF.")

        .def(py::init<const size_t>(),
            py::arg("N_P")
        )

        .def("calculateWaveFunctionResponse",
            [ ] (const GQCP::RHFElectricalResponseSolver& cphf_solver, const GQCP::SQHamiltonian<double>& sq_hamiltonian, const GQCP::VectorSQOneElectronOperator<double>& dipole_op) {
                return cphf_solver.calculateWaveFunctionResponse(sq_hamiltonian, dipole_op);
            },
            "Solve the linear response equations and return the wave function response.",
            py::arg("sq_hamiltonian"),
            py::arg("dipole_op")
        )

        .def("calculateElectricPolarizability",
            [ ] (const GQCP::RHFElectricalResponseSolver& cphf_solver, const Eigen::Matrix<double, Eigen::Dynamic, 3>& x, const GQCP::VectorSQOneElectronOperator<double>& dipole_op) {
                const auto F_p = cphf_solver.calculateParameterResponseForce(dipole_op);
                return GQCP::calculateElectricPolarizability(F_p, x);
            },
            "Calculate the RHF electric polarizability",
            py::arg("x"),
            py::arg("dipole_op")
        )
    ;
}


}  // namespace gqcpy
