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

#include "Basis/ScalarBasis/GTOShell.hpp"
#include "Basis/SpinorBasis/USpinOrbitalBasis.hpp"
#include "Molecule/Molecule.hpp"
#include "Utilities/aliases.hpp"
#include "gqcpy/include/interfaces.hpp"

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace gqcpy {


// Provide some shortcuts for frequent namespaces.
namespace py = pybind11;
using namespace GQCP;


/**
 *  Add Python bindings for some APIs related to `USpinOrbitalBasis`.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain APIs related to `USpinOrbitalBasis`.
 */
template <typename Class>
void bindUSpinOrbitalBasisInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;
    using Scalar = typename Type::ExpansionScalar;


    /*
     *  MARK: Constructors
     */

    py_class

        .def(py::init<const Molecule&, const std::string&>(),
             py::arg("molecule"),
             py::arg("basisset_name"))


        /*
         *  MARK: Named constructors
         */

        .def_static(
            "FromRestricted",
            [](const RSpinOrbitalBasis<Scalar, GTOShell>& r_spin_orbital_basis) {
                return USpinOrbitalBasis<Scalar, GTOShell>::FromRestricted(r_spin_orbital_basis);
            },
            py::arg("r_spin_orbital_basis"),
            "Create an unrestricted spin-orbital basis from a restricted spin-orbital basis, leading to alpha- and beta- coefficient matrices that are equal.")


        /*
         *  MARK: General information
         */

        .def(
            "numberOfSpinors",
            &USpinOrbitalBasis<Scalar, GTOShell>::numberOfSpinors,
            "Return the total number of spinors/spin-orbitals that this spin-orbital basis describes.");


    // Expose some spinor basis APIs to the Python class.
    bindSpinorBasisInterface(py_class);


    /*
     *  MARK: Quantization of first-quantized operators
     */

    // Expose some quantization API to the Python class;
    bindSpinorBasisQuantizationInterface(py_class);

    // Expose some Mulliken API to the Python class;
    bindSpinorBasisMullikenInterface(py_class);
}


/**
 *  Register `USpinOrbitalBasis_d` and `USpinOrbitalBasis_cd` to the gqcpy module and expose parts of their C++ interfaces to Python.
 * 
 *  @param module           The Pybind11 module in which the classes should be registered.
 */
void bindUSpinOrbitalBases(py::module& module) {

    // Define the Python class for `USpinOrbitalBasis_d`.
    py::class_<USpinOrbitalBasis<double, GTOShell>> py_USpinOrbitalBasis_d {module, "USpinOrbitalBasis_d", "A class that represents a real, unrestricted spinor basis with underlying GTO shells."};

    py_USpinOrbitalBasis_d
        .def(
            "quantizeDipoleOperator",
            [](const USpinOrbitalBasis<double, GTOShell>& spin_orbital_basis, const Vector<double, 3>& origin) {
                return spin_orbital_basis.quantize(Operator::ElectronicDipole(origin));
            },
            py::arg("origin") = Vector<double, 3>::Zero(),
            "Return the electronic dipole operator expressed in this spinor basis.");

    bindUSpinOrbitalBasisInterface(py_USpinOrbitalBasis_d);


    // Define the Python class for `USpinOrbitalBasis_cd`.
    py::class_<USpinOrbitalBasis<complex, GTOShell>> py_USpinOrbitalBasis_cd {module, "USpinOrbitalBasis_cd", "A class that represents a complex, unrestricted spinor basis with underlying GTO shells."};

    bindUSpinOrbitalBasisInterface(py_USpinOrbitalBasis_cd);
}


}  // namespace gqcpy