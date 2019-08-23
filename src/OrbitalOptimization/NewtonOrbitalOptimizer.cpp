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
#include "OrbitalOptimization/NewtonOrbitalOptimizer.hpp"

#include "Mathematical/Optimization/step.hpp"

#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>


namespace GQCP {



/*
 *  CONSTRUCTORS
 */

/*
 *  @param hessian_modifier                 the modifier functor that should be used when an indefinite Hessian is encountered
 *  @param convergence_threshold            the threshold used to check for convergence
 *  @param maximum_number_of_iterations     the maximum number of iterations that may be used to achieve convergence
*/
NewtonOrbitalOptimizer::NewtonOrbitalOptimizer(std::shared_ptr<BaseHessianModifier> hessian_modifier, const double convergence_threshold, const size_t maximum_number_of_iterations) :
    hessian_modifier (hessian_modifier),
    BaseOrbitalOptimizer(convergence_threshold, maximum_number_of_iterations)
{}



/*
 *  PUBLIC OVERRIDDEN METHODS
 */

/**
 *  Prepare this object (i.e. the context for the orbital optimization algorithm) to be able to check for convergence
 */
void NewtonOrbitalOptimizer::prepareConvergenceChecking(const HamiltonianParameters<double>& ham_par) {

    this->prepareOrbitalDerivativesCalculation(ham_par);

    // All Newton-based orbital optimizers need to calculate a gradient and Hessian
    this->gradient = this->calculateGradientVector(ham_par);
    this->hessian = this->calculateHessianMatrix(ham_par);
}


/**
 *  Determine if the algorithm has converged or not
 *  Specifically for the Newton-step based algorithms, this function
 *      - computes the gradient and checks its norm for convergence
 *      - if the gradient is zero, the Hessian is calculated and positive definiteness is checked
 * 
 *  @param ham_par      the current Hamiltonian parameters
 * 
 *  @return if the algorithm is considered to be converged
 */
bool NewtonOrbitalOptimizer::checkForConvergence(const HamiltonianParameters<double>& ham_par) const {

    // Check for convergence on the norm
    if (this->gradient.norm() < this->convergence_threshold) {
        if (this->newtonStepIsWellDefined()) {  // needs this->hessian
            return true;
        } else {
            return false;
        }
    }

    else {
        return false;
    }
}



/**
 *  Produce a new rotation matrix by either
 *      - continuing in the direction of the largest (in absolute value) non-conforming eigenvalue (i.e. the smallest (negative) eigenvalue for minimization algorithms and the largest (positive) eigenvalue for maximization algorithms)
 *      - using the Newton step if it is well-defined
 * 
 *  @param ham_par      the current Hamiltonian parameters
 * 
 *  @return a unitary matrix that will be used to rotate the current Hamiltonian parameters into the next iteration
 */
SquareMatrix<double> NewtonOrbitalOptimizer::calculateNewRotationMatrix(const HamiltonianParameters<double>& ham_par) const {

    // The general goal of this function is to:
    //      1) determine the free orbital rotation generators, using gradient and Hessian information
    //      2) determine the full orbital rotation generators, by also including any redundant parameters
    //      3) calculate the unitary rotation matrix from the full orbital rotation generators

   const auto full_kappa = this->calculateNewFullOrbitalGenerators(ham_par);  // should internally calculate the free orbital rotation generators

    return full_kappa.calculateRotationMatrix();  // matrix exponential
}



/* 
 *  PUBLIC METHODS
 */ 

/**
 *  @param ham_par      the current Hamiltonian parameters
 * 
 *  @return the current orbital gradient as a vector
 */
VectorX<double> NewtonOrbitalOptimizer::calculateGradientVector(const HamiltonianParameters<double>& ham_par) const {
    return this->calculateGradientMatrix(ham_par).pairWiseStrictReduce();
}


/**
 *  @param ham_par      the current Hamiltonian parameters
 * 
 *  @return the current orbital Hessian as a matrix
 */
SquareMatrix<double> NewtonOrbitalOptimizer::calculateHessianMatrix(const HamiltonianParameters<double>& ham_par) const {
    return this->calculateHessianTensor(ham_par).pairWiseStrictReduce();
}


/**
 *  @return if a Newton step would be well-defined, i.e. the Hessian is positive definite
 */
bool NewtonOrbitalOptimizer::newtonStepIsWellDefined() const {

    // Can only produce a well-defined descending Newton step if the Hessian is positive definite
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> hessian_diagonalizer (this->hessian);
    if (hessian_diagonalizer.eigenvalues()(0) < -1.0e-04) {  // not enough negative curvature to continue; can we change this to -this->convergence_threshold?
        return false;
    } else {
        return true;
    }
}


/**
 *  If the Newton step is ill-defined, examine the Hessian and produce a new direction from it: the eigenvector that corresponds to the smallest (negative) eigenvalue of the Hessian
 * 
 *  @return the new direction from the Hessian if the Newton step is ill-defined
 */
VectorX<double> NewtonOrbitalOptimizer::directionFromIndefiniteHessian() const {
    
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> hessian_diagonalizer (this->hessian);
    return hessian_diagonalizer.eigenvectors().col(0);
}


/**
 *  Use gradient and Hessian information to determine a new direction for the 'free' orbital rotation generators kappa. Note that a distinction is made between 'free' generators, i.e. those that are calculated from the gradient and Hessian information and the 'full' generators, which also include the redundant parameters (that can be set to zero). The 'full' generators are used to calculate the total rotation matrix using the matrix exponential
 * 
 *  @param ham_par      the current Hamiltonian parameters
 * 
 *  @return the new free orbital generators
 */
OrbitalRotationGenerators NewtonOrbitalOptimizer::calculateNewFreeOrbitalGenerators(const HamiltonianParameters<double>& ham_par) const {

    // If the norm hasn't converged, use the Newton step
    if (this->gradient.norm() > this->convergence_threshold) {

        const size_t dim = this->gradient.size();
        const VectorFunction gradient_function = [this] (const VectorX<double>& x) { return this->gradient; };

        auto modified_hessian = this->hessian;
        if (!this->newtonStepIsWellDefined()) {
            modified_hessian = this->hessian_modifier->operator()(this->hessian);
        }
        const MatrixFunction hessian_function = [&modified_hessian] (const VectorX<double>& x) { return modified_hessian; };

        return OrbitalRotationGenerators(newtonStep(VectorX<double>::Zero(dim), gradient_function, hessian_function));  // with only the free parameters
    }

    else {  // the gradient has converged but the Hessian is indefinite, so we have to 'push the algorithm over'
        // We're sure that if the program reaches this step, the Newton step is ill-defined
        return OrbitalRotationGenerators(this->directionFromIndefiniteHessian());
    }
}


}  // namespace GQCP
