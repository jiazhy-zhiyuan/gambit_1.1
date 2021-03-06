// ====================================================================
// This file is part of FlexibleSUSY.
//
// FlexibleSUSY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// FlexibleSUSY is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with FlexibleSUSY.  If not, see
// <http://www.gnu.org/licenses/>.
// ====================================================================

// File generated at Sat 27 Aug 2016 12:48:22

/**
 * @file CMSSMNoFV_two_scale_model_slha.cpp
 * @brief CMSSMNoFV model class wrapper for SLHA conversion
 */

#include "CMSSMNoFV_two_scale_model_slha.hpp"
#include "slha_io.hpp"
#include "ckm.hpp"
#include "pmns.hpp"

namespace flexiblesusy {

#define CLASSNAME CMSSMNoFV_slha<Two_scale>
#define LOCALPHYSICAL(p) physical.p

CLASSNAME::CMSSMNoFV_slha(const CMSSMNoFV_input_parameters& input_)
   : CMSSMNoFV<Two_scale>(input_)
   , physical_slha()
   , ckm(Eigen::Matrix<std::complex<double>,3,3>::Identity())
   , pmns(Eigen::Matrix<std::complex<double>,3,3>::Identity())
   , convert_masses_to_slha(true)
{
}

/**
 * Copy constructor.  Copies from base class (two-scale model class in
 * BPMZ convention) and converts parameters to SLHA.
 *
 * @param model_ model class in BPMZ convention
 * @param do_convert_masses_to_slha whether to convert majorana
 *    fermion masses to SLHA convention (allow them to be negative)
 */
CLASSNAME::CMSSMNoFV_slha(const CMSSMNoFV<Two_scale>& model_,
                            bool do_convert_masses_to_slha)
   : CMSSMNoFV<Two_scale>(model_)
   , convert_masses_to_slha(do_convert_masses_to_slha)
{
   convert_to_slha();
}

CLASSNAME::~CMSSMNoFV_slha()
{
}

void CLASSNAME::clear()
{
   CMSSMNoFV<Two_scale>::clear();
   physical_slha.clear();
}

void CLASSNAME::calculate_spectrum()
{
   CMSSMNoFV<Two_scale>::calculate_spectrum();
   convert_to_slha();
}

void CLASSNAME::convert_to_slha()
{
   physical_slha = get_physical();

   if (convert_masses_to_slha)
      physical_slha.convert_to_slha();

   convert_yukawa_couplings_to_slha();
   calculate_ckm_matrix();
   calculate_pmns_matrix();
   convert_trilinear_couplings_to_slha();
   convert_soft_squared_masses_to_slha();
}

void CLASSNAME::calculate_ckm_matrix()
{

}

void CLASSNAME::calculate_pmns_matrix()
{
   pmns << 1, 0, 0, 0, 1, 0, 0, 0, 1;

}

/**
 * Convert Yukawa couplings to SLHA convention
 */
void CLASSNAME::convert_yukawa_couplings_to_slha()
{
   Yu_slha = Yu.diagonal().real();
   Yd_slha = Yd.diagonal().real();
   Ye_slha = Ye.diagonal().real();

}

/**
 * Convert trilinear couplings to SLHA convention
 */
void CLASSNAME::convert_trilinear_couplings_to_slha()
{
   TYu_slha = TYu.real();
   TYd_slha = TYd.real();
   TYe_slha = TYe.real();

}

/**
 * Convert trilinear couplings to SLHA convention
 */
void CLASSNAME::convert_soft_squared_masses_to_slha()
{
   mq2_slha = mq2.real();
   mu2_slha = mu2.real();
   md2_slha = md2.real();
   ml2_slha = ml2.real();
   me2_slha = me2.real();

}

const CMSSMNoFV_physical& CLASSNAME::get_physical_slha() const
{
   return physical_slha;
}

CMSSMNoFV_physical& CLASSNAME::get_physical_slha()
{
   return physical_slha;
}

void CLASSNAME::print(std::ostream& ostr) const
{
   CMSSMNoFV<Two_scale>::print(ostr);

   ostr << "----------------------------------------\n"
           "SLHA convention:\n"
           "----------------------------------------\n";
   physical_slha.print(ostr);
}

void CLASSNAME::set_convert_masses_to_slha(bool flag)
{
   convert_masses_to_slha = flag;
}

std::ostream& operator<<(std::ostream& ostr, const CMSSMNoFV_slha<Two_scale>& model)
{
   model.print(ostr);
   return ostr;
}

} // namespace flexiblesusy
