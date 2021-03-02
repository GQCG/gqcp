/* 
 *  This file is part of the Chronus Quantum (ChronusQ) software package
 *  
 *  Copyright (C) 2014-2020 Li Research Group (University of Washington)
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *  
 *  Contact the Developers:
 *    E-Mail: xsli@uw.edu
 */

// The interfaces and implementations from this file have been adapted from
// those in the Chronus Quantum (ChronusQ) software package.


#pragma once

#include "Basis/Integrals/Interfaces/ChronusQ/aliases.hpp"
#include "Basis/Integrals/Interfaces/LibintInterfacer.hpp"

#include <vector>


namespace ChronusQ {


struct ComplexGIAOIntEngine {

    static std::vector<std::vector<dcomplex>> computeGIAOOverlapS(libint2::ShellPair&, libint2::Shell&, libint2::Shell&, const std::array<double, 3>&);

    // calculate the uncontracted overlap of (s||s) type for a shellpair
    static std::vector<dcomplex> computecompOverlapss(libint2::ShellPair&, libint2::Shell&, double*, libint2::Shell&, double*);

    // complex overlap horizontal recursion for contracted case
    static dcomplex comphRRSab(libint2::ShellPair&, libint2::Shell&, libint2::Shell&, double*, std::vector<dcomplex>&, int, int*, int, int*);

    // complex overlap horizontal recursion iPP specific for uncontracted case
    static dcomplex comphRRiPPSab(libint2::ShellPair::PrimPairData&, libint2::Shell&, libint2::Shell&, double*, dcomplex, int, int*, int, int*);

    // complex overlap vertical recursion for uncontracted case
    static dcomplex compvRRSa0(libint2::ShellPair::PrimPairData&, libint2::Shell&, double*, dcomplex, int, int*);
};


}  // namespace ChronusQ
