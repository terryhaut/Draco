//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi_eospac/QueryEospac.cc
 * \author Kelly Thompson
 * \date   Friday, Nov 09, 2012, 13:02 pm
 * \brief  An interactive program for querying data from EOSPAC.
 * \note   Copyright (C) 2012 Los Alamos National Security, LLC.
 *         All rights reserved.
 */
//---------------------------------------------------------------------------//
// $Id: tEospac.cc 6822 2012-10-12 22:56:00Z kellyt $
//---------------------------------------------------------------------------//

#include "Eospac.hh"
#include "SesameTables.hh"
#include "EospacException.hh"
#include "ds++/Release.hh"
#include "ds++/SP.hh"
#include "ds++/Assert.hh"

#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>

//---------------------------------------------------------------------------//
void printeosproplist()
{
    std::cout
        << "  Uic_DT  - Specific Ion Internal Energy (and heat capacity)\n"
        << "  Ktc_DT  - Electron thermal conductivity\n"
        << "  Ue_DT   - \n"
        << "  Zfc_DT  - \n"
        << std::endl;
    return;
}

//---------------------------------------------------------------------------//
void query_eospac()
{
    using std::endl;
    using std::cout;
    
    cout << "Starting QueryEospac...\n\n"
         << "Material tables are published at "
         << "http://xweb.lanl.gov/projects/data.\n" << endl;
    
    bool keepGoing(true);
    while( keepGoing )
    {
        // Table ID
        int tableID(0);
        cout << "What table id (0 or q to quit)? ";
        std::cin >> tableID;
        if( tableID == 0 ) break;

        // Property
        std::string eosprop("");
        cout << "What property (h for help)? ";
        std::cin >> eosprop;
        if( eosprop == std::string("h") )
        {
            printeosproplist();
            cout << "What property (h for help)? ";
            std::cin >> eosprop;
        }
        
        // Create a SesameTable
        rtt_cdi_eospac::SesameTables SesameTab;
        if( eosprop == std::string("Uic_DT") )
            SesameTab.Uic_DT( tableID );
        else if( eosprop == std::string("Ktc_DT") )
            SesameTab.Ktc_DT( tableID );
        else
        {
            cout << "Requested EOS property unknown.  "
                 << "Please select on of:" << endl;
            printeosproplist();
            continue;
        }
        
        // Generate EOS Table
        rtt_dsxx::SP< rtt_cdi_eospac::Eospac const > spEospac;
        spEospac = new rtt_cdi_eospac::Eospac( SesameTab );
                
        // Parameters
        double temp(0.0);
        double dens(0.0);
        cout << "Evaluate at\n"
                  << "  Temperature (keV): ";
        std::cin >> temp;
        cout << "  Density (g/cm^3): ";
        std::cin >> dens;

        // Result
        cout << "For table " << tableID 
            // << " (" << SesameTab.tableName[ SesameTab.returnTypes(tableID) ] << ")"
             << endl;
        if( eosprop == std::string("Uic_DT") )
        {
            cout << "  Specific Ion Internal Energy = "
                 << spEospac->getSpecificIonInternalEnergy(temp,dens)
                 << " kJ/g\n"
                 << "  Ion Heat Capacity            = "
                 << spEospac->getIonHeatCapacity(temp,dens)
                 << "kJ/g/keV" << endl;
        }
        else if( eosprop == std::string("Ktc_DT") )
        {
            cout << "  Electron thermal conductivity = "
                 << spEospac->getElectronThermalConductivity(temp,dens)
                 << " /s/cm." << endl;
        }
    }

    return;
}

//---------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    
    std::string filename;
    // Parse the arguments
    for (int arg = 1; arg < argc; arg++)
    {
	if (std::string(argv[arg]) == "--version")
	{
            std::cout << argv[0] << ": version " << rtt_dsxx::release() 
                      << std::endl;
	    return 0;
	}
	else if (std::string(argv[arg]) == "--help" ||
                 std::string(argv[arg]) == "-h")
	{
            std::cout << argv[0] << ": version " << rtt_dsxx::release()
                 << "\nUsage: QueryEospac\n" 
                 << "Follow the prompts to print equation-of-state data to the "
                 << "screen." << std::endl;
	    return 0;
	}
        // else
        // {
        //     filename = string(argv[arg]);
        // }
    }

    try
    {
	// >>> Run the application
        query_eospac();
    }
    catch (rtt_cdi_eospac::EospacException &err )
    {
        std::cout << "EospacException ERROR: While running " << argv[0] << ", "
                  << err.what() << std::endl;
        return 1;
    }
    catch (rtt_dsxx::assertion &err)
    {
        std::cout << "ERROR: While running " << argv[0] << ", "
                  << err.what() << std::endl;
        return 1;
    }
    catch( ... )
    {
        std::cout << "ERROR: While running " << argv[0] << ", " 
                  << "An unknown exception was thrown on processor "
                  << std::endl;
        return 1;
    }
    return 0;
}   

//---------------------------------------------------------------------------//
// end of QueryEospac.cc
//---------------------------------------------------------------------------//
