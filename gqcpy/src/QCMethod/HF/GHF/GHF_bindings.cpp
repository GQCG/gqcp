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

#include "Mathematical/Algorithm/IterativeAlgorithm.hpp"
#include "QCMethod/HF/GHF/GHF.hpp"
#include "QCMethod/HF/GHF/GHFSCFSolver.hpp"
#include "Utilities/aliases.hpp"

#include <pybind11/pybind11.h>


namespace gqcpy {


// Provide some shortcuts for frequent namespaces.
namespace py = pybind11;
using namespace GQCP;


/**
 *  Add Python bindings for some APIs related to `QCMethod::GHF`.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain APIs related to `QCMethod::GHF`.
 */
template <typename Class>
void bindQCMethodGHFInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;
    using Scalar = typename Type::Scalar;


    py_class

        /*
         *  MARK: Public methods
         */

        .def_static(
            "optimize",
            [](IterativeAlgorithm<GHFSCFEnvironment<Scalar>>& solver, GHFSCFEnvironment<Scalar>& environment) {
                return QCMethod::GHF<Scalar>().optimize(solver, environment);
            },
            py::arg("solver"),
            py::arg("environment"),
            "Optimize the GHF wave function model.");
}


/**
 *  Add Python bindings for GHF QCMethods.
 */
void bindQCMethodsGHF(py::module& module) {

    // Provide Python bindings for real-valued GHF.
    py::class_<QCMethod::GHF<double>> py_QCMethodGHF_d {module, "GHF_d", "The (real) generalized Hartree-Fock quantum chemical method."};

    bindQCMethodGHFInterface(py_QCMethodGHF_d);


    // Provide Python bindings for complex-valued GHF.
    py::class_<QCMethod::GHF<complex>> py_QCMethodGHF_cd {module, "GHF_cd", "The (complex) generalized Hartree-Fock quantum chemical method."};

    bindQCMethodGHFInterface(py_QCMethodGHF_cd);
}


}  // namespace gqcpy