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


#include "Mathematical/AbstractFunction/ScalarFunction.hpp"

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <vector>


namespace GQCP {


/**
 *  A class template representing a linear combination of scalar functions.
 *
 *  @tparam _Function                   the type of scalar function
 *  @tparam _CoefficientScalar          the type of scalar that is used as coefficient
 */
template <typename _CoefficientScalar, typename _Function>
class LinearCombination:
    public ScalarFunction<typename _Function::Valued, typename _Function::Scalar, _Function::Cols> {

public:
    using CoefficientScalar = _CoefficientScalar;
    using Function = _Function;

    static_assert(std::is_base_of<ScalarFunction<typename Function::Valued, typename Function::Scalar, Function::Cols>, Function>::value, "LinearCombination: Function must derive from ScalarFunction");


protected:
    std::vector<CoefficientScalar> m_coefficients;
    std::vector<Function> m_functions;


public:
    /*
     *  CONSTRUCTORS
     */

    /**
     *  @param coefficients     the coefficients of the linear combination
     *  @param functions        the scalar functions of the linear combination
     */
    LinearCombination(const std::vector<CoefficientScalar>& coefficients, const std::vector<Function>& functions) :
        m_coefficients {coefficients},
        m_functions {functions} {

        if (coefficients.size() != functions.size()) {
            throw std::invalid_argument("LinearCombination(std::vector<CoefficientScalar>, std::vector<Function>): the number of coefficients and functions should match");
        }
    }


    /**
     *  Default constructor: construct a 'zero vector', i.e. an empty linear combination
     */
    LinearCombination() :
        LinearCombination(std::vector<CoefficientScalar> {}, std::vector<Function> {}) {}


    /**
     *  Constructor providing a 'linear combination' of just one scalar function
     *
     *  @param coefficient      the one coefficient that belongs to the function
     *  @param function         one single scalar function
     */
    LinearCombination(const CoefficientScalar coefficient, const Function& function) :
        LinearCombination(std::vector<CoefficientScalar> {coefficient}, std::vector<Function> {function}) {}


    /**
     *  Constructor providing a 'linear combination' of just one scalar function, defaulting the coefficient to 1.0
     *
     *  @param function         one single scalar function
     */
    LinearCombination(const Function& function) :
        LinearCombination(1.0, function) {}


    /**
     *  The constructor for a 'zero vector' given an integer argument
     *
     *  This constructor is added to fix errors in Eigen concerning 'Scalar(0)';
     */
    LinearCombination(const int zero) :
        LinearCombination() {

        if (zero != 0) {
            throw std::invalid_argument("LinearCombination(int): Can't convert a non-zero integer to a zero vector");
        }
    }


    /*
     *  GETTERS
     */


    /*
     *  OPERATORS implementing the abstract notion of linear combinations
     */

    /**
     *  @param rhs      the right-hand side of the addition
     *
     *  @return the sum of this and the right-hand side
     */
    LinearCombination<CoefficientScalar, Function> operator+(const LinearCombination<CoefficientScalar, Function>& rhs) const {
        LinearCombination result = *this;
        result.append(rhs.m_coefficients, rhs.m_functions);
        return result;
    }

    /**
     *  @param rhs      the right-hand side of the addition
     *
     *  @return the sum of this and the right-hand side
     */
    LinearCombination<CoefficientScalar, Function>& operator+=(const LinearCombination<CoefficientScalar, Function>& rhs) {
        this->append(rhs.m_coefficients, rhs.m_functions);
        return *this;
    }

    /**
     *  @param scalar       the scalar to be used in the multiplication
     *
     *  @return the product of this with a scalar
     */
    LinearCombination<CoefficientScalar, Function> operator*(const CoefficientScalar& scalar) const {

        if (std::abs(scalar) < 1.0e-16) {  // multiplication by zero returns a 'zero vector'
            return LinearCombination();
        }

        auto coefficients = this->m_coefficients;

        for (auto& coeff : coefficients) {
            coeff *= scalar;
        }

        return LinearCombination(coefficients, this->m_functions);
    }

    /**
     *  @param scalar       the scalar to be used in the multiplication
     *  @param rhs          the right-hand side of the multiplication
     *
     *  @return the product of a LinearCombination with a scalar
     */
    friend LinearCombination<CoefficientScalar, Function> operator*(CoefficientScalar scalar, const LinearCombination<CoefficientScalar, Function>& rhs) {
        return rhs.operator*(scalar);
    }

    using ScalarFunction<typename Function::Valued, typename Function::Scalar, Function::Cols>::operator*;


    /**
     *  @return the negative of the linear combination
     */
    LinearCombination<CoefficientScalar, Function> operator-() const {
        return this->operator*(-1.0);
    }

    /**
     *  @param rhs          the right-hand side of the subtraction
     *
     *  @return the difference of this and a right-hand side
     */
    LinearCombination<CoefficientScalar, Function> operator-(const LinearCombination<CoefficientScalar, Function>& rhs) const {
        return this->operator+(-rhs);
    }


    /*
     *  OTHER OPERATORS
     */

    /**
     *  @param x        the vector/point at which the scalar function is to be evaluated
     *
     *  @return the scalar function value of this linear combination at the given point
     */
    typename Function::Valued operator()(const Vector<typename Function::Scalar, Function::Cols>& x) const override {
        size_t n = this->m_functions.size();

        CoefficientScalar value {};  // default initialization
        for (size_t i = 0; i < n; i++) {
            value += this->m_coefficients[i] * this->m_functions[i].operator()(x);  // evaluate every function of the linear combination
        }

        return value;
    }


    /**
     *  @param rhs      the right-hand side of the operator ==
     *
     *  @return whether two linear combinations are considered equal
     */
    bool operator==(const LinearCombination<CoefficientScalar, Function>& rhs) const {
        return (this->m_coefficients == rhs.m_coefficients) && (this->m_functions == rhs.m_functions);
    }


    /*
     *  PUBLIC METHODS
     */

    /**
     *  Append the given coefficients and functions to this linear combination
     *
     *  @param coefficients     the coefficients that should be appended to this linear combination
     *  @param functions        the functions that should be appended to this linear combination
     */
    void append(const std::vector<CoefficientScalar>& coefficients, const std::vector<Function>& functions) {

        if (coefficients.size() != functions.size()) {
            throw std::invalid_argument("LinearCombination::append(std::vector<CoefficientScalar>, std::vector<Function>): the number of coefficients and functions should match");
        }

        this->m_coefficients.insert(this->m_coefficients.end(), coefficients.begin(), coefficients.end());
        this->m_functions.insert(this->m_functions.end(), functions.begin(), functions.end());
    }


    /**
     *  @param i                the index
     * 
     *  @return the coefficient of this linear combination that corresponds to the given index
     */
    const CoefficientScalar& coefficient(const size_t i) const { return this->m_coefficients[i]; }

    /**
     *  @return the coefficients of the linear combination
     */
    const std::vector<CoefficientScalar>& coefficients() const { return this->m_coefficients; }

    /**
     *  @return a textual description of self
     */
    std::string description() const {

        std::string description {};
        for (size_t i = 0; i < this->length(); i++) {
            // Provide the coefficient and the function's description.
            description += (boost::format("(%|.3f|) %s") % this->coefficient(i) % this->function(i).description()).str();

            // Add a '+' when we're not at the end.
            if (i != this->length() - 1) {
                description += " + ";
            }
        }

        return description;
    }


    /**
     *  @param i                the index
     * 
     *  @return the function of this linear combination that corresponds to the given index
     */
    const Function& function(const size_t i) const { return this->m_functions[i]; }

    /**
     *  @return the scalar functions of this linear combination
     */
    const std::vector<Function>& functions() const { return this->m_functions; }

    /**
     *  @return the length of the linear combination, i.e. the number of coefficients/functions inside it
     */
    size_t length() const { return this->m_coefficients.size(); }
};


}  // namespace GQCP


/*
 *  Make GQCP::LinearCombination<Function> an Eigen scalar type
 */

namespace Eigen {

template <typename CoefficientScalar, typename Function>
struct NumTraits<GQCP::LinearCombination<CoefficientScalar, Function>>: public NumTraits<double> {  // permits to get the epsilon, dummy_precision, lowest, highest functions

    using Real = GQCP::LinearCombination<CoefficientScalar, Function>;
    using NonInteger = GQCP::LinearCombination<CoefficientScalar, Function>;
    using Nested = GQCP::LinearCombination<CoefficientScalar, Function>;

    enum {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
        ReadCost = 1,
        AddCost = 5,
        MulCost = 1000  // just put something big
    };
};


// Enable the scalar product of a LinearCombination<CoefficientScalar, Function> with its own CoefficientScalar (both sides)

template <typename CoefficientScalar, typename Function>
struct ScalarBinaryOpTraits<GQCP::LinearCombination<CoefficientScalar, Function>, CoefficientScalar,
                            Eigen::internal::scalar_product_op<GQCP::LinearCombination<CoefficientScalar, Function>, CoefficientScalar>> {

    using ReturnType = GQCP::LinearCombination<CoefficientScalar, Function>;
};

template <typename CoefficientScalar, typename Function>
struct ScalarBinaryOpTraits<CoefficientScalar, GQCP::LinearCombination<CoefficientScalar, Function>,
                            Eigen::internal::scalar_product_op<CoefficientScalar, GQCP::LinearCombination<CoefficientScalar, Function>>> {

    using ReturnType = GQCP::LinearCombination<CoefficientScalar, Function>;
};


}  // namespace Eigen
