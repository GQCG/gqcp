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
#include "Utilities/type_traits.hpp"

#include <boost/algorithm/string.hpp>

#include <unsupported/Eigen/CXX11/Tensor>

#include <algorithm>
#include <iostream>
#include <string>


namespace GQCP {


/**
 *  An extension of the Eigen::Tensor class, with extra operations.
 *
 *  @tparam _Scalar         The scalar type of one of the elements of the tensor.
 *  @tparam _Rank           The rank of the tensor, i.e. the number of axes.
 *
 *  We have decided to inherit from Eigen::Tensor, because we will use different hierarchies: see also: https://eigen.tuxfamily.org/dox-devel/TopicCustomizing_InheritingMatrix.html.
 */
template <typename _Scalar, int _Rank>
class Tensor:
    public Eigen::Tensor<_Scalar, _Rank> {

public:
    // The scalar type of one of the elements of the tensor.
    using Scalar = _Scalar;

    // The rank of the tensor, i.e. the number of axes.
    static constexpr auto Rank = _Rank;

    // The type of 'this'.
    using Self = Tensor<Scalar, Rank>;

    // The Eigen base type.
    using Base = Eigen::Tensor<Scalar, Rank>;


public:
    /*
     *  MARK: Constructors
     */

    // Inherit `Eigen::Tensor`'s constructors.
    using Eigen::Tensor<Scalar, _Rank>::Tensor;


    /*
     *  MARK: Named constructors
     */

    /**
     *  @param T            a rank-4 tensor
     *
     *  @param i            1st starting index
     *  @param j            2nd starting index
     *  @param k            3rd starting index
     *  @param l            4th starting index
     *  @param cutoff       early cut-off of index iteration
     *
     *  @return a rank-4 tensor from an other rank-4 tensor, starting from given indices
     */
    template <int Z = Rank>
    static enable_if_t<Z == 4, Self> FromBlock(const Self& T, const size_t i, const size_t j, const size_t k, const size_t l, const size_t cutoff = 0) {

        Tensor<double, Rank> T_result {static_cast<long>(T.dimension(0) - i - cutoff),
                                       static_cast<long>(T.dimension(1) - j - cutoff),
                                       static_cast<long>(T.dimension(2) - k - cutoff),
                                       static_cast<long>(T.dimension(3) - l - cutoff)};
        T_result.setZero();

        for (size_t p = 0; p < T_result.dimension(0); p++) {
            for (size_t q = 0; q < T_result.dimension(1); q++) {
                for (size_t r = 0; r < T_result.dimension(2); r++) {
                    for (size_t s = 0; s < T_result.dimension(3); s++) {
                        T_result(p, q, r, s) = T(i + p, j + q, k + r, l + s);
                    }
                }
            }
        }

        return T_result;
    }


    /*
     *  MARK: Conversions
     */

    /**
     *  @return This as a const Eigen base.
     */
    const Base& Eigen() const { return static_cast<const Base&>(*this); }

    /**
     *  @return This as a non-const Eigen base.
     */
    Base& Eigen() { return static_cast<Base&>(*this); }


    /*
     *  MARK: Tensor operations
     */

    /**
     *  Add a matrix to a this tensor starting from given indices
     *
     *  @tparam r               indicates with which tensor index axis (0,1,2,3) the row index axis of the matrix should align 
     *  @tparam s               indicates with which tensor index axis (0,1,2,3) the column index axis of the matrix should align 
     *
     *  @param M                a matrix
     *  @param i                starting index for the 1st index axis of the tensor
     *  @param j                starting index for the 2nd index axis of the tensor
     *  @param k                starting index for the 3rd index axis of the tensor
     *  @param l                starting index for the 4th index axis of the tensor
     *
     *  @return a reference to updated this
     *
     *
     *  Example:
     *      Given a rank-4 tensor of dimensions (10,10,10,10), and a matrix M of dimensions (3,3)
     *       Input : <2,0> (M, 0, 2, 1, 3):
     *       <2,0> dictates that the row index axis of the matrix aligns with the 3rd index axis of the tensor (2nd starting from 0)
     *       and that the column index axis of the matrix aligns with the 1st index axis tensor (0th starting from 0)
     *       (0, 2, 1, 3) dictates the starting indexes to which the matrix is added, 
     *       given the input <2,0> this means the indices of the 2nd (indicated by the "2") and the 4th (indicated by the "3") axes 
     *       are held fixed because they do not correspond to the entries <2,0>.
     */
    template <size_t r, size_t s, int Z = Rank>
    enable_if_t<Z == 4, Self&> addBlock(const MatrixX<Scalar>& M, const size_t i, const size_t j, const size_t k, const size_t l) {

        // Initialize series of arrays with 1 or 0 values, so that the correct tensor indices given by the template argument correspond to the matrix indices
        size_t ia[4] = {1, 0, 0, 0};
        size_t ja[4] = {0, 1, 0, 0};
        size_t ka[4] = {0, 0, 1, 0};
        size_t la[4] = {0, 0, 0, 1};

        for (size_t x = 0; x < M.rows(); x++) {
            for (size_t y = 0; y < M.cols(); y++) {

                size_t i_effective = i + x * ia[r] + y * ia[s];
                size_t j_effective = j + x * ja[r] + y * ja[s];
                size_t k_effective = k + x * ka[r] + y * ka[s];
                size_t l_effective = l + x * la[r] + y * la[s];

                this->operator()(i_effective, j_effective, k_effective, l_effective) += M(x, y);
            }
        }

        return (*this);
    }


    /**
     *  Add a rank-4 tensor into this, starting from given indices
     *
     *  @param T                a rank-4 tensor
     *  @param i                starting index for the 1st index axis of the tensor
     *  @param j                starting index for the 2nd index axis of the tensor
     *  @param k                starting index for the 3rd index axis of the tensor
     *  @param l                starting index for the 4th index axis of the tensor
     *
     *  @return a reference to updated this
     */
    template <int Z = Rank>
    enable_if_t<Z == 4, Self&> addBlock(const Self& T, const size_t i, const size_t j, const size_t k, const size_t l) {

        for (size_t p = 0; p < T.dimension(0); p++) {
            for (size_t q = 0; q < T.dimension(1); q++) {
                for (size_t r = 0; r < T.dimension(2); r++) {
                    for (size_t s = 0; s < T.dimension(3); s++) {
                        this->operator()(i + p, j + q, k + r, l + s) += T(p, q, r, s);
                    }
                }
            }
        }

        return (*this);
    }


    /**
     *  @param start_i      the index at which the first rank should start
     *  @param start_j      the index at which the second rank should start
     *  @param start_k      the index at which the third rank should start
     *  @param start_l      the index at which the fourth rank should start
     * 
     *  @return a pair-wise reduced form of this rank-4 tensor. The elements of the tensor are put into the matrix such that
     *      M(m,n) = T(i,j,k,l)
     *
     *  in which
     *      m is calculated from i and j in a column-major way
     *      n is calculated from k and l in a column-major way
     */
    template <int Z = Rank>
    enable_if_t<Z == 4, Matrix<Scalar>> pairWiseReduced(const size_t start_i = 0, const size_t start_j = 0, const size_t start_k = 0, const size_t start_l = 0) const {

        // Initialize the resulting matrix
        const auto dims = this->dimensions();
        Matrix<Scalar> M {(dims[0] - start_i) * (dims[1] - start_j),
                          (dims[2] - start_k) * (dims[3] - start_l)};

        // Calculate the compound indices and bring the elements from the tensor over into the matrix
        size_t row_index = 0;
        for (size_t j = start_j; j < dims[1]; j++) {      // "column major" ordering for row_index<-i,j so we do j first, then i
            for (size_t i = start_i; i < dims[0]; i++) {  // in column major indices, columns are contiguous, so the first of two indices changes more rapidly

                size_t column_index = 0;
                for (size_t l = start_l; l < dims[3]; l++) {      // "column major" ordering for column_index<-k,l so we do l first, then k
                    for (size_t k = start_k; k < dims[2]; k++) {  // in column major indices, columns are contiguous, so the first of two indices changes more rapidly

                        M(row_index, column_index) = this->operator()(i, j, k, l);

                        column_index++;
                    }
                }

                row_index++;
            }
        }

        return M;
    }


    /**
     *  Contract this tensor with another one, using a NumPy 'einsum'-like API.
     * 
     *  @param rhs                  The right-hand side of the contraction.
     *  @param lhs_labels           The labels for the axes of the tensor on the left-hand side of the contraction.
     *  @param rhs_labels           The labels for the axes of the tensor on the right-hand side of the contraction.
     *  @param output_labels        The labels for the the resulting/output tensor.
     * 
     *  @tparam N                   The number of axes that should be contracted over.
     * 
     *  @example T1.einsum(T2, 'ijkl', 'ia', 'jkla') will contract the first axis of T1 (with labels 'ijkl') with the first axis of T2 (with labels 'ia') (because of the matching index labels 'i') and return a tensor whose axes are labelled as 'jkla'.
     * 
     *  @return The result of the tensor contraction.
     */
    template <int N, int LHSRank = Rank, int RHSRank>
    Tensor<Scalar, LHSRank + RHSRank - 2 * N> einsum(const Tensor<Scalar, RHSRank>& rhs, const std::string& lhs_labels, const std::string& rhs_labels, const std::string& output_labels) const {

        // Check the length of the indices.
        constexpr auto ResultRank = LHSRank + RHSRank - 2 * N;

        if (lhs_labels.size() != LHSRank) {
            throw std::invalid_argument("Tensor.einsum(const Tensor<Scalar, RHSRank>&, const std::string&, const std::string&, const std::string&): The number of indices for the left-hand side of the contraction does not match the rank of the left-hand side tensor.");
        }

        if (rhs_labels.size() != RHSRank) {
            throw std::invalid_argument("Tensor.einsum(const Tensor<Scalar, RHSRank>&, const std::string&, const std::string&, const std::string&): The number of indices for the right-hand side of the contraction does not match the rank of the right-hand side tensor.");
        }

        if (output_labels.size() != ResultRank) {
            throw std::invalid_argument("Tensor.einsum(const Tensor<Scalar, RHSRank>&, const std::string&, const std::string&, const std::string&): The number of output indices does not match the number of axes that should be contracted over.");
        }


        // Find the indices that should be contracted over, these are the positions of the axis labels that are both in the lhs- and rhs-indices.
        Eigen::array<Eigen::IndexPair<int>, N> contraction_pairs {};
        size_t array_position = 0;              // The index at which an `Eigen::IndexPair` should be placed.
        for (size_t i = 0; i < LHSRank; i++) {  // 'i' loops over the lhs-indices
            const auto current_label = lhs_labels[i];
            const auto match = rhs_labels.find(current_label);
            if (match != std::string::npos) {  // an actual match was found
                contraction_pairs[array_position] = Eigen::IndexPair<int>(i, match);
                array_position++;
            }
        }

        // Perform the contraction. It is an intermediate result, because we still have to align the tensor axes that Eigen's contraction module produces with the user's requested axes.
        Tensor<Scalar, ResultRank> T_intermediate = this->contract(rhs.Eigen(), contraction_pairs);

        // Finally, we should find the shuffle indices that map the obtained intermediate axes to the requested axes.
        // The intermediate axes are just concatenated from left to right, removing any duplicates.
        auto intermediate_indices = lhs_labels + rhs_labels;

        for (size_t i = 1; i < LHSRank + RHSRank; i++) {  // We should skip the first iteration, since there wouldn't be any duplicates anyways.

            // Check if the current index label appears in the substring before it.
            const auto current_label = intermediate_indices[i];
            const auto match = intermediate_indices.substr(0, i).find(current_label);

            if (match != std::string::npos) {  // an actual match was found
                intermediate_indices.erase(match, 1);
                intermediate_indices.erase(i - 1, 1);

                // We have to set back the current index to account for the removed indices.
                i -= 2;
            }
        }
        // Find the position of the intermediate axes' labels in the requested output labels in order to set up the required shuffle indices.
        Eigen::array<int, 4> shuffle_indices {};
        for (size_t i = 0; i < 4; i++) {
            const auto current_label = intermediate_indices[i];
            shuffle_indices[i] = output_labels.find(current_label);
        }

        return T_intermediate.shuffle(shuffle_indices);
    }


    /**
     *  Contract this tensor with another one, using a NumPy 'einsum'-like API.
     * 
     *  @param contraction_string   The string used to specify the wanted contraction, e.g. "ijkl,jk->il"
     *  @param rhs                  The right-hand side of the contraction.
     * 
     *  @tparam N                   The number of axes that should be contracted over.
     * 
     *  @example T1.einsum("ijkl,jk->il", T2) will contract the j and k axes of the second tensor with those of the first tensor, resulting in a rank 2 tensor with axes i and l.
     * 
     *  @return The result of the tensor contraction.
     */
    template <int N, int LHSRank = Rank, int RHSRank>
    Tensor<Scalar, LHSRank + RHSRank - 2 * N> einsum(std::string contraction_string, const Tensor<Scalar, RHSRank>& rhs) {
        // remove unnecessary symbols from string
        boost::erase_all(contraction_string, ">");
        boost::replace_all(contraction_string, "-", " ");
        boost::replace_all(contraction_string, ",", " ");

        // Convert to stringstream
        std::stringstream index_stream(contraction_string);

        // split the stringstream in the 3 necessary components
        std::vector<std::string> segment_list;
        boost::split(segment_list, contraction_string, boost::is_any_of(" "));

        // Determine number of axes to contract over
        std::vector<char> segment_1(segment_list[0].begin(), segment_list[0].end());
        std::vector<char> segment_2(segment_list[1].begin(), segment_list[1].end());
        std::vector<char> intersection;

        std::set_intersection(segment_1.begin(), segment_1.end(),
                              segment_2.begin(), segment_2.end(), std::back_inserter(intersection));

        const int Z = intersection.size();

        return this->einsum<Z>(rhs, segment_list[0], segment_list[1], segment_list[2]);
    }


    /**
     *  Contract this tensor with a matrix, using a NumPy 'einsum'-like API.
     * 
     *  @param contraction_string   The string used to specify the wanted contraction, e.g. "ijkl,jk->il"
     *  @param rhs                  The right-hand side of the contraction, a matrix in this case.
     * 
     *  @tparam N                   The number of axes that should be contracted over.
     * 
     *  @example T.einsum("ijkl,jk->il", M) will contract the j and k axes of the second tensor with those of the first tensor, resulting in a rank 2 tensor with axes i and l.
     * 
     *  @return The result of the tensor contraction.
     */
    template <int N, int LHSRank = Rank>
    Tensor<Scalar, LHSRank + 2 - 2 * N> einsum(std::string contraction_string, const Matrix<Scalar> rhs) {
        // convert matrix to rank 2 tensor
        const auto tensor_from_matrix = Tensor<Scalar, 2>(Eigen::TensorMap<Eigen::Tensor<const Scalar, 2>>(rhs.data(), rhs.rows(), rhs.cols()));
        return this->einsum<N>(contraction_string, tensor_from_matrix);
    }


    /*
     *  MARK: General information
     */

    /**
     *  @param other        the other tensor
     *
     *  @return if this tensor has the same dimensions as the other tensor
     */
    bool hasEqualDimensionsAs(const Self& other) const {

        for (size_t i = 0; i < Rank; i++) {
            if (this->dimension(i) != other.dimension(i)) {
                return false;
            }
        }

        return true;
    }


    /**
     *  @param other        the other tensor
     *  @param tolerance    the tolerance for element-wise comparison
     *
     *  @return if this is approximately equal to the other
     */
    template <int Z = Rank>
    enable_if_t<Z == 4, bool> isApprox(const Self& other, const double tolerance = 1.0e-12) const {

        if (!this->hasEqualDimensionsAs(other)) {
            throw std::invalid_argument("RankFourTensor<Scalar>::isApprox(Self, double): the tensors have different dimensions");
        }

        // Check every pair of values
        for (size_t i = 0; i < this->dimension(0); i++) {
            for (size_t j = 0; j < this->dimension(1); j++) {
                for (size_t k = 0; k < this->dimension(2); k++) {
                    for (size_t l = 0; l < this->dimension(3); l++) {
                        if (std::abs(this->operator()(i, j, k, l) - other(i, j, k, l)) > tolerance) {
                            return false;
                        }
                    }
                }
            }
        }  // rank-4 tensor traversing

        return true;
    }


    /**
     *  Print the contents of this to an output stream
     *
     *  @param output_stream        the stream used for outputting
     */
    template <int Z = Rank>
    enable_if_t<Z == 4> print(std::ostream& output_stream = std::cout) const {

        for (size_t i = 0; i < this->dimension(0); i++) {
            for (size_t j = 0; j < this->dimension(1); j++) {
                for (size_t k = 0; k < this->dimension(2); k++) {
                    for (size_t l = 0; l < this->dimension(3); l++) {
                        output_stream << i << ' ' << j << ' ' << k << ' ' << l << "  " << this->operator()(i, j, k, l) << std::endl;
                    }
                }
            }
        }
    }

    /**
     *  Convert the given tensor to a matrix with the specified dimensions
     *
     *  @param rows        the dimension of the rows of the output matrix
     *  @param cols        the dimension of the columns of the output matrix
     */
    const GQCP::Matrix<Scalar> toMatrix(const size_t rows, const size_t cols) const {

        const auto rank_two_tensor = *this;
        return GQCP::Matrix<Scalar>(Eigen::Map<const Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>>(rank_two_tensor.data(), rows, cols));
    }
};


}  // namespace GQCP
