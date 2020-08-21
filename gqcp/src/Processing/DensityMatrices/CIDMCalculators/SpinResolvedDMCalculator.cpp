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

#include "Processing/DensityMatrices/CIDMCalculators/SpinResolvedDMCalculator.hpp"


namespace GQCP {


/*
 *  CONSTRUCTORS
 */

/**
 *  @param onv_basis                the ONV basis that is associated to this DMCalculator
 */
SpinResolvedDMCalculator::SpinResolvedDMCalculator(const SpinResolvedONVBasis& onv_basis) :
    onv_basis {onv_basis} {}


/*
 *  PUBLIC OVERRIDDEN METHODS
 */

/**
 *  @param x        the coefficient vector representing the FCI wave function
 *
 *  @return all 1-DMs given a coefficient vector
 */
SpinResolvedOneDM<double> SpinResolvedDMCalculator::calculate1DMs(const VectorX<double>& x) const {

    // Initialize as zero matrices
    size_t K = this->onv_basis.numberOfOrbitals();

    OneDM<double> D_aa = OneDM<double>::Zero(K, K);
    OneDM<double> D_bb = OneDM<double>::Zero(K, K);

    SpinUnresolvedONVBasis onv_basis_alpha = onv_basis.onvBasisAlpha();
    SpinUnresolvedONVBasis onv_basis_beta = onv_basis.onvBasisBeta();

    auto dim_alpha = onv_basis_alpha.dimension();
    auto dim_beta = onv_basis_beta.dimension();

    // ALPHA
    SpinUnresolvedONV spin_string_alpha = onv_basis_alpha.constructONVFromAddress(0);  // alpha spin string with address 0
    for (size_t I_alpha = 0; I_alpha < dim_alpha; I_alpha++) {                         // I_alpha loops over all the addresses of the alpha spin strings
        for (size_t p = 0; p < K; p++) {                                               // p loops over SOs
            int sign_p = 1;
            if (spin_string_alpha.annihilate(p, sign_p)) {  // if p is in I_alpha
                double diagonal_contribution = 0;

                // Diagonal contributions for the 1-DM, i.e. D_pp
                // We are storing the alpha addresses as 'major', i.e. the total address I_alpha I_beta = I_alpha * dim_beta + I_beta
                for (size_t I_beta = 0; I_beta < dim_beta; I_beta++) {
                    double c_I_alpha_I_beta = x(I_alpha * dim_beta + I_beta);
                    diagonal_contribution += std::pow(c_I_alpha_I_beta, 2);
                }
                D_aa(p, p) += diagonal_contribution;

                // Off-diagonal contributions for the 1-DM, i.e. D_pq (p!=q)
                for (size_t q = 0; q < p; q++) {  // q < p loops over SOs
                    int sign_pq = sign_p;
                    if (spin_string_alpha.create(q, sign_pq)) {                         // if q is not occupied in I_alpha
                        size_t J_alpha = onv_basis_alpha.addressOf(spin_string_alpha);  // find all strings J_alpha that couple to I_alpha

                        double off_diagonal_contribution = 0;
                        for (size_t I_beta = 0; I_beta < dim_beta; I_beta++) {
                            double c_I_alpha_I_beta = x(I_alpha * dim_beta + I_beta);  // alpha addresses are 'major'
                            double c_J_alpha_I_beta = x(J_alpha * dim_beta + I_beta);
                            off_diagonal_contribution += c_I_alpha_I_beta * c_J_alpha_I_beta;
                        }
                        D_aa(p, q) += sign_pq * off_diagonal_contribution;
                        D_aa(q, p) += sign_pq * off_diagonal_contribution;  // add the symmetric contribution because we are looping over q < p

                        spin_string_alpha.annihilate(q);  // undo the previous creation

                    }  // create on q
                }      // q loop

                spin_string_alpha.create(p);  // undo the previous annihilation

            }  // annihilate on p
        }      // p loop

        if (I_alpha < dim_alpha - 1) {  // prevent the last permutation from occurring
            onv_basis_alpha.transformONVToNextPermutation(spin_string_alpha);
        }

    }  // I_alpha loop


    // BETA
    SpinUnresolvedONV spin_string_beta = onv_basis_beta.constructONVFromAddress(0);  // spin string with address 0
    for (size_t I_beta = 0; I_beta < dim_beta; I_beta++) {                           // I_beta loops over all the addresses of the spin strings
        for (size_t p = 0; p < K; p++) {                                             // p loops over SOs
            int sign_p = 1;
            if (spin_string_beta.annihilate(p, sign_p)) {  // if p is in I_beta
                double diagonal_contribution = 0;

                // Diagonal contributions for the 1-DM, i.e. D_pp
                // We are storing the alpha addresses as 'major', i.e. the total address I_alpha I_beta = I_alpha * dim_beta + I_beta
                for (size_t I_alpha = 0; I_alpha < dim_alpha; I_alpha++) {
                    double c_I_alpha_I_beta = x(I_alpha * dim_beta + I_beta);
                    diagonal_contribution += std::pow(c_I_alpha_I_beta, 2);
                }

                D_bb(p, p) += diagonal_contribution;

                // Off-diagonal contributions for the 1-DM
                for (size_t q = 0; q < p; q++) {  // q < p loops over SOs
                    int sign_pq = sign_p;
                    if (spin_string_beta.create(q, sign_pq)) {                       // if q is not in I_beta
                        size_t J_beta = onv_basis_beta.addressOf(spin_string_beta);  // find all strings J_beta that couple to I_beta

                        double off_diagonal_contribution = 0;
                        for (size_t I_alpha = 0; I_alpha < dim_alpha; I_alpha++) {
                            double c_I_alpha_I_beta = x(I_alpha * dim_beta + I_beta);  // alpha addresses are 'major'
                            double c_I_alpha_J_beta = x(I_alpha * dim_beta + J_beta);
                            off_diagonal_contribution += c_I_alpha_I_beta * c_I_alpha_J_beta;
                        }
                        D_bb(p, q) += sign_pq * off_diagonal_contribution;
                        D_bb(q, p) += sign_pq * off_diagonal_contribution;  // add the symmetric contribution because we are looping over q < p

                        spin_string_beta.annihilate(q);  // undo the previous creation

                    }  // create on q
                }      // loop over q

                spin_string_beta.create(p);  // undo the previous annihilation

            }  // annihilate on p
        }      // loop over p

        if (I_beta < dim_beta - 1) {  // prevent the last permutation from occurring
            onv_basis_beta.transformONVToNextPermutation(spin_string_beta);
        }

    }  // I_beta loop
    return SpinResolvedOneDM<double>(D_aa, D_bb);
}


/**
 *  @param x        the coefficient vector representing the FCI wave function
 *
 *  @return all 2-DMs given a coefficient vector
 */
SpinResolvedTwoDM<double> SpinResolvedDMCalculator::calculate2DMs(const VectorX<double>& x) const {


    // KISS implementation of the 2-DMs (no symmetry relations are used yet)

    SpinUnresolvedONVBasis onv_basis_alpha = onv_basis.onvBasisAlpha();
    SpinUnresolvedONVBasis onv_basis_beta = onv_basis.onvBasisBeta();

    auto dim_alpha = onv_basis_alpha.dimension();
    auto dim_beta = onv_basis_beta.dimension();

    // Initialize as zero matrices
    size_t K = this->onv_basis.numberOfOrbitals();

    TwoDM<double> d_aaaa {K};
    d_aaaa.setZero();

    TwoDM<double> d_aabb {K};
    d_aabb.setZero();

    TwoDM<double> d_bbbb {K};
    d_bbbb.setZero();


    // ALPHA-ALPHA-ALPHA-ALPHA
    SpinUnresolvedONV spin_string_alpha_aaaa = onv_basis_alpha.constructONVFromAddress(0);  // spin string with address 0
    for (size_t I_alpha = 0; I_alpha < dim_alpha; I_alpha++) {                              // I_alpha loops over all the addresses of the alpha spin strings

        for (size_t p = 0; p < K; p++) {  // p loops over SOs
            int sign_p = 1;               // sign of the operator a_p

            if (spin_string_alpha_aaaa.annihilate(p, sign_p)) {  // if p is not in I_alpha

                for (size_t r = 0; r < K; r++) {  // r loops over SOs
                    int sign_pr = sign_p;         // sign of the operator a_r a_p

                    if (spin_string_alpha_aaaa.annihilate(r, sign_pr)) {  // if r is not in I_alpha

                        for (size_t s = 0; s < K; s++) {  // s loops over SOs
                            int sign_prs = sign_pr;       // sign of the operator a^dagger_s a_r a_p

                            if (spin_string_alpha_aaaa.create(s, sign_prs)) {  // if s is in I_alpha

                                for (size_t q = 0; q < K; q++) {  // q loops over SOs
                                    int sign_prsq = sign_prs;     // sign of the operator a^dagger_q a^dagger_s a_r a_p

                                    if (spin_string_alpha_aaaa.create(q, sign_prsq)) {                       // if q is not in I_alpha
                                        size_t J_alpha = onv_basis_alpha.addressOf(spin_string_alpha_aaaa);  // address of the coupling string

                                        double contribution = 0.0;
                                        for (size_t I_beta = 0; I_beta < dim_beta; I_beta++) {
                                            double c_I_alpha_I_beta = x(I_alpha * dim_beta + I_beta);  // alpha addresses are 'major'
                                            double c_J_alpha_I_beta = x(J_alpha * dim_beta + I_beta);
                                            contribution += c_I_alpha_I_beta * c_J_alpha_I_beta;
                                        }


                                        d_aaaa(p, q, r, s) += sign_prsq * contribution;

                                        spin_string_alpha_aaaa.annihilate(q);  // undo the previous creation
                                    }
                                }  // loop over q

                                spin_string_alpha_aaaa.annihilate(s);  // undo the previous creation
                            }
                        }  // loop over s

                        spin_string_alpha_aaaa.create(r);  // undo the previous annihilation
                    }
                }  // loop over r

                spin_string_alpha_aaaa.create(p);  // undo the previous annihilation
            }
        }  // loop over p

        if (I_alpha < dim_alpha - 1) {  // prevent the last permutation from occurring
            onv_basis_alpha.transformONVToNextPermutation(spin_string_alpha_aaaa);
        }

    }  // loop over I_alpha


    // ALPHA-ALPHA-BETA-BETA
    SpinUnresolvedONV spin_string_alpha_aabb = onv_basis_alpha.constructONVFromAddress(0);  // spin string with address 0
    for (size_t I_alpha = 0; I_alpha < dim_alpha; I_alpha++) {                              // I_alpha loops over all the addresses of the alpha spin strings

        for (size_t p = 0; p < K; p++) {  // p loops over SOs
            int sign_p = 1;               // sign of the operator a_p_alpha

            if (spin_string_alpha_aabb.annihilate(p, sign_p)) {  // if p is in I_alpha

                for (size_t q = 0; q < K; q++) {  // q loops over SOs
                    int sign_pq = sign_p;         // sign of the operator a^dagger_p_alpha a_p_alpha

                    if (spin_string_alpha_aabb.create(q, sign_pq)) {                         // if q is not in I_alpha
                        size_t J_alpha = onv_basis_alpha.addressOf(spin_string_alpha_aabb);  // the string that couples to I_alpha


                        SpinUnresolvedONV spin_string_beta_aabb = onv_basis_beta.constructONVFromAddress(0);  // spin string with address 0
                        for (size_t I_beta = 0; I_beta < dim_beta; I_beta++) {                                // I_beta loops over all addresses of beta spin strings

                            for (size_t r = 0; r < K; r++) {  // r loops over all SOs
                                int sign_r = 1;               // sign of the operator a_r_beta

                                if (spin_string_beta_aabb.annihilate(r, sign_r)) {

                                    for (size_t s = 0; s < K; s++) {  // s loops over all SOs
                                        int sign_rs = sign_r;         // sign of the operator a^dagger_s_beta a_r_beta

                                        if (spin_string_beta_aabb.create(s, sign_rs)) {
                                            size_t J_beta = onv_basis_beta.addressOf(spin_string_beta_aabb);  // the string that couples to I_beta

                                            double c_I_alpha_I_beta = x(I_alpha * dim_beta + I_beta);  // alpha addresses are 'major'
                                            double c_J_alpha_J_beta = x(J_alpha * dim_beta + J_beta);
                                            d_aabb(p, q, r, s) += sign_pq * sign_rs * c_I_alpha_I_beta * c_J_alpha_J_beta;

                                            spin_string_beta_aabb.annihilate(s);  // undo the previous creation
                                        }
                                    }  // loop over s


                                    spin_string_beta_aabb.create(r);  // undo the previous annihilation
                                }

                            }  // loop over r

                            if (I_beta < dim_beta - 1) {  // prevent the last permutation from occurring
                                onv_basis_beta.transformONVToNextPermutation(spin_string_beta_aabb);
                            }

                        }  // loop over beta addresses

                        spin_string_alpha_aabb.annihilate(q);  // undo the previous creation
                    }
                }  // loop over q

                spin_string_alpha_aabb.create(p);  // undo the previous annihilation
            }
        }  // loop over p

        if (I_alpha < dim_alpha - 1) {  // prevent the last permutation from occurring
            onv_basis_alpha.transformONVToNextPermutation(spin_string_alpha_aabb);
        }

    }  // loop over alpha addresses


    // BETA-BETA-ALPHA-ALPHA
    // We know that d^aabb_pqrs = d^bbaa_rspq
    Eigen::array<int, 4> shuffle {2, 3, 0, 1};  // array specifying the axes that should be swapped
    TwoDM<double> d_bbaa(d_aabb.Eigen().shuffle(shuffle));


    // BETA-BETA-BETA-BETA
    SpinUnresolvedONV spin_string_beta_bbbb = onv_basis_beta.constructONVFromAddress(0);  // spin string with address 0
    for (size_t I_beta = 0; I_beta < dim_beta; I_beta++) {                                // I_beta loops over all the addresses of the beta spin strings

        for (size_t p = 0; p < K; p++) {  // p loops over SOs
            int sign_p = 1;               // sign of the operator a_p

            if (spin_string_beta_bbbb.annihilate(p, sign_p)) {  // if p is not in I_beta

                for (size_t r = 0; r < K; r++) {  // r loops over SOs
                    int sign_pr = sign_p;         // sign of the operator a_r a_p

                    if (spin_string_beta_bbbb.annihilate(r, sign_pr)) {  // if r is not in I_beta

                        for (size_t s = 0; s < K; s++) {  // s loops over SOs
                            int sign_prs = sign_pr;       // sign of the operator a^dagger_s a_r a_p

                            if (spin_string_beta_bbbb.create(s, sign_prs)) {  // if s is in I_beta

                                for (size_t q = 0; q < K; q++) {  // q loops over SOs
                                    int sign_prsq = sign_prs;     // sign of the operator a^dagger_q a^dagger_s a_r a_p

                                    if (spin_string_beta_bbbb.create(q, sign_prsq)) {                     // if q is not in I_beta
                                        size_t J_beta = onv_basis_beta.addressOf(spin_string_beta_bbbb);  // address of the coupling string

                                        double contribution = 0.0;
                                        for (size_t I_alpha = 0; I_alpha < dim_alpha; I_alpha++) {
                                            double c_I_alpha_I_beta = x(I_alpha * dim_beta + I_beta);  // alpha addresses are 'major'
                                            double c_I_alpha_J_beta = x(I_alpha * dim_beta + J_beta);
                                            contribution += c_I_alpha_I_beta * c_I_alpha_J_beta;
                                        }


                                        d_bbbb(p, q, r, s) += sign_prsq * contribution;

                                        spin_string_beta_bbbb.annihilate(q);  // undo the previous creation
                                    }
                                }  // loop over q

                                spin_string_beta_bbbb.annihilate(s);  // undo the previous creation
                            }
                        }  // loop over s

                        spin_string_beta_bbbb.create(r);  // undo the previous annihilation
                    }
                }  // loop over r

                spin_string_beta_bbbb.create(p);  // undo the previous annihilation
            }
        }  // loop over p

        if (I_beta < dim_beta - 1) {  // prevent the last permutation from occurring
            onv_basis_beta.transformONVToNextPermutation(spin_string_beta_bbbb);
        }

    }  // loop over I_beta

    return SpinResolvedTwoDM<double>(d_aaaa, d_aabb, d_bbaa, d_bbbb);
}


}  // namespace GQCP
