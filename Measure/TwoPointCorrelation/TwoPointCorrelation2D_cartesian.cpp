/********************************************************************
 *  Copyright (C) 2015 by Federico Marulli and Alfonso Veropalumbo  *
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
 ********************************************************************/

/**
 *  @file
 *  CatalogueAnalysis/TwoPointCorrelation/TwoPointCorrelation2D_cartesian.cpp
 *
 *  @brief Methods of the class TwoPointCorrelation2D_cartesian used
 *  to measure the monopole of the two-point correlation function
 *
 *  This file contains the implementation of the methods of the class
 *  TwoPointCorrelation2D_cartesian used to measure the monopole of
 *  the two-point correlation function
 *
 *  @authors Federico Marulli, Alfonso Veropalumbo
 *
 *  @authors federico.marulli3@unibo.it, alfonso.veropalumbo@unibo.it
 */


#include "TwoPointCorrelation2D_cartesian.h"

using namespace std;

using namespace cbl;
using namespace catalogue;
using namespace chainmesh;
using namespace data;
using namespace pairs;
using namespace measure;
using namespace twopt;


// ============================================================================================


void cbl::measure::twopt::TwoPointCorrelation2D_cartesian::set_parameters (const BinType binType_rp, const double rpMin, const double rpMax, const int nbins_rp, const double shift_rp, const BinType binType_pi, const double piMin, const double piMax, const int nbins_pi, const double shift_pi, const CoordinateUnits angularUnits, std::function<double(double)> angularWeight, const bool compute_extra_info) 
{
  if (!compute_extra_info) {
    if (binType_rp==BinType::_logarithmic_) {
      if (binType_pi==BinType::_logarithmic_) {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_loglog_, PairInfo::_standard_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits, angularWeight));
      }
      else {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_loglin_, PairInfo::_standard_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits, angularWeight));
      }
    }
    else {
      if (binType_pi==BinType::_logarithmic_) {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_linlog_, PairInfo::_standard_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits, angularWeight));
      }
      else {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_linlin_, PairInfo::_standard_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits, angularWeight));
      }
    }
  }
  else {
    if (binType_rp==BinType::_logarithmic_) {
      if (binType_pi==BinType::_logarithmic_) {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_loglog_, PairInfo::_extra_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits, angularWeight));
      }
      else {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_loglin_, PairInfo::_extra_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits, angularWeight));
      }
    }
    else {
      if (binType_pi==BinType::_logarithmic_) {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_linlog_, PairInfo::_extra_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits, angularWeight));
      }
      else {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_linlin_, PairInfo::_extra_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits, angularWeight));
      }
    }
  }
  
  if (binType_rp==BinType::_logarithmic_) {
    if (binType_pi==BinType::_logarithmic_) {
      m_rr = move(Pair::Create(PairType::_comovingCartesian_loglog_, PairInfo::_standard_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits));
      m_dr = move(Pair::Create(PairType::_comovingCartesian_loglog_, PairInfo::_standard_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits));
    }
    else {
      m_rr = move(Pair::Create(PairType::_comovingCartesian_loglin_, PairInfo::_standard_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits));
      m_dr = move(Pair::Create(PairType::_comovingCartesian_loglin_, PairInfo::_standard_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits));
    }
  }
  else {
    if (binType_pi==BinType::_logarithmic_) {
      m_rr = move(Pair::Create(PairType::_comovingCartesian_linlog_, PairInfo::_standard_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits));
      m_dr = move(Pair::Create(PairType::_comovingCartesian_linlog_, PairInfo::_standard_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits));
    }
    else {
      m_rr = move(Pair::Create(PairType::_comovingCartesian_linlin_, PairInfo::_standard_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits));
      m_dr = move(Pair::Create(PairType::_comovingCartesian_linlin_, PairInfo::_standard_, rpMin, rpMax, nbins_rp, shift_rp, piMin, piMax, nbins_pi, shift_pi, angularUnits));
    }
  }
}


// ============================================================================================


void cbl::measure::twopt::TwoPointCorrelation2D_cartesian::set_parameters (const BinType binType_rp, const double rpMin, const double rpMax, const double binSize_rp, const double shift_rp, const BinType binType_pi, const double piMin, const double piMax, const double binSize_pi, const double shift_pi, const CoordinateUnits angularUnits, std::function<double(double)> angularWeight, const bool compute_extra_info)
{
  if (!compute_extra_info) {
    if (binType_rp==BinType::_logarithmic_) {
      if (binType_pi==BinType::_logarithmic_) {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_loglog_, PairInfo::_standard_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits, angularWeight));
      }
      else {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_loglin_, PairInfo::_standard_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits, angularWeight));
      }
    }
    else {
      if (binType_pi==BinType::_logarithmic_) {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_linlog_, PairInfo::_standard_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits, angularWeight));
      }
      else {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_linlin_, PairInfo::_standard_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits, angularWeight));
      }
    }
  }
  else {
    if (binType_rp==BinType::_logarithmic_) {
      if (binType_pi==BinType::_logarithmic_) {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_loglog_, PairInfo::_extra_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits, angularWeight));
      }
      else {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_loglin_, PairInfo::_extra_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits, angularWeight));
      }
    }
    else {
      if (binType_pi==BinType::_logarithmic_) {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_linlog_, PairInfo::_extra_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits, angularWeight));
      }
      else {
	m_dd = move(Pair::Create(PairType::_comovingCartesian_linlin_, PairInfo::_extra_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits, angularWeight));
      }
    }
  }


  if (binType_rp==BinType::_logarithmic_) {
    if (binType_pi==BinType::_logarithmic_) {
      m_rr = move(Pair::Create(PairType::_comovingCartesian_loglog_, PairInfo::_standard_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits));
      m_dr = move(Pair::Create(PairType::_comovingCartesian_loglog_, PairInfo::_standard_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits));
    }
    else {
      m_rr = move(Pair::Create(PairType::_comovingCartesian_loglin_, PairInfo::_standard_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits));
      m_dr = move(Pair::Create(PairType::_comovingCartesian_loglin_, PairInfo::_standard_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits));
    }
  }
  else {
    if (binType_pi==BinType::_logarithmic_) {
      m_rr = move(Pair::Create(PairType::_comovingCartesian_linlog_, PairInfo::_standard_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits));
      m_dr = move(Pair::Create(PairType::_comovingCartesian_linlog_, PairInfo::_standard_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits));
    }
    else {
      m_rr = move(Pair::Create(PairType::_comovingCartesian_linlin_, PairInfo::_standard_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits));
      m_dr = move(Pair::Create(PairType::_comovingCartesian_linlin_, PairInfo::_standard_, rpMin, rpMax, binSize_rp, shift_rp, piMin, piMax, binSize_pi, shift_pi, angularUnits));
    }
  }
}


// ============================================================================================


void cbl::measure::twopt::TwoPointCorrelation2D_cartesian::read (const std::string dir, const std::string file) 
{
  m_dataset->read(dir+file);
}


// ============================================================================================


void cbl::measure::twopt::TwoPointCorrelation2D_cartesian::write (const std::string dir, const std::string file, const bool full, const int rank) const 
{
  vector<double> xx = m_dataset->xx(), yy = m_dataset->yy();
  
  checkDim(xx, m_dd->nbins_D1(), "rp"); checkDim(yy, m_dd->nbins_D2(), "pi");
  
  string header = "[1] perpendicular separation at the bin centre # [2] parallel separation at the bin centre # [3] 2D two-point correlation function # [4] error";
  if (m_compute_extra_info) header += " # [5] mean perpendicular separation # [6] standard deviation of the distribution of perpendicular separations # [7] mean parallel separation # [8] standard deviation of the distribution of parallel separations # [9] mean redshift # [10] standard deviation of the redshift distribution";
  
  m_dataset->write(dir, file, header, full, 5, rank);
}


// ============================================================================================


void cbl::measure::twopt::TwoPointCorrelation2D_cartesian::measure (const ErrorType errorType, const std::string dir_output_pairs, const std::vector<std::string> dir_input_pairs, const std::string dir_output_resample, const int nMocks, const bool count_dd, const bool count_rr, const bool count_dr, const bool tcount, const Estimator estimator, const double fact, const int seed)
{
  switch (errorType) {
  case (ErrorType::_Poisson_) :
    measurePoisson(dir_output_pairs, dir_input_pairs, count_dd, count_rr, count_dr, tcount, estimator, fact);
    break;
  case (ErrorType::_Jackknife_) :
    measureJackknife(dir_output_pairs, dir_input_pairs, dir_output_resample, count_dd, count_rr, count_dr, tcount, estimator, fact);
    break;
  case (ErrorType::_Bootstrap_) :
    measureBootstrap(nMocks, dir_output_pairs, dir_input_pairs, dir_output_resample, count_dd, count_rr, count_dr, tcount, estimator, fact, seed);
    break;
  default:
    ErrorCBL("unknown type of error!", "measure", "TwoPointCorrelation2D_cartesian.cpp");
  }
}


// ============================================================================================


void cbl::measure::twopt::TwoPointCorrelation2D_cartesian::measurePoisson (const std::string dir_output_pairs, const std::vector<std::string> dir_input_pairs, const bool count_dd, const bool count_rr, const bool count_dr, const bool tcount, const Estimator estimator, const double fact)
{
  // ----------- count the data-data, random-random and data-random pairs, or read them from file ----------- 
  
  count_allPairs(m_twoPType, dir_output_pairs, dir_input_pairs, count_dd, count_rr, count_dr, tcount, estimator, fact);
  
  
  // ----------- compute the monopole of the two-point correlation function ----------- 

  if (estimator==Estimator::_natural_)
    m_dataset = correlation_NaturalEstimator(m_dd, m_rr);
  else if (estimator==Estimator::_LandySzalay_) {
    m_dataset = correlation_LandySzalayEstimator(m_dd, m_rr, m_dr);
  }
  else
    ErrorCBL("the chosen estimator is not implemented!", "measurePoisson", "TwoPointCorrelation2D_cartesian.cpp");
  
}


// ============================================================================================


void cbl::measure::twopt::TwoPointCorrelation2D_cartesian::measureJackknife (const std::string dir_output_pairs, const std::vector<std::string> dir_input_pairs, const std::string dir_output_resample, const bool count_dd, const bool count_rr, const bool count_dr, const bool tcount, const Estimator estimator, const double fact)
{
  if (dir_output_resample!=par::defaultString && dir_output_resample!="") {
    string mkdir = "mkdir -p "+dir_output_resample;
    if (system(mkdir.c_str())) {}
  }
  
  vector<long> region_list = m_data->region_list();
  size_t nRegions = region_list.size();

  vector< vector<double > > xi_SubSamples, covariance;

  vector<shared_ptr<Pair> > dd_regions, rr_regions, dr_regions;
  count_allPairs_region(dd_regions, rr_regions, dr_regions, m_twoPType, dir_output_pairs, dir_input_pairs, count_dd, count_rr, count_dr, tcount, estimator, fact);

  vector<shared_ptr<Data> > data_SS = (estimator==Estimator::_natural_) ? XiJackknife(dd_regions, rr_regions) : XiJackknife(dd_regions, rr_regions, dr_regions);

  for (size_t i=0; i<nRegions; i++) {

    if (dir_output_resample!=par::defaultString && dir_output_resample!="") {
      string file = "xi_Jackknife_"+conv(i, par::fINT)+".dat";
      string header = "[1] perpendicular separation at the bin centre # [2] parallel separation at the bin centre # [3] 2D two-point correlation function # [4] error";
  if (m_compute_extra_info) header += " # [5] mean perpendicular separation # [6] standard deviation of the distribution of perpendicular separations # [7] mean parallel separation # [8] standard deviation of the distribution of parallel separations # [9] mean redshift # [10] standard deviation of the redshift distribution";
      data_SS[i]->write(dir_output_resample, file, header, false, 10, 0);
    }
    
    xi_SubSamples.push_back(data_SS[i]->data());
  }

  covariance_matrix(xi_SubSamples, covariance, 1);

  if (estimator==Estimator::_natural_)
    m_dataset = correlation_NaturalEstimator(m_dd, m_rr);
  else if (estimator==Estimator::_LandySzalay_)
    m_dataset = correlation_LandySzalayEstimator(m_dd, m_rr, m_dr);
  else
    ErrorCBL("the chosen estimator is not implemented!", "measureJackknife", "TwoPointCorrelation2D_cartesian.cpp");

  m_dataset->set_covariance(covariance);
}

// ============================================================================================


void cbl::measure::twopt::TwoPointCorrelation2D_cartesian::measureBootstrap (const int nMocks, const std::string dir_output_pairs, const std::vector<std::string> dir_input_pairs, const std::string dir_output_resample, const bool count_dd, const bool count_rr, const bool count_dr, const bool tcount, const Estimator estimator, const double fact, const int seed)
{
  if (nMocks<1)
    ErrorCBL("the number of mocks must be >0", "measureBootstrap", "TwoPointCorrelation2D_cartesian.cpp");

  if (dir_output_resample!=par::defaultString && dir_output_resample!="") {
    string mkdir = "mkdir -p "+dir_output_resample;
    if (system(mkdir.c_str())) {}
  }

  vector< vector<double > > xi_SubSamples, covariance;

  vector<shared_ptr<Pair> > dd_regions, rr_regions, dr_regions;
  count_allPairs_region(dd_regions, rr_regions, dr_regions, m_twoPType, dir_output_pairs, dir_input_pairs, count_dd, count_rr, count_dr, tcount, estimator, fact);

  vector<shared_ptr<Data> > data_SS = (estimator==Estimator::_natural_) ? XiBootstrap(nMocks, dd_regions, rr_regions, seed) : XiBootstrap(nMocks, dd_regions, rr_regions, dr_regions, seed);

  for (int i=0; i<nMocks; i++) {

    if (dir_output_resample!=par::defaultString && dir_output_resample!="") {
      string file = "xi_Bootstrap_"+conv(i, par::fINT)+".dat";
      string header = "[1] perpendicular separation at the bin centre # [2] parallel separation at the bin centre # [3] 2D two-point correlation function # [4] error";
  if (m_compute_extra_info) header += " # [5] mean perpendicular separation # [6] standard deviation of the distribution of perpendicular separations # [7] mean parallel separation # [8] standard deviation of the distribution of parallel separations # [9] mean redshift # [10] standard deviation of the redshift distribution";
      data_SS[i]->write(dir_output_resample, file, header, false, 10, 0);
    }
    
    xi_SubSamples.push_back(data_SS[i]->data());
  }

  covariance_matrix(xi_SubSamples, covariance, 1);

  if (estimator==Estimator::_natural_)
    m_dataset = correlation_NaturalEstimator(m_dd, m_rr);
  else if (estimator==Estimator::_LandySzalay_)
    m_dataset = correlation_LandySzalayEstimator(m_dd, m_rr, m_dr);
  else
    ErrorCBL("the chosen estimator is not implemented!", "measureBootstrap", "TwoPointCorrelation2D_cartesian.cpp");

  m_dataset->set_covariance(covariance);
}
