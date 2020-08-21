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


#include "Mathematical/Representation/QCRankFourTensor.hpp"
#include "Processing/DensityMatrices/OneDM.hpp"


namespace GQCP {


/**
 *  A type that represents a two-electron density matrix.
 *
 *  @tparam _Scalar     the scalar type
 */
template <typename _Scalar>
class TwoDM: public QCRankFourTensor<_Scalar> {
public:
    using Scalar = _Scalar;

    using BaseRepresentation = QCRankFourTensor<Scalar>;


public:
    /*
     *  CONSTRUCTORS
     */

    using QCRankFourTensor<Scalar>::QCRankFourTensor;  // use base constructors


    /*
     *  PUBLIC METHODS
     */

    /**
     *  @return a partial contraction of the 2-DM, where D(p,q) = d(p,q,r,r)
     */
    OneDM<Scalar> reduce() const {

        // TODO: when Eigen3 releases tensor.trace(), use it to implement the reduction

        auto K = static_cast<size_t>(this->dimension());

        OneDM<double> D = OneDM<double>::Zero(K, K);
        for (size_t p = 0; p < K; p++) {
            for (size_t q = 0; q < K; q++) {
                for (size_t r = 0; r < K; r++) {
                    D(p, q) += this->operator()(p, q, r, r);
                }
            }
        }

        return D;
    }


    /**
     *  @return the trace of the 2-DM, i.e. d(p,p,q,q)
     */
    Scalar trace() const {
        // TODO: when Eigen3 releases tensor.trace(), use it to implement the reduction

        auto K = static_cast<size_t>(this->dimension());

        Scalar trace {};
        for (size_t p = 0; p < K; p++) {
            for (size_t q = 0; q < K; q++) {
                trace += this->operator()(p, p, q, q);
            }
        }

        return trace;
    }
};


}  // namespace GQCP
