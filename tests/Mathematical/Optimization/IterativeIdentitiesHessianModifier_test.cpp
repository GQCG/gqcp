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
#define BOOST_TEST_MODULE "IterativeIdentitiesHessianModifier_test"

#include <boost/test/unit_test.hpp>

#include "Mathematical/Optimization/IterativeIdentitiesHessianModifier.hpp"

#include "Eigen/Dense"

BOOST_AUTO_TEST_CASE ( becomes_positive_definite ) {

    GQCP::SquareMatrix<double> A (2);  // is an indefinite matrix: eigenvalues are approx. -1.7 and 4.7
    A << -1.0, -2.0,
         -2.0,  4.0;


    GQCP::IterativeIdentitiesHessianModifier hessian_modifier {};  // default values for the parameters
    auto modified_hessian = hessian_modifier(A);

    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver (modified_hessian);
    BOOST_CHECK(solver.eigenvalues().minCoeff() > 0);  // all eigenvalues should be positive!
}
