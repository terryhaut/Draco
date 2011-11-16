//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi_ipcress/IpcressDataTable.cc
 * \author Kelly Thompson
 * \date   Thu Oct 12 09:39:22 2000
 * \brief  Implementation file for IpcressDataTable objects.
 * \note   Copyright (C) 2001-2010 Los Alamos National Security, LLC.
 */
//---------------------------------------------------------------------------//
// $Id$
//---------------------------------------------------------------------------//

#include "IpcressDataTable.hh"  // the associated header file.
#include "IpcressFile.hh"       // we have a SP to a GandofFile object.
#include "cdi/OpacityCommon.hh" // defines Model and Reaction
                                // enumerated values.
#include "ds++/Assert.hh"
#include <cmath>                // we need to define log(double)


// ------------------------- //
// NAMESPACE RTT_CDI_IPCRESS //
// ------------------------- //

namespace rtt_cdi_ipcress
{

// helper functions: local to file scope
double unary_log( double x ) { return std::log( x ); }
    

//---------------------------------------------------------------------------//
/*!
 * \brief IpcressData Table constructor.
 *
 *     The constructor requires that the data state be completely defined.
 *     With this information the DataTypeKey is set, then the data table sizes
 *     are loaded and finally the table data is loaded.
 *
 * \param opacityEnergyDescriptor This string variable specifies the energy
 *     model { "gray" or "mg" } for the opacity data contained in this
 *     IpcressDataTable object.
 * \param opacityModel This enumerated value specifies the physics model {
 *     Rosseland or Planck } for the opacity data contained in this object.
 *     The enumeration is defined in IpcressOpacity.hh
 * \param opacityReaction This enumerated value specifies the interaction
 *     model { total, scattering, absorption " for the opacity data contained
 *     in this object.  The enumeration is defined in IpcressOpacity.hh
 * \param vKnownKeys This vector of strings is a list of data keys that the
 *     IPCRESS file knows about.  This list is read from the IPCRESS file when
 *     a IpcressOpacity object is instantiated but before the associated
 *     IpcressDataTable object is created.
 * \param matID The material identifier that specifies a particular material
 *     in the IPCRESS file to associate with the IpcressDataTable container.
 * \param spIpcressFile A ds++ SmartPointer to a IpcressFile object.  One
 *     GanolfFile object should exist for each IPCRESS file.  Many
 *     IpcressOpacity (and thus IpcressDataTable) objects may point to the
 *     same IpcressFile object.
 */
IpcressDataTable::IpcressDataTable( 
    std::string              const & in_opacityEnergyDescriptor,
    rtt_cdi::Model                   in_opacityModel, 
    rtt_cdi::Reaction                in_opacityReaction,
    std::vector<std::string> const & in_vKnownKeys,
    size_t                              in_matID,
    rtt_dsxx::SP< const IpcressFile > const & in_spIpcressFile )
    : ipcressDataTypeKey( "" ),
      dataDescriptor( "" ),
      opacityEnergyDescriptor ( in_opacityEnergyDescriptor ),
      opacityModel( in_opacityModel ),
      opacityReaction( in_opacityReaction ),
      vKnownKeys ( in_vKnownKeys ),
      matID ( in_matID ),
      spIpcressFile( in_spIpcressFile ),
      numTemperatures( 0 ),
      numDensities( 0 ),
      numGroupBoundaries( 0 ),
      numOpacities( 0 ),
      logTemperatures(),
      temperatures(),
      logDensities(),
      densities(),
      groupBoundaries(),
      logOpacities()
{
    // Obtain the Ipcress keyword for the opacity data type specified by the
    // EnergyPolicy, opacityModel and the opacityReaction.  Valid keywords
    // are: { ramg, rsmg, rtmg, pmg, rgray, ragray, rsgray, pgray } This
    // function also ensures that the requested data type is available in the
    // IPCRESS file.
    setIpcressDataTypeKey();
    
    // Retrieve the size of the data set and resize the vector containers.
    setIpcressDataTableSizes();
    
    // Retrieve table data (temperatures, densities, group boundaries and
    // opacities.  These are stored as logorithmic values.
    loadDataTable();
    
} // end of IpcressDataTable constructor.

    
// ----------------- //
// PRIVATE FUNCTIONS //
// ----------------- //


/*!
 * \brief This function sets both "ipcressDataTypeKey" and
 *     "dataDescriptor" based on the values given for
 *     opacityEnergyDescriptor, opacityModel and opacityReaction.
 */
void IpcressDataTable::setIpcressDataTypeKey( ) const
{
    // Build the Ipcress key for the requested data.  Valid
    // keys are:
    // { ramg, rsmg, rtmg, pmg, rgray, ragray, rsgray, pgray }

    if ( opacityEnergyDescriptor == "gray" )
    {
        switch ( opacityModel ) {
            case ( rtt_cdi::ROSSELAND ) :

                switch ( opacityReaction ) {
                    case ( rtt_cdi::TOTAL ) :
                        ipcressDataTypeKey = "rgray";
                        dataDescriptor = "Gray Rosseland Total";
                        break;
                    case ( rtt_cdi::ABSORPTION ) :
                        ipcressDataTypeKey = "ragray";
                        dataDescriptor = "Gray Rosseland Absorption";
                        break;
                    case ( rtt_cdi::SCATTERING ) :
                        // *** NOTE: THIS KEY DOES NOT ACTUALLY EVER EXIST *** //
                        // See LA-UR-01-5543
                        ipcressDataTypeKey = "rsgray";
                        dataDescriptor = "Gray Rosseland Scattering";
                        break;
                    default :
                        Assert(false);
                        break;
                }
                break;

            case ( rtt_cdi::PLANCK ) :
			
                switch ( opacityReaction ) {
                    case ( rtt_cdi::TOTAL ) :
                        // *** NOTE: THIS KEY DOES NOT ACTUALLY EVER EXIST *** //
                        // See LA-UR-01-5543
                        ipcressDataTypeKey = "ptgray";
                        dataDescriptor = "Gray Planck Total";
                        break;
                    case ( rtt_cdi::ABSORPTION ) :
                        ipcressDataTypeKey = "pgray";
                        dataDescriptor = "Gray Planck Absorption";
                        break;
                    case ( rtt_cdi::SCATTERING ) :
                        // *** NOTE: THIS KEY DOES NOT ACTUALLY EVER EXIST *** //
                        // See LA-UR-01-5543
                        ipcressDataTypeKey = "psgray";
                        dataDescriptor = "Gray Planck Scattering";
                        break;
                    default :
                        Assert(false);
                        break;
                }
                break;
			
            default :
                Assert(false);
                break;
        }
    }
    else // "mg"
    {
        switch ( opacityModel ) {
            case ( rtt_cdi::ROSSELAND ) :

                switch ( opacityReaction ) {
                    case ( rtt_cdi::TOTAL ) :
                        ipcressDataTypeKey = "rtmg";
                        dataDescriptor = "Multigroup Rosseland Total";
                        break;
                    case ( rtt_cdi::ABSORPTION ) :
                        ipcressDataTypeKey = "ramg";
                        dataDescriptor = "Multigroup Rosseland Absorption";
                        break;
                    case ( rtt_cdi::SCATTERING ) :
                        ipcressDataTypeKey = "rsmg";
                        dataDescriptor = "Multigroup Rosseland Scattering";
                        break;
                    default :
                        Assert(false);
                        break;
                }
                break;
			
            case ( rtt_cdi::PLANCK ) :
			
                switch ( opacityReaction ) {
                    case ( rtt_cdi::TOTAL ) :
                        // *** NOTE: THIS KEY DOES NOT ACTUALLY EVER EXIST *** //
                        // See LA-UR-01-5543
                        ipcressDataTypeKey = "ptmg";
                        dataDescriptor = "Multigroup Planck Total";
                        break;
                    case ( rtt_cdi::ABSORPTION ) :
                        ipcressDataTypeKey = "pmg";
                        dataDescriptor = "Multigroup Planck Absorption";
                        break;
                    case ( rtt_cdi::SCATTERING ) :
                        // *** NOTE: THIS KEY DOES NOT ACTUALLY EVER EXIST *** //
                        // See LA-UR-01-5543
                        ipcressDataTypeKey = "psmg";
                        dataDescriptor = "Multigroup Planck Scattering";
                        break;
                    default :
                        Assert(false);
                        break;
                }
                break;
			
            default :
                Assert(false);
                break;
        }
    }

    // Verify that the requested opacity type is available in
    // the IPCRESS file.
    Insist( key_available( ipcressDataTypeKey, vKnownKeys ),
            "requested opacity type is not available in the IPCRESS file.");

}

/*!
 * \brief Load the temperature, density, energy boundary and
 *     opacity opacity tables from the IPCRESS file.  Convert all
 *     tables (except energy boundaries) to log values.
 */
void IpcressDataTable::setIpcressDataTableSizes() const
{
    temperatures = spIpcressFile->getData( matID, "tgrid" );
    densities = spIpcressFile->getData( matID, "rgrid" );
    groupBoundaries  = spIpcressFile->getData( matID, "hnugrid" );
    
    numTemperatures = temperatures.size();
    numDensities = densities.size();
    numGroupBoundaries = groupBoundaries.size();
}

/*!
 * \brief Load the temperature, density, energy boundary and
 *     opacity opacity tables from the IPCRESS file.  Convert all
 *     tables (except energy boundaries) to log values.
 */
void IpcressDataTable::loadDataTable()
{
    // The interpolation routines expect everything to be in
    // log form so we only store the logorithmic temperature,
    // density and opacity data.
    logTemperatures.resize( temperatures.size() );
    std::transform( temperatures.begin(), temperatures.end(),
                    logTemperatures.begin(), unary_log );
    logDensities.resize( densities.size() );
    std::transform( densities.begin(), densities.end(), 
                    logDensities.begin(), unary_log );

    std::vector< double > opacities =
        spIpcressFile->getData( matID, ipcressDataTypeKey );
    logOpacities.resize( opacities.size() );
    std::transform( opacities.begin(), opacities.end(),
                    logOpacities.begin(), unary_log );
}

/*! 
 * \brief This function returns "true" if "key" is found in the list
 *        of "keys".  This is a static member function.
 */
template < typename T >
bool IpcressDataTable::key_available( T const              & key, 
                                      std::vector<T> const & keys ) const
{
    // Loop over all available keys.  If the requested key matches one in the
    // list return true.  If we reach the end of the list without a match
    // return false.
    for ( size_t i=0; i<keys.size(); ++i )
        if ( key == keys[i] ) return true;
    return false;	    
} // end of IpcressDataTable::key_available( string, vector<string> )


//---------------------------------------------------------------------------//
/*! 
 * \brief 
 * 
 * \param name description
 * \return description
 *
 * Note: the opacity array is a 1D array.  group id is the fastest moving
 * index and temperatures are the slowest moving index.
 */
double IpcressDataTable::interpOpac( double const targetTemperature,
                                     double const targetDensity,
                                     size_t const group ) const
{
    double logT   = std::log(targetTemperature);
    double logrho = std::log(targetDensity);

    size_t const numrho = logDensities.size();
    size_t const numT   = logTemperatures.size();
    // size_t const numpergroup = numrho*numT;
    size_t const ng = opacityEnergyDescriptor == std::string("gray") ?
                      1 :
                      numGroupBoundaries-1;
    
    // Check if we are off the table boundaries.  We don't allow
    // extrapolation, so move the target temperature or density to the table
    // boundary.
    if( targetTemperature < temperatures[0] )
        logT = std::log(temperatures[0]);
    if( targetTemperature > temperatures[numT-1] )
        logT = std::log(temperatures[numT-1]);
    if( targetDensity < densities[0] )
        logrho = std::log(densities[0]);
    if( targetDensity > densities[numrho-1] )
        logrho = std::log(densities[numrho-1]);

    /*
     * The grid looks like this:
     *
     *      |   T1     |   T      |   T2
     * -----------------------------------------
     * rho1 |   sig11  |          |   sig13
     * -----------------------------------------
     * rho  |   sig21  |  sig22   |   sig23
     * -----------------------------------------
     * rho2 |   sig31  |          |   sig33
     *
     * rho1, rho2, T1 and T2 are table values.
     * sig11, sig13, sig31 and sig33 are table values.
     *
     * Use linear interploation wrt log(rho) to find sig21 and sig23, then use
     * linear interpolation wrt log(T) to find sig22.
     */
    
    // Find the bracketing table values (T1, T2) and (rho1, rho2) for rho and
    // T.
    size_t irho = logDensities.size()-1;
    size_t iT   = logTemperatures.size()-1;
    for( size_t i=0; i<numT-1; ++i )
    {
        if( logT >= logTemperatures[i] && logT < logTemperatures[i+1] )
        {
            iT = i;
            break;
        }
    }
    for( size_t i=0; i<numrho-1; ++i )
    {
        if( logrho >= logDensities[i] && logrho < logDensities[i+1] )
        {
            irho = i;
            break;
        }
    }

    // Perform the linear interpolation.

    // index of cell with lower T and lower rho bound
    size_t i = (iT*numrho+irho)*ng+group;
    // size_t j = i + ng; // index for cell with higher rho value.
    size_t k = i + ng*numrho; // index for cell with higher T value
               
    double logsig12 = logOpacities[i]
                      + ( logrho - logDensities[irho] )
                      / ( logDensities[irho+1] - logDensities[irho] )
                      * ( logOpacities[i+ng] - logOpacities[i] );

    double logsig32 = logOpacities[k]
                      + ( logrho - logDensities[irho] )
                      / ( logDensities[irho+1] - logDensities[irho] )
                      * ( logOpacities[k+ng] - logOpacities[k] );

    double logsig22 = logsig12
                      + (logT-logTemperatures[iT])
                      / (logTemperatures[iT+1]-logTemperatures[iT])
                      * (logsig32-logsig12);
    
    // double logsig12 = logOpacities[group*numpergroup+iT*numrho+irho]
    //                   + (logrho-logDensities[irho])
    //                   / (logDensities[irho+1]-logDensities[irho])
    //                   * (logOpacities[group*numpergroup+iT*numrho+irho+1]
    //                      - logOpacities[group*numpergroup+iT*numrho+irho]);
    
    // double logsig32 = logOpacities[group*numpergroup+(iT+1)*numrho+irho]
    //                   + (logrho-logDensities[irho])
    //                   / (logDensities[irho+1]-logDensities[irho])
    //                   * (logOpacities[group*numpergroup+(iT+1)*numrho+irho+1]
    //                      - logOpacities[group*numpergroup+(iT+1)*numrho+irho]);
    
    // double logsig22 = logsig12
    //                   + (logT-logTemperatures[iT])
    //                   / (logTemperatures[iT+1]-logTemperatures[iT])
    //                   * (logsig32-logsig12);
    
    return std::exp(logsig22);  
}



} // end namespace rtt_cdi_ipcress

//---------------------------------------------------------------------------//
// end of IpcressDataTable.cc
//---------------------------------------------------------------------------//