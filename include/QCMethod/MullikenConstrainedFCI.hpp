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
#ifndef GQCP_QCMETHOD_MULLIKENCONSTRAONEDFCI_HPP
#define GQCP_QCMETHOD_MULLIKENCONSTRAONEDFCI_HPP


#include "HamiltonianParameters/HamiltonianParameters.hpp"
#include "CISolver/CISolver.hpp"
#include "HamiltonianBuilder/FCI.hpp"
#include "FockSpace/ProductFockSpace.hpp"
#include "RDM/RDMCalculator.hpp"
#include "HamiltonianParameters/AtomicDecompositionParameters.hpp"
#include "HamiltonianBuilder/FrozenCoreFCI.hpp"

namespace GQCP {
namespace QCMethod {


/**
 *  A class that solves the FCI Hamiltonian given a perturbation in the form of a langragian multiplier and the Mulliken operator for a pre-specified set of basis functions
 */
class MullikenConstrainedFCI {
private:
    double solve_time;
    std::vector<size_t> basis_targets;
    Molecule molecule;
    HamiltonianParameters<double> ham_par;
    std::string basis_set;  // the basisset that should be used
    FrozenProductFockSpace fock_space = FrozenProductFockSpace(0, 0, 0, 0); // Default
    FrozenCoreFCI fci = FrozenCoreFCI(FrozenProductFockSpace(0, 0, 0, 0)); 
    OneElectronOperator<double> mulliken_operator;
    AtomicDecompositionParameters adp = AtomicDecompositionParameters();
    RDMCalculator rdm_calculator = RDMCalculator();

    bool are_solutions_available = false;

    // Molecular solutions
    std::vector<double> energy;
    std::vector<double> population; 
    std::vector<double> lambda;
    std::vector<double> entropy;

    // Decomposed solutions
    std::vector<double> A_fragment_energy;
    std::vector<double> A_fragment_self_energy;
    std::vector<double> B_fragment_energy;
    std::vector<double> B_fragment_self_energy;
    std::vector<double> interaction_energy;

    // Eigenvectors
    std::vector<VectorX<double>> eigenvector;

    // PRIVATE METHODS
    /**
     *  Store the solutions from a solve
     *  
     *  @param eigenpairs           the eigenpairs from the CI solver
     *  @param multiplier           the Lagrangian multiplier associated with the solution
     */ 
    void parseSolution(const std::vector<Eigenpair>& eigenpairs, double multiplier);

    /**
     *  Throws an error if no solution is available
     *  
     *  @param function_name            name of the function that should throw the error
     */
    void checkAvailableSolutions(const std::string& function_name) const;

    /**
     *  Throws an error if the molecule is not diatomic
     *  
     *  @param function_name            name of the function that should throw the error
     */
    void checkDiatomicMolecule(const std::string& function_name) const;

public:

    // CONSTRUCTORS
    /**
     *  @param molecule                 the molecule that will be solved for
     *  @param basis_set                the basisset that should be used
     *  @param basis_targets            the targeted basis functions for the constraint
     *  @param frozencores              the amount of frozen cores for the FCI calculation
     */
    MullikenConstrainedFCI(const Molecule& molecule, const std::string& basis_set, const std::vector<size_t>& basis_targets, size_t frozencores = 0);


    // PUBLIC METHODS
    /**
     *  Solve the eigenvalue problem for a multiplier with the davidson algorithm
     *  
     *  @param multiplier           a given multiplier    
     *  @param guess                supply a davidson guess         
     */
    void solveMullikenDavidson(const double multiplier, const VectorX<double>& guess);

    /**
     *  Solve the eigenvalue problem for a multiplier with the davidson algorithm, davidson guess will be the previously stored solution
     *  if none is available the Hartree Fock expansion will be used instead
     *  
     *  @param multiplier           a given multiplier          
     */
    void solveMullikenDavidson(const double multiplier);

    /**
     *  Solve the eigenvalue problem for a the next multiplier dense
     * 
     *  @param multiplier           a given multiplier       
     *  @param nos                  the number of eigenpairs or "states" that should be stored for each multiplier
     */
    void solveMullikenDense(const double multiplier, const size_t nos);

    /**
     *  @return a property of the last solve
     */
    double get_energy(size_t index = 0) const { checkAvailableSolutions("get_energy"); return energy[index]; };
    double get_population(size_t index = 0) const { checkAvailableSolutions("get_population"); return population[index]; };
    double get_lambda(size_t index = 0) const { checkAvailableSolutions("get_lambda"); return lambda[index]; };
    double get_entropy(size_t index = 0) const { checkAvailableSolutions("get_entropy"); return entropy[index]; };
    double get_A_fragment_energy(size_t index = 0) const { checkAvailableSolutions("get_A_fragment_energy"); checkDiatomicMolecule("get_A_fragment_energy"); return A_fragment_energy[index]; };
    double get_A_fragment_self_energy(size_t index = 0) const { checkAvailableSolutions("get_A_fragment_self_energy"); checkDiatomicMolecule("get_A_fragment_self_energy"); return A_fragment_self_energy[index]; };
    double get_B_fragment_energy(size_t index = 0) const { checkAvailableSolutions("get_B_fragment_energy"); checkDiatomicMolecule("get_B_fragment_energy"); return B_fragment_energy[index]; };
    double get_B_fragment_self_energy(size_t index = 0) const { checkAvailableSolutions("get_B_fragment_self_energy"); checkDiatomicMolecule("get_B_fragment_self_energy"); return B_fragment_self_energy[index]; };
    double get_interaction_energy(size_t index = 0) const { checkAvailableSolutions("get_interaction_energy"); checkDiatomicMolecule("get_interaction_energy"); return interaction_energy[index]; };

    double get_solve_time() const { checkAvailableSolutions("get_solve_time"); return solve_time; };
    /**
     *  @return all properties
     */
    std::vector<double> all(size_t index = 0) const; 
};


}  // namespace QCMethod
}  // namespace GQCP


#endif  // GQCP_QCMETHOD_MULLIKENCONSTRAONEDFCI_HPP
