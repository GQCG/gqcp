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

#define BOOST_TEST_MODULE "QCModel::RHF"

#include <boost/test/unit_test.hpp>

#include "QCModel/HF/RHF.hpp"


/**
 *  Check for valid and invalid arguments for GQCP::QCModel::RHF::calculateOrthonormalBasis1RDM()
 */
BOOST_AUTO_TEST_CASE(RHF_1RDM_invalid_argument) {

    const size_t K = 5;          // the number of spatial orbitals
    const size_t N_invalid = 3;  // the number of electrons must be even
    const size_t N_valid = 4;

    BOOST_CHECK_THROW(GQCP::QCModel::RHF<double>::calculateOrthonormalBasis1RDM(K, N_invalid), std::invalid_argument);
    BOOST_CHECK_NO_THROW(GQCP::QCModel::RHF<double>::calculateOrthonormalBasis1RDM(K, N_valid));
}


/**
 *  Check if GQCP::QCModel::RHF::calculateOrthonormalBasis1RDM() yields a correct 1-RDM for an example
 */
BOOST_AUTO_TEST_CASE(RHF_1RDM_matrix) {

    const size_t K = 5;  // the number of spatial orbitals
    const size_t N = 6;  // the number of electrons
    GQCP::OneRDM<double> D_ref {K};
    // clang-format off
    D_ref << 2, 0, 0, 0, 0,
             0, 2, 0, 0, 0,
             0, 0, 2, 0, 0,
             0, 0, 0, 0, 0,
             0, 0, 0, 0, 0;
    // clang-format on

    BOOST_CHECK(GQCP::QCModel::RHF<double>::calculateOrthonormalBasis1RDM(K, N).isApprox(D_ref));
}


/**
 *  Check if the RHF HOMO and LUMO indices are correctly implemented
 */
BOOST_AUTO_TEST_CASE(HOMO_LUMO_index) {

    // For K=7 and N=10, the index of the HOMO should be 4
    const size_t K = 7;   // number of spatial orbitals
    const size_t N = 10;  // number of electrons

    BOOST_CHECK_EQUAL(GQCP::QCModel::RHF<double>::homoIndex(N), 4);
    BOOST_CHECK_EQUAL(GQCP::QCModel::RHF<double>::lumoIndex(K, N), 5);

    BOOST_CHECK_THROW(GQCP::QCModel::RHF<double>::homoIndex(N + 1), std::invalid_argument);
    BOOST_CHECK_THROW(GQCP::QCModel::RHF<double>::lumoIndex(K, N + 1), std::invalid_argument);
}

/**
 *  Check if the methods for returning spinorbital energies are correctly implemented.
 */
BOOST_AUTO_TEST_CASE(spinorbitalEnergies) {

    // Set up toy UHF model parameters.
    const size_t K = 2;
    const GQCP::TransformationMatrix<double> C = GQCP::TransformationMatrix<double>::Identity(K, K);
    GQCP::VectorX<double> orbital_energies {K};
    orbital_energies << -0.5, 0.5;

    GQCP::QCModel::RHF<double> rhf_parameters {1, orbital_energies, C};


    // Provide reference values and check the results.
    GQCP::VectorX<double> ref_spinorbital_energies_interleaved {2 * K};
    ref_spinorbital_energies_interleaved << -0.5, -0.5, 0.5, 0.5;
    BOOST_CHECK(rhf_parameters.spinOrbitalEnergiesInterleaved().isApprox(ref_spinorbital_energies_interleaved, 1.0e-12));

    GQCP::VectorX<double> ref_spinorbital_energies_blocked {2 * K};
    ref_spinorbital_energies_blocked << -0.5, 0.5, -0.5, 0.5;
    BOOST_CHECK(rhf_parameters.spinOrbitalEnergiesBlocked().isApprox(ref_spinorbital_energies_blocked, 1.0e-12));
}
