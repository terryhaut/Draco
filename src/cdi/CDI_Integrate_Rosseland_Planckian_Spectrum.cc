//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi/CDI_integrate_Rosseland_Planckian_Spectrum.cc
 * \author Kelly Thompson
 * \date   Thu Jun 22 16:22:07 2000
 * \brief  CDI class implementation file.
 * \note   Copyright (C) 2016 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id: CDI.cc 7388 2015-01-22 16:02:07Z kellyt $
//---------------------------------------------------------------------------//

#include "CDI.hh"

namespace rtt_cdi {
//---------------------------------------------------------------------------//
// Rosseland Spectrum Integrators
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/*!
 * The arguments to this function must all be in consistent units. For
 * example, if low and high are expressed in keV, then the temperature must
 * also be expressed in keV. If low and high are in Hz and temperature is in
 * K, then low and high must first be multiplied by Planck's constant and
 * temperature by Boltzmann's constant before they are passed to this function.
 *
 * \brief Integrate the Planckian and Rosseland spectrum over a frequency
 *        range.
 *
 * \param low Lower limit of frequency range.
 *
 * \param high Higher limit of frequency range.
 *
 * \param T Temperature (must be greater than 0.0).
 *
 * \planck On return, contains the integrated normalized Planckian from
 * low to high.
 *
 * \rosseland On return, contains the integrated normalized Rosseland from
 * low to high
 *
 * \f[
 * planck(T) = \int_{\nu_1}^{\nu_2}{B(\nu,T)d\nu}
 * rosseland(T) = \int_{\nu_1}^{\nu_2}{\frac{\partial B(\nu,T)}{\partial T}d\nu}
 * \f]
 */
void CDI::integrate_Rosseland_Planckian_Spectrum(double low, double high,
                                                 double const T, double &planck,
                                                 double &rosseland) {
  Require(low >= 0.0);
  Require(high >= low);
  Require(T >= 0.0);

  if (T == 0.0) {
    planck = 0.0;
    rosseland = 0.0;
    return;
  }

  double planck_high, rosseland_high;
  double planck_low, rosseland_low;

  // Sale the frequencies by temperature
  low /= T;
  high /= T;

  double const exp_low = std::exp(-low);
  double const exp_high = std::exp(-high);

  integrate_planck_rosseland(low, exp_low, planck_low, rosseland_low);
  integrate_planck_rosseland(high, exp_high, planck_high, rosseland_high);

  planck = planck_high - planck_low;
  rosseland = rosseland_high - rosseland_low;

  return;
}

} // end namespace rtt_cdi

//---------------------------------------------------------------------------//
// end of CDI_integrate_Rosseland_Planckian_Spectrum.cc
//---------------------------------------------------------------------------//
