//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi/GrayOpacity.hh
 * \author Kelly Thompson
 * \date   Mon Jan 8 15:02:21 2001
 * \brief  GrayOpacity class header file (an abstract class)
 * \note   Copyright (C) 2016 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#ifndef __cdi_GrayOpacity_hh__
#define __cdi_GrayOpacity_hh__

#include "OpacityCommon.hh"  // Stuff that is common between Gray and
                             // Multigroup.
#include "ds++/config.h"
#include <vector>
#include <string>

namespace rtt_cdi
{
    
//========================================================================
/*!
 * \class GrayOpacity
 *
 * \brief This is a pure virtual class that defines a standard interface for
 * all derived GrayOpacity objects.
 *
 * Any derived GrayOpacity object must provide as a minumum the functionality
 * outlined in this routine.  This functionality includes access to the data
 * grid and the ability to return interpolated opacity values.
 */
/*!
 * \example cdi/test/tDummyOpacity.cc
 * \example cdi/test/tCDI.cc
 */
//========================================================================
    
class DLL_PUBLIC_cdi GrayOpacity
{
    // DATA
	
    // There is no data for a pure virtual object.  This class
    // provides an interface and does not preserve state.
	
  public:
	
    // ---------- //
    // Destructor //
    // ---------- //
	
    /*!
     * \brief Default GrayOpacity() destructor.
     *
     * This is required to correctly release memory when any
     * object derived from GrayOpacity is destroyed.
     */
    virtual ~GrayOpacity() {/*empty*/};
	
    // --------- //
    // Accessors //
    // --------- //
	
    /*!
     * \brief Opacity accessor that returns a single opacity that 
     *     corresponds to the provided temperature and density.
     *
     * \param targetTemperature The temperature value for which an
     *     opacity value is being requested (keV).
     *
     * \param targetDensity The density value for which an opacity 
     *     value is being requested (g/cm^3).
     *
     * \return A single interpolated opacity (cm^2/g).
     */
    virtual double getOpacity( 
	double targetTemperature,
	double targetDensity ) const = 0; 
	
    /*!
     * \brief Opacity accessor that returns a vector of opacities that
     *     correspond to the provided vector of temperatures and a
     *     single density value. 
     *
     * \param targetTemperature A vector of temperature values for
     *     which opacity values are being requested (keV).
     *
     * \param targetDensity The density value for which an opacity 
     *     value is being requested (g/cm^3).
     *
     * \return A vector of opacities (cm^2/g).
     */
    virtual std::vector< double >  getOpacity( 
	    const  std::vector< double >& targetTemperature,
	    double targetDensity ) const = 0; 
	
    /*!
     * \brief Opacity accessor that returns a vector of opacities
     *     that correspond to the provided vector of densities and a
     *     single temperature value. 
     *
     * \param targetTemperature The temperature value for which an 
     *     opacity value is being requested (keV).
     *
     * \param targetDensity A vector of density values for which
     *     opacity values are being requested (g/cm^3).
     *
     * \return A vector of opacities (cm^2/g).
     */
    virtual std::vector< double > getOpacity( 
	    double targetTemperature,
	    const std::vector<double>& targetDensity ) const = 0;

    /*!
     * \brief Query whether the data is in tables or functional form.
     */
    virtual bool data_in_tabular_form() const = 0; 

    /*!
     * \brief Query to determine the reaction model.
     */
    virtual rtt_cdi::Reaction getReactionType() const = 0;

    /*!
     * \brief Query to determine the physics model.
     */
    virtual rtt_cdi::Model getModelType() const = 0;
	
    /*!
     * \brief Returns a string that describes the EnergyPolicy.
     *     Currently this will return either "mg" or "gray."
     */ 
    virtual std::string getEnergyPolicyDescriptor() const = 0;
	
    /*!
     * \brief Returns a "plain English" description of the opacity
     *     data that this class references. (e.g. "Gray Rosseland
     *     Scattering".) 
     */
    virtual std::string getDataDescriptor() const = 0;
	
    /*!
     * \brief Returns the name of the associated data file (if any).
     */
    virtual  std::string getDataFilename() const = 0;
	
    /*!
     * \brief Returns a vector of temperatures that define the cached
     *     opacity data table (keV).
     */
    virtual std::vector<double> getTemperatureGrid() const = 0;
	
    /*!
     * \brief Returns a vector of densities that define the cached
     *     opacity data table (g/cm^3).
     */
    virtual std::vector<double> getDensityGrid() const = 0;
	
    /*!
     * \brief Returns the size of the temperature grid.
     */
    virtual size_t getNumTemperatures() const = 0;
	
    /*! 
     * \brief Returns the size of the density grid.
     */
    virtual size_t getNumDensities() const = 0;

    /*!
     * \brief Interface for packing a derived GrayOpacity object.
     *
     * Note, the user hands the return value from this function to a derived
     * GrayOpacity constructor.  Thus, even though one can pack a GrayOpacity
     * through a base class pointer, the client must know the derived type
     * when unpacking.
     */
    virtual std::vector<char> pack() const = 0;

	/*!
	 * \brief Returns the general opacity model type (Analytic or Gandolf),
	 * defined in OpacityCommon.hh
	 */
	virtual rtt_cdi::OpacityModelType getOpacityModelType() const = 0;
	
}; // end of class GrayOpacity    
    
} // end namespace rtt_cdi

#endif // __cdi_GrayOpacity_hh__

//---------------------------------------------------------------------------//
// end of cdi/GrayOpacity.hh
//---------------------------------------------------------------------------//