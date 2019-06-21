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
#include "FockSpace/BaseFrozenCoreFockSpace.hpp"


namespace GQCP {


/*
 *  CONSTRUCTORS
 */

/**
 *  @param fock_space                    shared pointer to active (non-frozen core) Fock space
 *  @param X                             the number of frozen orbitals
 */
BaseFrozenCoreFockSpace::BaseFrozenCoreFockSpace(std::shared_ptr<GQCP::BaseFockSpace> fock_space, size_t X) :
    BaseFockSpace(fock_space->get_K()+X, fock_space->get_dimension()),
    active_fock_space (std::move(fock_space)),
    X (X)
{}



/*
 *  PUBLIC METHODS
 */

/**
 *  Evaluate the operator in a dense matrix
 *
 *  @param one_op               the one-electron operator in an orthonormal orbital basis to be evaluated in the Fock space
 *  @param diagonal_values      bool to indicate if diagonal values will be calculated
 *
 *  @return the operator's evaluation in a dense matrix with the dimensions of the Fock space
 */
SquareMatrix<double> BaseFrozenCoreFockSpace::evaluateOperatorDense(const OneElectronOperator<double>& one_op, bool diagonal_values) const {

    // Freeze Hamiltonian parameters
    OneElectronOperator<double> frozen_one_op = BaseFrozenCoreFockSpace::freezeOperator(one_op, this->X);

    // Evaluate the frozen operator in the active space
    auto evaluation = this->active_fock_space->evaluateOperatorDense(frozen_one_op, diagonal_values);

    if (diagonal_values) {
        // Diagonal correction
        const auto frozen_core_diagonal = BaseFrozenCoreFockSpace::frozenCoreDiagonal(one_op, this->X, this->active_fock_space->get_dimension());
        evaluation += frozen_core_diagonal.asDiagonal();
    }

    return evaluation;
}


/**
 *  Evaluate the operator in a sparse matrix
 *
 *  @param one_op               the one-electron operator in an orthonormal orbital basis to be evaluated in the Fock space
 *  @param diagonal_values      bool to indicate if diagonal values will be calculated
 *
 *  @return the operator's evaluation in a sparse matrix with the dimensions of the Fock space
 */
Eigen::SparseMatrix<double> BaseFrozenCoreFockSpace::evaluateOperatorSparse(const OneElectronOperator<double>& one_op,
                                                   bool diagonal_values) const {

    // Freeze the operator
    OneElectronOperator<double> frozen_one_op = BaseFrozenCoreFockSpace::freezeOperator(one_op, this->X);

    // Evaluate the frozen operator in the active space
    auto evaluation = this->active_fock_space->evaluateOperatorSparse(frozen_one_op, diagonal_values);

    if (diagonal_values) {
        // Diagonal correction
        const auto frozen_core_diagonal = BaseFrozenCoreFockSpace::frozenCoreDiagonal(one_op, this->X, this->active_fock_space->get_dimension());
        evaluation += frozen_core_diagonal.asDiagonal();
    }

    return evaluation;
}


/**
 *  Evaluate the operator in a dense matrix
 *
 *  @param two_op               the two-electron operator in an orthonormal orbital basis to be evaluated in the Fock space
 *  @param diagonal_values      bool to indicate if diagonal values will be calculated
 *
 *  @return the operator's evaluation in a dense matrix with the dimensions of the Fock space
 */
SquareMatrix<double> BaseFrozenCoreFockSpace::evaluateOperatorDense(const TwoElectronOperator<double>& two_op, bool diagonal_values) const {

    // Freeze the operators
    const auto frozen_ops = BaseFrozenCoreFockSpace::freezeOperator(two_op, this->X);

    // Evaluate the frozen operator in the active space
    auto evaluation = this->active_fock_space->evaluateOperatorDense(frozen_ops.one_op, diagonal_values);
    evaluation += this->active_fock_space->evaluateOperatorDense(frozen_ops.two_op, diagonal_values);

    if (diagonal_values) {
        // Diagonal correction
        const auto frozen_core_diagonal = BaseFrozenCoreFockSpace::frozenCoreDiagonal(two_op, this->X, this->active_fock_space->get_dimension());
        evaluation += frozen_core_diagonal.asDiagonal();
    }

    return evaluation;
}


/**
 *  Evaluate the operator in a sparse matrix
 *
 *  @param two_op               the two-electron operator in an orthonormal orbital basis to be evaluated in the Fock space
 *  @param diagonal_values      bool to indicate if diagonal values will be calculated
 *
 *  @return the operator's evaluation in a sparse matrix with the dimensions of the Fock space
 */
Eigen::SparseMatrix<double> BaseFrozenCoreFockSpace::evaluateOperatorSparse(const TwoElectronOperator<double>& two_op,
                                                   bool diagonal_values) const {

    // Freeze the operators
    const auto frozen_ops = BaseFrozenCoreFockSpace::freezeOperator(two_op, this->X);

    // Evaluate the frozen operator in the active space
    auto evaluation = this->active_fock_space->evaluateOperatorSparse(frozen_ops.one_op, diagonal_values);
    evaluation += this->active_fock_space->evaluateOperatorSparse(frozen_ops.two_op, diagonal_values);

    if (diagonal_values) {
        // Diagonal correction
        const auto frozen_core_diagonal = BaseFrozenCoreFockSpace::frozenCoreDiagonal(two_op, this->X, this->active_fock_space->get_dimension());
        evaluation += frozen_core_diagonal.asDiagonal();
    }

    return evaluation;
}


/**
 *  Evaluate the Hamiltonian in a dense matrix
 *
 *  @param ham_par              Hamiltonian parameters in an orthonormal orbital basis to be evaluated in the Fock space
 *  @param diagonal_values      bool to indicate if diagonal values will be calculated
 *
 *  @return the Hamiltonian's evaluation in a dense matrix with the dimensions of the Fock space
 */
SquareMatrix<double> BaseFrozenCoreFockSpace::evaluateOperatorDense(const HamiltonianParameters<double>& ham_par,
                                           bool diagonal_values) const  {
    // Freeze the operators
    const auto frozen_ham_par = BaseFrozenCoreFockSpace::freezeOperator(ham_par, this->X);

    // Evaluate the frozen operator in the active space
    auto evaluation = this->active_fock_space->evaluateOperatorDense(frozen_ham_par, diagonal_values);

    if (diagonal_values) {
        // Diagonal correction
        const auto frozen_core_diagonal = BaseFrozenCoreFockSpace::frozenCoreDiagonal(ham_par, this->X, this->active_fock_space->get_dimension());
        evaluation += frozen_core_diagonal.asDiagonal();
    }

    return evaluation;
}


/**
 *  Evaluate the Hamiltonian in a sparse matrix
 *
 *  @param ham_par              Hamiltonian parameters in an orthonormal orbital basis to be evaluated in the Fock space
 *  @param diagonal_values      bool to indicate if diagonal values will be calculated
 *
 *  @return the Hamiltonian's evaluation in a sparse matrix with the dimensions of the Fock space
 */
Eigen::SparseMatrix<double> BaseFrozenCoreFockSpace::evaluateOperatorSparse(const HamiltonianParameters<double>& ham_par,
                                                   bool diagonal_values) const  {
    // Freeze the operators
    const auto frozen_ham_par = BaseFrozenCoreFockSpace::freezeOperator(ham_par, this->X);

    // Evaluate the frozen operator in the active space
    auto evaluation = this->active_fock_space->evaluateOperatorSparse(frozen_ham_par, diagonal_values);

    if (diagonal_values) {
        // Diagonal correction
        const auto frozen_core_diagonal = BaseFrozenCoreFockSpace::frozenCoreDiagonal(ham_par, this->X, this->active_fock_space->get_dimension());
        evaluation += frozen_core_diagonal.asDiagonal();
    }

    return evaluation;
}


/**
 *  Evaluate the diagonal of the operator
 *
 *  @param one_op               the one-electron operator in an orthonormal orbital basis to be evaluated in the Fock space
 *
 *  @return the operator's diagonal evaluation in a vector with the dimension of the Fock space
 */
VectorX<double> BaseFrozenCoreFockSpace::evaluateOperatorDiagonal(const OneElectronOperator<double>& one_op) const {

    const auto frozen_op = BaseFrozenCoreFockSpace::freezeOperator(one_op, this->X);

    // Calculate diagonal in the active space with the "frozen" Hamiltonian parameters
    const auto diagonal = this->active_fock_space->evaluateOperatorDiagonal(frozen_op);

    // Calculate diagonal for the frozen orbitals
    const auto frozen_core_diagonal = BaseFrozenCoreFockSpace::frozenCoreDiagonal(one_op, this->X, this->active_fock_space->get_dimension());

    return diagonal + frozen_core_diagonal;
};


/**
 *  Evaluate the diagonal of the operator
 *
 *  @param two_op               the two-electron operator in an orthonormal orbital basis to be evaluated in the Fock space
 *
 *  @return the operator's diagonal evaluation in a vector with the dimension of the Fock space
 */
VectorX<double> BaseFrozenCoreFockSpace::evaluateOperatorDiagonal(const TwoElectronOperator<double>& two_op) const {

    const auto frozen_ops = BaseFrozenCoreFockSpace::freezeOperator(two_op, this->X);

    // Calculate diagonal in the active space with the "frozen" Hamiltonian parameters
    auto diagonal = this->active_fock_space->evaluateOperatorDiagonal(frozen_ops.two_op);
    diagonal += this->active_fock_space->evaluateOperatorDiagonal(frozen_ops.one_op);

    // Calculate diagonal for the frozen orbitals
    const auto frozen_core_diagonal = BaseFrozenCoreFockSpace::frozenCoreDiagonal(two_op, this->X, this->active_fock_space->get_dimension());

    return diagonal + frozen_core_diagonal;
}


/**
 *  Evaluate the diagonal of the Hamiltonian
 *
 *  @param ham_par              Hamiltonian parameters in an orthonormal orbital basis to be evaluated in the Fock space
 *
 *  @return the Hamiltonian's diagonal evaluation in a vector with the dimension of the Fock space
 */
VectorX<double> BaseFrozenCoreFockSpace::evaluateOperatorDiagonal(const HamiltonianParameters<double>& ham_par) const {

    const auto frozen_ham_par = BaseFrozenCoreFockSpace::freezeOperator(ham_par, this->X);

    // Calculate diagonal in the active space with the "frozen" Hamiltonian parameters
    const auto diagonal = this->active_fock_space->evaluateOperatorDiagonal(frozen_ham_par);

    // Calculate diagonal for the frozen orbitals
    const auto frozen_core_diagonal = BaseFrozenCoreFockSpace::frozenCoreDiagonal(ham_par, this->X, this->active_fock_space->get_dimension());

    return diagonal + frozen_core_diagonal;
}


/**
 *  @param one_op       the one-electron operator in an orthonormal orbital basis
 *  @param X            the number of frozen orbitals
 *
 *  @return 'frozen' one-electron operator which cover evaluations from the active and inactive orbitals
 */
OneElectronOperator<double> BaseFrozenCoreFockSpace::freezeOperator(const OneElectronOperator<double>& one_op, size_t X) {

    size_t K_active = one_op.cols() - X;
    return OneElectronOperator<double>(one_op.block(X, X, K_active, K_active));
}


/**
 *  @param two_op       the two-electron operator in an orthonormal orbital basis
 *  @param X            the number of frozen orbitals
 *
 *  @return 'frozen' two-electron operators as a struct of a one- and two-electron operator which cover evaluations from the active and inactive orbitals
 */
FrozenOperators BaseFrozenCoreFockSpace::freezeOperator(const TwoElectronOperator<double>& two_op, size_t X) {

    size_t K_active = two_op.dimension(0) - X;
    OneElectronOperator<double> frozen_one_op = OneElectronOperator<double>::Zero(K_active, K_active);
    auto frozen_two_op = TwoElectronOperator<double>::FromBlock(two_op, X, X, X, X);

    // Frozen two-electron integrals can be rewritten partially as one electron integrals.
    for (size_t i = 0; i < K_active; i++) {  // iterate over the active orbitals
        size_t q = i + X;  // map active orbitals indexes to total orbital indexes (those including the frozen orbitals)

        for (size_t l = 0; l < X; l++) {  // iterate over the frozen orbitals
            frozen_one_op(i,i) += two_op(q,q,l,l) + two_op(l,l,q,q) - two_op(q,l,l,q)/2 - two_op(l,q,q,l)/2;
        }

        for (size_t j = i+1; j < K_active; j++) {  // iterate over the active orbitals
            size_t p = j + X;  // map active orbitals indexes to total orbital indexes (those including the frozen orbitals)

            for (size_t l = 0; l < X; l++) {  // iterate over the frozen orbitals
                frozen_one_op(i,j) += two_op(q,p,l,l) + two_op(l,l,q,p) - two_op(q,l,l,p)/2 - two_op(l,p,q,l)/2;
                frozen_one_op(j,i) += two_op(p,q,l,l) + two_op(l,l,p,q) - two_op(p,l,l,q)/2 - two_op(l,q,p,l)/2;
            }
        }
    }

    return {frozen_one_op, frozen_two_op};
}


/*
 *  STATIC PUBLIC METHODS
 */

/**
 *  @param ham_par      the Hamiltonian parameters in an orthonormal orbital basis
 *  @param X            the number of frozen orbitals
 *
 *  @return a set of 'frozen' Hamiltonian parameters which cover two-electron integral evaluations from the active and inactive orbitals
 *  (see https://drive.google.com/file/d/1Fnhv2XyNO9Xw9YDoJOXU21_6_x2llntI/view?usp=sharing)
 */
HamiltonianParameters<double> BaseFrozenCoreFockSpace::freezeOperator(const HamiltonianParameters<double>& ham_par, size_t X) {

    size_t K_active = ham_par.get_K() - X;  // number of non-frozen orbitals

    const auto frozen_components_g = BaseFrozenCoreFockSpace::freezeOperator(ham_par.get_g(), X);
    OneElectronOperator<double> S = BaseFrozenCoreFockSpace::freezeOperator(ham_par.get_S(), X);  // active
    OneElectronOperator<double> h = BaseFrozenCoreFockSpace::freezeOperator(ham_par.get_h(), X) + frozen_components_g.one_op;  // active

    std::shared_ptr<AOBasis> ao_basis;  // nullptr
    TwoElectronOperator<double> g = frozen_components_g.two_op;
    SquareMatrix<double> T = ham_par.get_T_total().block(X, X, K_active, K_active);

    return HamiltonianParameters<double>(ao_basis, S, h, g, T);
}


/**
 *  @param one_op       the one-electron operator in an orthonormal orbital basis
 *  @param X            the number of frozen orbitals
 *
 *  @return the operator diagonal from strictly evaluating the frozen orbitals in the Fock space
 */
VectorX<double> BaseFrozenCoreFockSpace::frozenCoreDiagonal(const OneElectronOperator<double>& one_op, size_t X,  size_t dimension) {

    // The diagonal value for the frozen orbitals is the same for each ONV
    double value = 0;
    for (size_t i = 0; i < X; i++) {
        value += 2*one_op(i,i);
    }

    return  VectorX<double>::Constant(dimension, value);
}


/**
 *  @param two_op       the two-electron operator in an orthonormal orbital basis
 *  @param X            the number of frozen orbitals
 *
 *  @return the operator diagonal from strictly evaluating the frozen orbitals in the Fock space
 */
VectorX<double> BaseFrozenCoreFockSpace::frozenCoreDiagonal(const TwoElectronOperator<double>& two_op, size_t X, size_t dimension) {

    // The diagonal value for the frozen orbitals is the same for each ONV
    double value = 0;
    for (size_t i = 0; i < X; i++) {
        value += two_op(i,i,i,i);

        for (size_t j = i+1; j < X; j++) {
            value += 2*two_op(i,i,j,j) + 2*two_op(j,j,i,i) - two_op(j,i,i,j) - two_op(i,j,j,i);
        }
    }

    return  VectorX<double>::Constant(dimension, value);
}


/**
 *  @param ham_par      the Hamiltonian parameters in an orthonormal orbital basis
 *  @param X            the number of frozen orbitals
 *
 *  @return the Hamiltonian diagonal from strictly evaluating the frozen orbitals in the Fock space
 */
VectorX<double> BaseFrozenCoreFockSpace::frozenCoreDiagonal(const HamiltonianParameters<double>& ham_par, size_t X,  size_t dimension) {
    return BaseFrozenCoreFockSpace::frozenCoreDiagonal(ham_par.get_h(), X, dimension) + BaseFrozenCoreFockSpace::frozenCoreDiagonal(ham_par.get_g(), X, dimension);
}


}  // namespace GQCP