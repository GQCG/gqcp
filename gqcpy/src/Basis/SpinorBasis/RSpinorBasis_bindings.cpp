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
#include "Basis/SpinorBasis/RSpinorBasis.hpp"
#include "Molecule/Molecule.hpp"
#include "Operator/FirstQuantized/Operator.hpp"

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace py = pybind11;


namespace gqcpy {


void bindRSpinorBasis(py::module& module) {
    py::class_<GQCP::RSpinorBasis<double, GQCP::GTOShell>>(module, "RSpinorBasis", "A class that represents a real, restricted spinor basis with underlying GTO shells.")

        // CONSTRUCTORS

        .def(py::init<const GQCP::Molecule&, const std::string&>(),
             py::arg("molecule"),
             py::arg("basisset_name"))


        // INHERITED METHODS

        .def(
            "coefficientMatrix",
            [](const GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis) {
                return spinor_basis.coefficientMatrix();
            },
            "Return the transformation matrix between the scalar basis and the current orbitals.")

        .def(
            "isOrthonormal",
            [](const GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis, const double precision) {
                return spinor_basis.isOrthonormal(precision);
            },
            py::arg("precision") = 1.0e-08,
            "Return if this spinor basis is orthonormal within the given precision")

        .def(
            "lowdinOrthonormalizationMatrix",
            [](const GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis) {
                return spinor_basis.lowdinOrthonormalizationMatrix();
            },
            "Return the transformation matrix to the Löwdin basis: T = S_current^{-1/2}")

        .def(
            "lowdinOrthonormalize",
            [](GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis) {
                spinor_basis.lowdinOrthonormalize();
            },
            "Transform the spinor basis to the 'Löwdin basis', which is the orthonormal basis that we transform to with T = S^{-1/2}, where S is the current overlap matrix.")

        .def(
            "overlap",
            [](const GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis) {
                return spinor_basis.overlap();
            },
            "Return the overlap (one-electron) operator of this restricted spinor basis")

        .def(
            "rotate",
            [](GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis, const Eigen::MatrixXd& U) {
                spinor_basis.rotate(GQCP::TransformationMatrix<double>(U));
            },
            py::arg("U"),
            "Rotate the spinor basis to another one using the given unitary transformation matrix.")

        .def(
            "transform", [](GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis, const Eigen::MatrixXd& T) {
                spinor_basis.transform(GQCP::TransformationMatrix<double>(T));
            },
            py::arg("T"), "Transform the current spinor basis using a given transformation matrix")


        // PUBLIC METHODS

        .def(
            "calculateMullikenOperator", [](const GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis, const std::vector<size_t>& ao_list) {
                return spinor_basis.calculateMullikenOperator(ao_list);
            },
            "Return the Mulliken operator for a set of given AO indices.")

        .def(
            "numberOfSpatialOrbitals", [](GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis) {
                return spinor_basis.numberOfSpatialOrbitals();
            },
            "Return the number of different spatial orbitals that are used in this spinor basis.")

        .def(
            "quantizeCoulombRepulsionOperator", [](const GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis) {
                return spinor_basis.quantize(GQCP::Operator::Coulomb());
            },
            "Return the Coulomb repulsion operator expressed in this spinor basis.")

        .def(
            "quantizeDipoleOperator", [](const GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis, const GQCP::Vector<double, 3>& origin) {
                return spinor_basis.quantize(GQCP::Operator::ElectronicDipole(origin));
            },
            py::arg("origin") = GQCP::Vector<double, 3>::Zero(), "Return the electronic dipole operator expressed in this spinor basis.")

        .def(
            "quantizeKineticOperator", [](const GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis) {
                return spinor_basis.quantize(GQCP::Operator::Kinetic());
            },
            "Return the kinetic energy operator expressed in this spinor basis.")

        .def(
            "quantizeNuclearAttractionOperator", [](const GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis, const GQCP::Molecule& molecule) {
                return spinor_basis.quantize(GQCP::Operator::NuclearAttraction(molecule));
            },
            "Return the nuclear attraction operator expressed in this spinor basis.")

        .def(
            "quantizeOverlapOperator", [](const GQCP::RSpinorBasis<double, GQCP::GTOShell>& spinor_basis) {
                return spinor_basis.quantize(GQCP::Operator::Overlap());
            },
            "Return the overlap operator expressed in this spinor basis.");
}


}  // namespace gqcpy