// This file is part of GQCG-GQCP.
//
// Copyright (C) 2017-2020  the GQCG developers
//
// GQCG-GQCP is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GQCG-GQCP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GQCG-GQCP.  If not, see <http://www.gnu.org/licenses/>.

#include "QCMethod/HF/UHF/UHFSCFSolver.hpp"

#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace gqcpy {


void bindUHFSCFSolver(py::module& module) {
    py::class_<GQCP::UHFSCFSolver<double>>(module, "UHFSCFSolver", "An unrestricted Hartree-Fock self-consistent field solver factory.")

        .def_static(
            "Plain",
            [](const double threshold, const size_t maximum_number_of_iterations) {
                return GQCP::UHFSCFSolver<double>::Plain(threshold, maximum_number_of_iterations);
            },
            py::arg("threshold") = 1.0e-08,
            py::arg("maximum_number_of_iterations") = 128,
            "Plain UHF SCF solver that uses the combination of norm of the difference of two consecutive alpha and beta density matrices as a convergence criterion.")

        .def_static(
            "DIIS",
            [](const size_t minimum_subspace_dimension, const size_t maximum_subspace_dimension, const double threshold, const size_t maximum_number_of_iterations) {
                return GQCP::UHFSCFSolver<double>::DIIS(minimum_subspace_dimension, maximum_subspace_dimension, threshold, maximum_number_of_iterations);
            },
            py::arg("minimum_subspace_dimension") = 6,
            py::arg("maximum_subspace_dimension") = 6,
            py::arg("threshold") = 1.0e-08,
            py::arg("maximum_number_of_iterations") = 128,
            "Return a DIIS UHF SCF solver that uses the combination of norm of the difference of two consecutive alpha and beta density matrices as a convergence criterion.");
}


}  // namespace gqcpy
