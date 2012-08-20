/*
	Numerical Integration by Gauss-Legendre Quadrature Formulas of high orders.
	High-precision abscissas and weights are used.

	Project homepage: http://www.holoborodko.com/pavel/?page_id=679
	Contact e-mail:   pavel@holoborodko.com

	Copyright (c)2007-2008 Pavel Holoborodko

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Contributors
	Konstantin Holoborodko - Optimization of Legendre polynomial computing.
*/

#ifndef __GAUSS_LEGENDRE_H__
#define __GAUSS_LEGENDRE_H__

template<class T>
class GaussLegendre {

  const T& obj_;
  double (T::*fptr_)(double) const;

 public:
  GaussLegendre(const T& obj, double (T::*fptr)(double) const)
   : obj_(obj)
   , fptr_(fptr)
  { }

  double integrate(int n, double, double);
};

struct GLAW{
  int n;
  double* x;
  double* w;
  static const unsigned int GLAWSIZE;
  static const GLAW glaw[];
};

#include "mathutils/gauss_legendre.hpp"

#endif /* __GAUSS_LEGENDRE_H__ */


