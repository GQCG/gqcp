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

#pragma once


#include "Mathematical/Algorithm/Step.hpp"
#include "QCMethod/HF/UHF/UHFSCFEnvironment.hpp"
#include "QCModel/HF/UHF.hpp"


namespace GQCP {


/**
 *  An iteration step that calculates the current UHF Fock matrices (expressed in the scalar/AO basis) from the current density matrices.
 * 
 *  @tparam _Scalar              the scalar type used to represent the expansion coefficient/elements of the transformation matrix
 */
template <typename _Scalar>
class UHFFockMatrixCalculation:
    public Step<UHFSCFEnvironment<_Scalar>> {

public:
    using Scalar = _Scalar;
    using Environment = UHFSCFEnvironment<Scalar>;


public:
    /*
     *  OVERRIDDEN PUBLIC METHODS
     */

    /**
     *  Calculate the current UHF Fock matrices (expressed in the scalar/AO basis) and place them in the environment
     * 
     *  @param environment              the environment that acts as a sort of calculation space
     */
    void execute(Environment& environment) override {

        const auto& P_alpha = environment.density_matrices_alpha.back();  // the most recent alpha density matrix
        const auto& P_beta = environment.density_matrices_beta.back();    // the most recent beta density matrices

        const auto F_alpha = QCModel::UHF<double>::calculateScalarBasisFockMatrix(Spin::alpha, P_alpha, P_beta, environment.sq_hamiltonian);
        const auto F_beta = QCModel::UHF<double>::calculateScalarBasisFockMatrix(Spin::beta, P_alpha, P_beta, environment.sq_hamiltonian);

        environment.fock_matrices_alpha.push_back(F_alpha.parameters());
        environment.fock_matrices_beta.push_back(F_beta.parameters());
    }
};


}  // namespace GQCP
