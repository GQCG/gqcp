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


namespace GQCP {


/**
 *  A tag designating types corresponding to spin-orbitals whose spatial parts are equal.
 */
class RestrictedSpinorTag {};

/**
 *  A tag designating types corresponding to spin-orbitals whose spatial parts are equal.
 */
using RestrictedSpinOrbitalTag = RestrictedSpinorTag;

/**
 *  A tag designating types corresponding to spin-orbitals whose spatial parts may differ.
 */
class UnrestrictedSpinorTag {};

/**
 *  A tag designating types corresponding to spin-orbitals whose spatial parts may differ.
 */
using UnrestrictedSpinOrbitalTag = UnrestrictedSpinorTag;

/**
 *  A tag designating types corresponding to spinors which may have non-zero alpha- and beta-components.
 */
class GeneralSpinorTag {};


}  // namespace GQCP