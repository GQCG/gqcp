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


#include "Mathematical/Representation/Matrix.hpp"
#include "Utilities/miscellaneous.hpp"

#include <boost/numeric/conversion/converter.hpp>

#include <numeric>


namespace GQCP {


/**
 *  A dynamic square extension of the Matrix class
 *
 *  @tparam _Scalar      the scalar type
 */
template <typename _Scalar>
class SquareMatrix: public MatrixX<_Scalar> {
public:
    using Scalar = _Scalar;

    using Base = MatrixX<Scalar>;
    using Self = SquareMatrix<Scalar>;


public:
    /*
     *  CONSTRUCTORS
     */

    /**
     *  A default constructor.
     */
    SquareMatrix() :
        Base() {}


    /**
     *  Construct a square matrix given a dimension
     *
     *  @param dim      the dimension of the matrix
     */
    SquareMatrix(const size_t dim) :
        Base(dim, dim) {}


    /**
     *  A basic constructor from a GQCP::MatrixX that checks if the given matrix is square
     *
     *  @param matrix       the matrix that should be square
     */
    SquareMatrix(const Base& matrix) :
        Base(matrix) {

        // Check if the given matrix is square
        if (this->cols() != this->rows()) {
            throw std::invalid_argument("SquareMatrix::SquareMatrix(Base): The given matrix is not square.");
        }
    }


    /**
     *  Constructor from Eigen::Matrix expressions
     *
     *  @tparam ExpDerived      the type of the Eigen::Matrix expression (normally generated by the compiler)
     *
     *  @param exp              the Eigen::Matrix expression
     */
    template <typename ExpDerived>
    SquareMatrix(const Eigen::MatrixBase<ExpDerived>& exp) :
        Self(Base(exp))  // the Base constructor returns the required type for the square-checking constructor
    {}


    /*
     *  NAMED CONSTRUCTORS
     */

    /**
     *  @param a        the strict (not including the diagonal) lower/upper triangle of the matrix in column major form
     * 
     *  @return a square matrix in which the lower triangle is filled in with its given vector representation; all other elements are set to zero
     */
    static Self FromStrictTriangle(const VectorX<Scalar>& v) {

        size_t dim = strictTriangularRootOf(v.size());
        Self A = Self::Zero(dim, dim);

        size_t column_index = 0;
        size_t row_index = column_index + 1;  // fill the lower triangle
        for (size_t vector_index = 0; vector_index < v.size(); vector_index++) {
            A(row_index, column_index) = v(vector_index);

            if (row_index == dim - 1) {  // -1 because of computers
                column_index++;
                row_index = column_index + 1;
            } else {
                row_index++;
            }
        }
        return A;
    }


    /**
     *  @param indices          the indices of the diagonal which will be set to 1
     *  @param M                the dimension of the resulting matrix
     *
     * @return a 'partition' matrix, which is a diagonal matrix, whose diagonal elements are either 0 or 1, as specified by the given indices
     */
    static Self PartitionMatrix(const std::vector<size_t>& indices, const size_t M) {

        Self A = Self::Zero(M, M);

        for (const auto& index : indices) {
            if (index >= M) {
                throw std::invalid_argument("SquareMatrix::PartitionMatrix(std::vector<size_t>, size_t): index is larger than matrix dimension");
            }

            A(index, index) = 1;
        }

        return A;
    }


    /**
     *  @param start            starting index of the partition
     *  @param range            range overwhich the partition indices stretch
     *  @param M                the dimension of the resulting matrix
     *
     * @return a 'partition' matrix, which is a diagonal matrix, whose diagonal elements are either 0 or 1, as specified by the given indices
     */
    static Self PartitionMatrix(const size_t start, const size_t range, const size_t M) {

        std::vector<size_t> l(range);
        std::iota(std::begin(l), std::end(l), start);

        return PartitionMatrix(l, M);
    }


    /**
    *  @param M                the dimension of the resulting matrix
    *
    *  @return a random unitary matrix
    */
    static Self RandomUnitary(const size_t M) {

        // Get a random unitary matrix by diagonalizing a random symmetric matrix
        Self A_random = Self::Random(M, M);
        Self A_symmetric = A_random + A_random.adjoint();
        Eigen::SelfAdjointEigenSolver<Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>> unitary_solver {A_symmetric};

        return unitary_solver.eigenvectors();
    }


    /**
     *  @param v    the upper triangle (including the diagonal) of a matrix
     *
     *  @return the full, symmetric matrix corresponding to the given upper triangle
     */
    static Self SymmetricFromUpperTriangle(const VectorX<Scalar>& v) {

        size_t dim = triangularRootOf(v.size());

        Self A = Self::Zero(dim, dim);

        size_t k = 0;                           // vector index
        for (size_t i = 0; i < dim; i++) {      // row index
            for (size_t j = i; j < dim; j++) {  // column index
                if (i != j) {
                    A(i, j) = v(k);
                    A(j, i) = v(k);
                } else {
                    A(i, i) = v(k);
                }

                k++;
            }
        }

        return A;
    }


    /*
     *  PUBLIC METHODS
     */

    /**
     *  @return the dimension of this square matrix, i.e. the number of rows or columns
     */
    size_t dimension() const { return this->cols(); }  // equals this->rows()

    /**
     *  @return a non-pivoted LU decomposition in an array, with L at position 0 and U on position 1 of the array.
     *  
     *  @note Pivoting is required to ensure that the decomposition is stable. Eigen3 provides partial and full pivot modules, so when the pivot or permutation of a matrix is not of interest we strongly recommend using Eigen3 instead
     */
    std::array<Self, 2> noPivotLUDecompose() const {

        const auto M = this->dimension();

        Self L = Self::Zero(M, M);
        Self U = Self::Zero(M, M);

        // Algorithm from "https://www.geeksforgeeks.org/doolittle-algorithm-lu-decomposition/"
        for (size_t i = 0; i < M; i++) {

            // Systematically solve for the entries of the upper triangular matrix U:
            //  U_ik = A_ik - (LU)_ik
            for (size_t k = i; k < M; k++) {

                // Summation of L(i, j) * U(j, k)
                double sum = 0.0;
                for (size_t j = 0; j < i; j++) {
                    sum += (L(i, j) * U(j, k));
                }

                // Evaluating U(i, k)
                U(i, k) = this->operator()(i, k) - sum;
            }

            // Systematically solve for the entries of the lower triangular matrix L:
            //  L_ik = (A_ik - (LU)_ik) / U_kk
            for (size_t k = i; k < M; k++) {
                if (i == k) {
                    L(i, i) = 1;  // diagonal as 1
                } else {

                    // Summation of L(k, j) * U(j, i)
                    double sum = 0.0;
                    for (size_t j = 0; j < i; j++) {
                        sum += (L(k, j) * U(j, i));
                    }

                    // Evaluating L(k, i)
                    L(k, i) = (this->operator()(k, i) - sum) / U(i, i);
                }
            }
        }

        // Test if the decomposition was stable
        Self A = L * U;
        if (A.isApprox(*this)) {
            return std::array<Self, 2>({L, U});
        } else {
            throw std::runtime_error("SquareMatrix<Scalar>::noPivotLUDecompose(): The decomposition was not stable");
        }
    }


    /**
     *  @return a pair-wise strict reduced form of this square matrix. The elements of the matrix are put into the vector such that
     *      v(m) = M(i,j)
     *
     *  in which
     *      m is calculated from i and j in a column-major way, with the restriction that i>j
     * 
     *  Note that this is equal to extracting to strict lower triangular matrix as a vector in column major form. Example:
     * 
     *          5
     *          1   5       -> (1, 2, 3)
     *          2   3   5
     */
    VectorX<Scalar> pairWiseStrictReduced() const {

        const auto dim = this->dimension();

        VectorX<Scalar> m = VectorX<Scalar>::Zero((dim * (dim - 1) / 2));  // strictly lower triangle has dim(dim-1)/2 parameters

        size_t vector_index = 0;
        for (size_t q = 0; q < dim; q++) {          // "column major" ordering for, so we do p first, then q
            for (size_t p = q + 1; p < dim; p++) {  // strict lower triangle means p > q
                m(vector_index) = this->operator()(p, q);
                vector_index++;
            }
        }

        return m;
    }


    /**
     *  @return the permanent using a combinatorial algorithm
     */
    double calculatePermanentCombinatorial() const {

        // The recursion ends when the given 'matrix' is just a number
        if ((this->rows() == 1) && (this->cols() == 1)) {
            return this->operator()(0, 0);
        }

        size_t j = 0;  // develop by the first column
        double value = 0.0;
        for (size_t i = 0; i < this->rows(); i++) {
            value += this->operator()(i, j) * SquareMatrix<Scalar>(this->calculateMinor(i, j)).calculatePermanentCombinatorial();  // need to convert because 'minor' is a Base function and isn't guaranteed to be square
        }

        return value;
    }


    /**
     *  @return the permanent of the given square matrix using the Ryser algorithm
     *
     *  Note that this algorithm does not work for dimensions larger than 64: see https://www.codeproject.com/Articles/21282/%2FArticles%2F21282%2FCompute-Permanent-of-a-Matrix-with-Ryser-s-Algorit
     */
    double calculatePermanentRyser() const {

        size_t n = this->dimension();

        // Loop over all submatrices of A
        double value = 0.0;  // value of the permanent
        size_t number_of_submatrices = boost::numeric::converter<size_t, double>::convert(std::pow(2, n));
        for (size_t S = 1; S < number_of_submatrices; S++) {  // there are no 'chosen columns' in S=0

            // Generate the current submatrix through the Gray code of S: if the bit is 1, the column is chosen
            size_t gray_code_value = grayCodeOf(S);
            size_t k = __builtin_popcountll(gray_code_value);  // number of columns

            Base X = Base::Zero(n, k);
            size_t j = 0;                                         // the column index in X
            while (gray_code_value != 0) {                        // loop over the set bits in the Gray code
                size_t index = __builtin_ctzll(gray_code_value);  // the index in the original matrix

                X.col(j) = this->col(index);

                gray_code_value ^= gray_code_value & -gray_code_value;  // flip the first set bit
                j++;
            }


            // Calculate the product of all the row sums and multiply by the sign
            double product_of_rowsums = X.array().rowwise().sum().prod();

            size_t t = n - k;  // number of deleted columns
            int sign = std::pow(-1, t);
            value += sign * product_of_rowsums;
        }

        return value;
    }
};


}  // namespace GQCP
