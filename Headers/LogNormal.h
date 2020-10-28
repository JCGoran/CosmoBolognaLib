/*******************************************************************
 *  Copyright (C) 2010 by Federico Marulli and Alfonso Veropalumbo  *
 *  federico.marulli3@unibo.it                                      *
 *                                                                  *
 *  This program is free software; you can redistribute it and/or   *
 *  modify it under the terms of the GNU General Public License as  *
 *  published by the Free Software Foundation; either version 2 of  *
 *  the License, or (at your option) any later version.             *
 *                                                                  *
 *  This program is distributed in the hope that it will be useful, *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of  * 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   *
 *  GNU General Public License for more details.                    *
 *                                                                  *
 *  You should have received a copy of the GNU General Public       *
 *  License along with this program; if not, write to the Free      *
 *  Software Foundation, Inc.,                                      *
 *  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.       *
 *******************************************************************/

/**
 *  @file Headers/LogNormal.h
 *
 *  @brief Implementation of the lognormal data structure
 *
 *  This file defines the interface of the class LogNormal
 *
 *  @authors Federico Marulli, Alfonso Veropalumbo
 *
 *  @authors federico.marulli3@unibo.it, alfonso.veropalumbo@unibo.it
 */

#ifndef __LOGNORMAL__
#define __LOGNORMAL__

#include "Catalogue.h"
#include <fftw3.h>

namespace cbl {
  
  /**
   *  @brief The namespace of the functions and classes used to
   *  construct <B> log-normal mocks </B> 
   *  
   *  The \e lognormal namespace contains all the functions and
   *  classes used to construct log-normal mocks
   */
  namespace lognormal {
    
    /**
     *  @class LogNormal LogNormal.h "Headers/LogNormal.h"
     *
     *  @brief The class LogNormal
     *
     *  This class is used to handle objects of type <EM> lognormal
     *  </EM>
     */
    class LogNormal {
    
    protected:

      /// the number of lognormal realization to produce/read
      int m_nLN;
    
      /// pointer to the input data 
      std::shared_ptr<catalogue::Catalogue> m_data;
    
      /// pointer to the random catalogues
      std::shared_ptr<catalogue::Catalogue> m_random;
    
      /// cector containing pointers to the LogNormal realizations
      std::vector<std::shared_ptr<catalogue::Catalogue> > m_LNCat;  

      /// 0 &rarr; the input is a cosmology; 1 &rarr; the input is &xi;(r)
      bool m_withxi;

      /// vector containing the binnend separations of the two-point corrleation function used to create the density field
      std::vector<double> m_rmodel;
    
      /// vector containing the starting two-point correlation function used to create the density field
      std::vector<double> m_ximodel;

      /// approximate cell size of the density field
      double m_rmin;
    
      /// bias of the lognormal density field to be realized
      double m_bias;
    
      /// pointer to the fiducial cosmology
      std::shared_ptr<cosmology::Cosmology> m_cosmology;
    
      /// 0 &rarr; redshift-space (only monopole distortion); 1 &rarr; real-space
      bool m_Real;
    
      /// std::string containing the author for the model power spectrum
      std::string m_author;
    
      /// 0 &rarr; compute the linear power spectrum; 1 &rarr; compute the non-linear power spectrum
      bool m_NL;

      /// the cosmological model used to compute distances
      std::string m_model;

    
    public:

      /**
       *  @brief default constructor
       *  
       */
      LogNormal() {};

      /**
       *  @brief default destructor
       *  
       */
      ~LogNormal() {};

      /**
       *  @brief constructor 
       *  @param data input data catalogue
       *
       *  @param random input random catalogue (should be much larger
       *  than the random catalogue used to measure &xi;(r))
       *
       *  @param nLN number of lognormal realizations
       *  
       */
      LogNormal (const std::shared_ptr<catalogue::Catalogue> data, const std::shared_ptr<catalogue::Catalogue> random, const int nLN) : m_nLN(nLN), m_data(data), m_random(random)
      {
	m_LNCat.resize(m_nLN);
      }

      /**
       *  @brief set data and random catalogues
       *  @param data input data catalogue
       *  @param random input random catalogue
       *  
       */
      void setCatalogues (const std::shared_ptr<catalogue::Catalogue> data, const std::shared_ptr<catalogue::Catalogue> random);
    
      /**
       *  @brief set the starting two-point correlation function
       *  @param rr binned comoving separation 
       *  @param xi input two-point correlation function
       *  
       */
      void setParameters_from_xi (const std::vector<double> rr, const std::vector<double> xi);

      /**
       *  @brief set the parameters to compute a prediction of &xi;(r)
       *  @param cosmology the input cosmology
       *  @param bias the bias parameter
       *  @param Real 0 &rarr; redshift-space (only monopole distortion); 1 &rarr; real-space
       *  @param author the method used to compute dark matter power spectrum
       *  @param NL 0 &rarr; compute the linear power spectrum; 1 &rarr; compute the non-linear power spectrum
       *  @param model the cosmological model used to compute distances
       *  
       */
      void setParameters_from_model (const std::shared_ptr<cosmology::Cosmology> cosmology, const double bias, const bool Real=1, const std::string author="CAMB", const bool NL=0, const std::string model="LCDM");
    
      /**
       *  @brief set the total number of realizations
       *  @param nLN the number of realizations
       *  
       */
      void set_nLN (const int nLN);
    
      /**
       *  @brief get the private member LogNormal::m_nLN
       *  @return the number of LogNormal realizations
       */
      int nLN () { return m_nLN; }

      /**
       *  @brief get the private member LogNormal::m_LNCat[i]
       *  @param i index of the LogNormal realization
       *  @return the i-th LogNormal realization
       */
      std::shared_ptr<catalogue::Catalogue> LNCat (const int i) { return m_LNCat[i]; }

      /**
       *  @brief get the \f$\xi(r)\f$ model used in lognormal mock
       *  creation
       *
       *  @param radius the scale to compute the model
       *
       *  @return the \f$\xi(r)\f$ model used in lognormal mock
       *  creation
       */
      std::vector<double> get_xi_model(const std::vector<double> radius);
    
      /**
       *  @brief generate the LogNormal mock catalogues
       *
       *  @param rmin the cell size in comoving coordinates
       *  @param dir the output directory
       *      
       *  @param start the starting index of the mock to be created
       * 
       *  @param filename the prefix of the ouput file containing the
       *  LogNormal realizations
       *
       *  @param seed the seed for random number generation
       *
       *  
       */
      void generate_LogNormal_mock (const double rmin, const std::string dir, const int start=0, const std::string filename="lognormal_", const int seed=3213);
    
    };

  }

}

#endif
