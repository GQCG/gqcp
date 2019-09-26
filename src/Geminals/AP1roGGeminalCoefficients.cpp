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
#include "Geminals/AP1roGGeminalCoefficients.hpp"

#include "FockSpace/FockSpace.hpp"
#include "Geminals/APIGGeminalCoefficients.hpp"
#include "Utilities/miscellaneous.hpp"


namespace GQCP {


/*
 *  CONSTRUCTORS
 */

/**
 *  Default constructor setting everything to zero
 */
AP1roGGeminalCoefficients::AP1roGGeminalCoefficients() :
    AP1roGVariables()
{}


/**
 *  @param g        the geminal coefficients in a vector representation that is in row-major storage
 *
 *  @param N_P      the number of electron pairs (= the number of geminals)
 *  @param K        the number of spatial orbitals
 */
AP1roGGeminalCoefficients::AP1roGGeminalCoefficients(const VectorX<double>& g, size_t N_P, size_t K) :
    AP1roGVariables(g, N_P, K)
{}


/**
 *  Constructor that sets the geminal coefficients to zero
 *
 *  @param N_P      the number of electron pairs (= the number of geminals)
 *  @param K        the number of spatial orbitals
 */
AP1roGGeminalCoefficients::AP1roGGeminalCoefficients(size_t N_P, size_t K) :
    AP1roGVariables(N_P, K)
{}



/*
 *  NAMED CONSTRUCTORS
 */

/**
 *  @param sq_hamiltonian       the Hamiltonian expressed in an orthonormal basis
 *  @param N_P                  the number of orbitals
 *
 *  @return the AP1roG geminal coefficients in the weak interaction limit
 */
AP1roGGeminalCoefficients AP1roGGeminalCoefficients::WeakInteractionLimit(const SQHamiltonian<double>& sq_hamiltonian, size_t N_P) {

    const auto K = sq_hamiltonian.dimension();
    const auto number_of_geminal_coefficients = AP1roGGeminalCoefficients::numberOfGeminalCoefficients(N_P, K);
    const auto& h = sq_hamiltonian.core().parameters();  // core Hamiltonian integrals
    const auto& g = sq_hamiltonian.twoElectron().parameters();  // two-electron integrals

    // Provide the weak interaction limit values for the geminal coefficients
    VectorX<double> g_vector = VectorX<double>::Zero(number_of_geminal_coefficients);  // in row-major representation
    for (size_t mu = 0; mu < number_of_geminal_coefficients; mu++) {
        size_t i = GQCP::matrixIndexMajor(mu, K, N_P);
        size_t a = GQCP::matrixIndexMinor(mu, K, N_P);

        g_vector(mu) = - g(a,i,a,i) / (2 * (h(a,a) - h(i,i)));
    }


    return AP1roGGeminalCoefficients::FromRowMajor(g_vector, N_P, K);
}


/**
 *  @param g        the geminal coefficients in a vector representation that is in column-major storage
 *
 *  @param N_P      the number of electron pairs (= the number of geminals)
 *  @param K        the number of spatial orbitals
 */
AP1roGGeminalCoefficients AP1roGGeminalCoefficients::FromColumnMajor(const VectorX<double>& g, size_t N_P, size_t K) {

    // Loop over the row indices and column indices to place the given column-major elements into a row-major vector
    const auto number_of_geminal_coefficients = AP1roGGeminalCoefficients::numberOfGeminalCoefficients(N_P, K);
    VectorX<double> g_row_major = VectorX<double>::Zero(number_of_geminal_coefficients);
    for (size_t i = 0; i < N_P; i++) {  // i labels rows
        for (size_t j = 0; j < K-N_P; j++) {  // j labels columns
            size_t row_major_index = i * (K-N_P) + j;
            size_t column_major_index = j * N_P + i;

            g_row_major(row_major_index) = g(column_major_index);  // access the column-major vector and place them in the row-major vector
        }
    }

    return AP1roGGeminalCoefficients::FromRowMajor(g_row_major, N_P, K);
}


/**
 *  @param g        the geminal coefficients in a vector representation that is in row-major storage
 *
 *  @param N_P      the number of electron pairs (= the number of geminals)
 *  @param K        the number of spatial orbitals
 */
AP1roGGeminalCoefficients AP1roGGeminalCoefficients::FromRowMajor(const VectorX<double>& g, const size_t N_P, const size_t K) {

    return AP1roGGeminalCoefficients(g, N_P, K);
}



/*
 *  DESTRUCTOR
 */

AP1roGGeminalCoefficients::~AP1roGGeminalCoefficients() {}



/*
 *  STATIC PUBLIC METHODS
 */

/**
 *  @param N_P      the number of electron pairs (= the number of geminals)
 *  @param K        the number of spatial orbitals
 *
 *  @return the number of 'free' geminal coefficients
 */
size_t AP1roGGeminalCoefficients::numberOfGeminalCoefficients(size_t N_P, size_t K) {

    return AP1roGVariables::numberOfVariables(N_P, K);
}



/*
 *  PUBLIC METHODS
 */

/**
 *  @return the geminal coefficients in matrix form
 */
MatrixX<double> AP1roGGeminalCoefficients::asMatrix() const {

    // Initialize the geminal coefficient matrix
    MatrixX<double> G = MatrixX<double>::Zero(this->N_P, this->K);

    // The AP1roG coefficients are the identity matrix in the leftmost (N_P x N_P)-block
    G.topLeftCorner(this->N_P, this->N_P) = MatrixX<double>::Identity(this->N_P, this->N_P);

    // Set the right AP1roG coefficient block
    using RowMajorMatrixXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    Eigen::RowVectorXd x_row = this->x;
    RowMajorMatrixXd B = Eigen::Map<RowMajorMatrixXd, Eigen::RowMajor>(x_row.data(), this->N_P, this->K-this->N_P);
    G.topRightCorner(this->N_P, this->K-this->N_P) = B;

    return G;
}


/**
 *  @param onv      the ONV that is being projected on
 *
 *  @return the overlap of the AP1roG wave function with the given on, i.e. the projection of the APIG wave function onto that ONV
 */
double AP1roGGeminalCoefficients::overlap(const ONV& onv) const {

    // For an AP1roG, we use a simplification for singly and doubly pair-excited ONVs


    FockSpace fock_space (this->K, this->N_P);  // the DOCI Fock space
    ONV reference = fock_space.makeONV(0);

    if (onv.countNumberOfDifferences(reference) == 0) {  // no excitations
        return 1.0;
    }

    else if (onv.countNumberOfDifferences(reference) == 2) {  // one pair excitation

        size_t i = reference.findDifferentOccupations(onv)[0];
        size_t a = onv.findDifferentOccupations(reference)[0];

        return this->operator()(i, a);
    }

    else if (onv.countNumberOfDifferences(reference) == 4) {  // two pair excitations

        auto different_occupied = reference.findDifferentOccupations(onv);
        auto different_virtual = onv.findDifferentOccupations(reference);

        size_t i = different_occupied[0];
        size_t j = different_occupied[1];
        size_t a = different_virtual[0];
        size_t b = different_virtual[1];

        return this->operator()(i, a) * this->operator()(j, b) + this->operator()(j, a) * this->operator()(i, b);
    }

    else {

        APIGGeminalCoefficients APIG (this->asMatrix());
        return APIG.overlap(onv);
    }
}


}  // namespace GQCP
