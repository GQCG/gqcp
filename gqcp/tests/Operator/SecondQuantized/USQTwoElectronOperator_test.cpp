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
#define BOOST_TEST_MODULE "SQTwoElectronOperator"

#include <boost/test/unit_test.hpp>

#include "Operator/SecondQuantized/USQTwoElectronOperator.hpp"

#include "Utilities/linalg.hpp"
#include "Utilities/miscellaneous.hpp"


/**
 *  Check the interface for constructing SQTwoElectronOperators from Tensors
 */
BOOST_AUTO_TEST_CASE ( USQTwoElectronOperator_constructor ) {

    // Check a correct constructor
    const GQCP::QCRankFourTensor<double> tensor (3);
    GQCP::ScalarUSQTwoElectronOperator<double> O (tensor, tensor);


    // Check a faulty constructor
    GQCP::Tensor<double, 4> tensor2 (3, 3, 3, 2);
    BOOST_CHECK_THROW(GQCP::ScalarUSQTwoElectronOperator<double> O2 (tensor2, tensor2), std::invalid_argument);
    BOOST_CHECK_THROW(GQCP::ScalarUSQTwoElectronOperator<double> O2 (tensor, tensor2), std::invalid_argument);
    BOOST_CHECK_THROW(GQCP::ScalarUSQTwoElectronOperator<double> O2 (tensor2, tensor), std::invalid_argument);

}


/**
 *  Check if the zero constructor really sets is parameters to all zeros
 */
BOOST_AUTO_TEST_CASE ( SQTwoElectronOperator_zero_constructor ) {

    const size_t K = 2;
    auto K_ = static_cast<double>(K);
    GQCP::ScalarUSQTwoElectronOperator<double> op {K}; // should initialize zero's

    // Create a reference zero tensor
    GQCP::QCRankFourTensor<double> ref (K);

    for (size_t i = 0; i < K; i++) {
        for (size_t j = 0; j < K; j++) {
            for (size_t k = 0; k < K; k++) {
                for (size_t l = 0; l < K; l++) {
                    ref(i,j,k,l) = 0;
                }
            }
        }
    }

    BOOST_CHECK_EQUAL(op.alphaDimension(), K);
    BOOST_CHECK_EQUAL(op.betaDimension(), K);
    BOOST_CHECK(op.alphaParameters().isApprox(ref, 1.0e-08));
    BOOST_CHECK(op.betaParameters().isApprox(ref, 1.0e-08));

}


/**
 *  Check if the formulas in effectiveOneElectronPartition are implemented correctly
 */
BOOST_AUTO_TEST_CASE ( USQTwoElectronOperator_effectiveOneElectronPartition ) {

    const size_t K = 4;
    auto K_ = static_cast<double>(K);

    // Set up toy 2-electron integrals
    GQCP::QCRankFourTensor<double> g_par_alpha (K);
    GQCP::QCRankFourTensor<double> g_par_beta (K);

    g_par_alpha.setZero();
    g_par_beta.setZero();

    for (size_t i = 0; i < K; i++) {
        for (size_t j = 0; j < K; j++) {
            for (size_t k = 0; k < K; k++) {
                for (size_t l = 0; l < K; l++) {
                    g_par_alpha(i,j,k,l) = (i+1) + 2*(j+1) + 4*(k+1) + 8*(l+1);
                    g_par_beta(i,j,k,l) = (i+1) + 2*(j+1) + 4*(k+1) + 8*(l+1);
                }
            }
        }
    }

    GQCP::ScalarUSQTwoElectronOperator<double> g (g_par_alpha, g_par_beta);


// Set up the reference effective one-electron integrals by manual calculation
    GQCP::QCMatrix<double> k_par_ref = GQCP::QCMatrix<double>::Zero(K, K);  // reference parameters
    for (size_t p = 0; p < K; p++) {
        for (size_t q = 0; q < K; q++) {
            auto p_ = static_cast<double>(p) + 1;
            auto q_ = static_cast<double>(q) + 1;

            k_par_ref(p,q) = -K_ / 2 * (p_ + 8*q_ + 3*K_ + 3);
        }
    }


    BOOST_CHECK(k_par_ref.isApprox(g.effectiveOneElectronPartition().alphaParameters(), 1.0e-08));
    BOOST_CHECK(k_par_ref.isApprox(g.effectiveOneElectronPartition().betaParameters(), 1.0e-08));
}


/**
 *  Check if calculateExpectationValue throws when necessary
 */
BOOST_AUTO_TEST_CASE ( calculateExpectationValue_throw ) {

    const GQCP::ScalarUSQTwoElectronOperator<double> g {2};

    const GQCP::TwoRDM<double> D_valid (2);
    const GQCP::TwoRDM<double> D_invalid (3);

    BOOST_CHECK_THROW(g.calculateExpectationValue(D_invalid, D_invalid), std::invalid_argument);
    BOOST_CHECK_THROW(g.calculateExpectationValue(D_invalid, D_valid), std::invalid_argument);
    BOOST_CHECK_THROW(g.calculateExpectationValue(D_valid, D_invalid), std::invalid_argument);

    BOOST_CHECK_NO_THROW(g.calculateExpectationValue(D_valid, D_valid));
}


/**
 * Check whether or not calculateExpectationValue shows the correct behaviour
 */
BOOST_AUTO_TEST_CASE ( calculateExpectationValue_behaviour ) {
    
    const size_t dim = 2;

   // Initialize a test tensor and convert it into an operators
    GQCP::QCRankFourTensor<double> T1 (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T1(i,j,k,l) = (i+1) + 2*(j+1) + 4*(k+1) + 8*(l+1);
                }
            }
        }
    }
    const GQCP::ScalarUSQTwoElectronOperator<double> op (T1, T1);

    // initialize an alpha and beta density matrix, each one is chosen as a hermitian matrix.
    GQCP::TwoRDM<double> d_alpha (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    d_alpha(i,j,k,l) = 1;
                }
            }
        }
    }

    GQCP::TwoRDM<double> d_beta (dim);
    
    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    d_beta(i,j,k,l) = 2;
                }
            }
        }
    }

    // Initialize a reference value
    GQCP::QCMatrix<double> ref (1);
    ref << 540.0;

    const auto ex_value = op.calculateExpectationValue(d_alpha, d_beta);
    BOOST_CHECK(ex_value.isApprox(ref, 1.0e-08));
}


/**
 *  Check if addition of operators works as expected
 */
BOOST_AUTO_TEST_CASE ( USQTwoElectronOperator_addition ) {

    const size_t dim = 2;

    // Initialize two test tensors and convert them into operators
    GQCP::QCRankFourTensor<double> T1 (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T1(i,j,k,l) = (i+1) + 2*(j+1) + 4*(k+1) + 8*(l+1);
                }
            }
        }
    }
    const GQCP::ScalarUSQTwoElectronOperator<double> op1 (T1, T1);

    GQCP::QCRankFourTensor<double> T2 (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T2(i,j,k,l) = (i+1) + 2*(j+1) + 4*(k+1) + 8*(l+1);
                }
            }
        }
    }   
    const GQCP::ScalarUSQTwoElectronOperator<double> op2 (T2, T2);


    // Initialize the reference and check the result
    GQCP::QCRankFourTensor<double> T_sum_ref (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T_sum_ref(i,j,k,l) = 2*(i+1) + 4*(j+1) + 8*(k+1) + 16*(l+1);
                }
            }
        }
    }   
    
    auto op_sum = op1 + op2;
    BOOST_CHECK(op_sum.alphaParameters().isApprox(T_sum_ref, 1.0e-08));
    BOOST_CHECK(op_sum.betaParameters().isApprox(T_sum_ref, 1.0e-08));
}


/**
 *  Check if the scalar product with an operator works as expected
 */
BOOST_AUTO_TEST_CASE ( USQTwoElectronOperator_scalar_product ) {

    const size_t dim = 2;
    const double scalar = 2.0;

    // Initialize two test tensors and convert them into operators
    GQCP::QCRankFourTensor<double> T1 (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T1(i,j,k,l) = (i+1) + 2*(j+1) + 4*(k+1) + 8*(l+1);
                }
            }
        }
    }
    const GQCP::ScalarUSQTwoElectronOperator<double> op1 (T1, T1);

    // Initialize the reference and check the result
    GQCP::QCRankFourTensor<double> T_prod_ref (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T_prod_ref(i,j,k,l) = 2*(i+1) + 4*(j+1) + 8*(k+1) + 16*(l+1);
                }
            }
        }
    }   
    
    auto op_prod = scalar * op1;
    BOOST_CHECK(op_prod.alphaParameters().isApprox(T_prod_ref, 1.0e-08));
    BOOST_CHECK(op_prod.betaParameters().isApprox(T_prod_ref, 1.0e-08));
}


/**
 *  Check if negating an operator works as expected
 */
BOOST_AUTO_TEST_CASE ( USQTwoElectronOperator_negate ) {

    const size_t dim = 2;
    const double scalar = 2.0;

    // Initialize two test tensors and convert them into operators
    GQCP::QCRankFourTensor<double> T1 (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T1(i,j,k,l) = 5;
                }
            }
        }
    }
    const GQCP::ScalarUSQTwoElectronOperator<double> op1 (T1, T1);

    // Initialize the reference and check the result
    GQCP::QCRankFourTensor<double> T_neg_ref (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T_neg_ref(i,j,k,l) = -5;
                }
            }
        }
    }   
    
    auto op_neg = -op1;
    BOOST_CHECK(op_neg.alphaParameters().isApprox(T_neg_ref, 1.0e-08));
    BOOST_CHECK(op_neg.betaParameters().isApprox(T_neg_ref, 1.0e-08));
}


BOOST_AUTO_TEST_CASE ( USQTwoElectronOperator_difference ) {

    const size_t dim = 2;

    // Initialize two test tensors and convert them into operators
    GQCP::QCRankFourTensor<double> T1 (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T1(i,j,k,l) = (i+1) + 2*(j+1) + 4*(k+1) + 8*(l+1);
                }
            }
        }
    }
    const GQCP::ScalarUSQTwoElectronOperator<double> op1 (T1, T1);

    GQCP::QCRankFourTensor<double> T2 (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T2(i,j,k,l) = 2*(i+1) + 4*(j+1) + 8*(k+1) + 16*(l+1);
                }
            }
        }
    }   
    const GQCP::ScalarUSQTwoElectronOperator<double> op2 (T2, T2);

    auto op_diff = op2 - op1;
    BOOST_CHECK(op_diff.alphaParameters().isApprox(T1, 1.0e-08));
    BOOST_CHECK(op_diff.betaParameters().isApprox(T1, 1.0e-08));
}


/**
 * Check whether or not the rotate with transformation matrix method works as expected
 */
BOOST_AUTO_TEST_CASE ( rotate_with_unitary_transformation_matrix ) {

    const size_t dim = 2;

    // Initialize a test tensor and convert it into an operator
    GQCP::QCRankFourTensor<double> T1 (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T1(i,j,k,l) = (i+1) + 2*(j+1) + 4*(k+1) + 8*(l+1);
                }
            }
        }
    }
    GQCP::ScalarUSQTwoElectronOperator<double> op (T1, T1);

    // Initialize a unitary transformation matrix
    GQCP::TransformationMatrix<double> U (dim);
    U << 1.0, 0.0,
         0.0, 1.0;
    
    op.rotate(U);
    BOOST_CHECK(op.alphaParameters().isApprox(T1, 1.0e-08));
    BOOST_CHECK(op.betaParameters().isApprox(T1, 1.0e-08));

}


/**
 * Check whether or not the rotate with transformation matrix method works as expected
 */
BOOST_AUTO_TEST_CASE ( transform_with_transformation_matrix ) {

    const size_t dim = 2;

    // Initialize a test tensor and convert it into an operator
    GQCP::QCRankFourTensor<double> T1 (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T1(i,j,k,l) = 1;
                }
            }
        }
    }
    GQCP::ScalarUSQTwoElectronOperator<double> op (T1, T1);

    // Initialize a transformation matrix
    GQCP::TransformationMatrix<double> T (dim);
    T << 2.0, 3.0,
         3.0, 4.0;

    // Initialize a reference tensor
    GQCP::QCRankFourTensor<double> ref (dim);
    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    if ((i + j + k + l) == 0) {
                        ref(i,j,k,l) = 625.0;
                    }
                    if ((i + j + k + l) == 1) {
                        ref(i,j,k,l) = 875.0;
                    }
                    if ((i + j + k + l) == 2) {
                        ref(i,j,k,l) = 1225.0;
                    }
                    if ((i + j + k + l) == 3) {
                        ref(i,j,k,l) = 1715.0;
                    }
                    if ((i + j + k + l) == 4) {
                        ref(i,j,k,l) = 2401.0;
                    }
                }
            }
        }
    }

    op.transform(T);
    BOOST_CHECK(op.alphaParameters().isApprox(ref, 1.0e-08));
    BOOST_CHECK(op.betaParameters().isApprox(ref, 1.0e-08));
}


/**
 * Check whether or not the jacobi rotation method works as expected
 */
BOOST_AUTO_TEST_CASE ( transform_with_jacobi_matrix ) {

    const size_t dim = 2;

    // Initialize a test tensor and convert it into an operator
    GQCP::QCRankFourTensor<double> T1 (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    T1(i,j,k,l) = 1;
                }
            }
        }
    }
    GQCP::ScalarUSQTwoElectronOperator<double> op (T1, T1);

    // Initialize a transformation matrix
    GQCP::JacobiRotationParameters J (1, 0, M_PI_2);

    // Initialize a reference tensor
    GQCP::QCRankFourTensor<double> ref (dim);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            for (size_t k = 0; k < dim; k++) {
                for (size_t l = 0; l < dim; l++) {
                    if ((i + j + k + l)%2 == 0) {
                        ref(i,j,k,l) = 1.0;
                    }
                    if ((i + j + k + l)%2 != 0) {
                        ref(i,j,k,l) = -1.0;
                    }
                }
            }
        }
    }

    op.rotate(J);
    BOOST_CHECK(op.alphaParameters().isApprox(ref, 1.0e-08));
    BOOST_CHECK(op.betaParameters().isApprox(ref, 1.0e-08));
}
