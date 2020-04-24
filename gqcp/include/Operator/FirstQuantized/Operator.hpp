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


#include "Molecule/Molecule.hpp"
#include "Operator/FirstQuantized/CoulombRepulsionOperator.hpp"
#include "Operator/FirstQuantized/ElectronicDipoleOperator.hpp"
#include "Operator/FirstQuantized/ElectronicSpinOperator.hpp"
#include "Operator/FirstQuantized/KineticOperator.hpp"
#include "Operator/FirstQuantized/NuclearAttractionOperator.hpp"
#include "Operator/FirstQuantized/NuclearDipoleOperator.hpp"
#include "Operator/FirstQuantized/NuclearRepulsionOperator.hpp"
#include "Operator/FirstQuantized/OverlapOperator.hpp"


namespace GQCP {


/**
 *  A class that is used to construct operators using static methods, much like a factory class.
 */
class Operator {
public:
    // PUBLIC STATIC METHODS

    /**
     *  @return a CoulombRepulsionOperator
     */
    static CoulombRepulsionOperator Coulomb();

    /**
     *  @param origin               the origin of the dipole operator
     * 
     *  @return an ElectronicDipoleOperator
     */
    static ElectronicDipoleOperator ElectronicDipole(const Vector<double, 3>& o = Vector<double, 3>::Zero());

    /**
     *  @return an ElectronicSpinOperator
     */
    static ElectronicSpinOperator ElectronicSpin();

    /**
     *  @return a KineticOperator
     */
    static KineticOperator Kinetic();

    /**
     *  @param nuclear_framework            the nuclear framework
     * 
     *  @return a NuclearAttractionOperator
     */
    static NuclearAttractionOperator NuclearAttraction(const NuclearFramework& nuclear_framework);

    /**
     *  @param mol              the molecule that contains the nuclear framework
     * 
     *  @return a NuclearAttractionOperator
     */
    static NuclearAttractionOperator NuclearAttraction(const Molecule& mol);

    /**
     *  @param mol                  the molecule that contains the nuclear framework
     *  @param origin               the origin of the dipole operator
     * 
     *  @return a NuclearDipoleOperator
     */
    static NuclearDipoleOperator NuclearDipole(const Molecule& mol, const Vector<double, 3>& o = Vector<double, 3>::Zero());

    /**
     *  @param mol              the molecule that contains the nuclear framework
     * 
     *  @return a NuclearRepulsionOperator
     */
    static NuclearRepulsionOperator NuclearRepulsion(const Molecule& mol);

    /**
     *  @return an OverlapOperator
     */
    static OverlapOperator Overlap();
};


}  // namespace GQCP
