/********************************************************************
 *  Copyright (C) 2021 by Giorgio Lesci and Federico Marulli        *
 *  giorgio.lesci2@unibo.it, federico.marulli3@unibo.it             *
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
 *  Modelling/NumberCounts/ModelFunction_NumberCounts1D_MassProxy.cpp
 *
 *  @brief Functions to model the mass number counts as a function of a mass proxy
 *
 *  This file contains the implementation of the functions used to
 *  model mass number counts as a function of mass proxy
 *
 *  @authors Giorgio Lesci, Federico Marulli
 *
 *  @authors giorgio.lesci2@unibo.it, federico.marulli3@unibo.it
 */


#include "ModelFunction_NumberCounts1D_MassProxy.h"

using namespace std;

using namespace cbl;


// ===========================================================================================


std::vector<double> cbl::modelling::numbercounts::number_counts_proxy (const std::vector<double> proxy, const std::shared_ptr<void> inputs, std::vector<double> &parameter)
{
  (void)proxy;
  
  // structure contaning the required input data
  shared_ptr<STR_NC_data_model> pp = static_pointer_cast<STR_NC_data_model>(inputs);

  // redefine the cosmology
  cbl::cosmology::Cosmology cosmo = *pp->cosmology;

  // redefine the cluster object
  cbl::catalogue::Cluster cluster = *pp->cluster;

  // set the cosmological parameters
  for (size_t i=0; i<pp->Cpar.size(); ++i)
    cosmo.set_parameter(pp->Cpar[i], parameter[i]);

  // set the cluster parameters
  cluster.set_alpha_scaling_rel(parameter[pp->Cpar.size()]);
  cluster.set_beta_scaling_rel(parameter[pp->Cpar.size()+1]);
  cluster.set_gamma_scaling_rel(parameter[pp->Cpar.size()+2]);
  cluster.set_scatter0_scaling_rel(parameter[pp->Cpar.size()+3]);
  cluster.set_scatterM_scaling_rel(parameter[pp->Cpar.size()+4]);
  cluster.set_scatterM_exponent_scaling_rel(parameter[pp->Cpar.size()+5]);
  cluster.set_scatterz_scaling_rel(parameter[pp->Cpar.size()+6]);
  cluster.set_scatterz_exponent_scaling_rel(parameter[pp->Cpar.size()+7]);
  cluster.set_zbias(parameter[pp->Cpar.size()+8]);
  cluster.set_proxybias(parameter[pp->Cpar.size()+9]);
  cluster.set_zerror(parameter[pp->Cpar.size()+10]);
  cluster.set_proxyerror(parameter[pp->Cpar.size()+11]);  
  cluster.set_Plambda_a(parameter[pp->Cpar.size()+12]);
  cluster.set_Plambda_b(parameter[pp->Cpar.size()+13]);
  cluster.set_Plambda_c(parameter[pp->Cpar.size()+14]);

  // compute the power spectrum
  std::vector<double> Pk = cosmo.Pk_DM(pp->kk, pp->method_Pk, false, 0., pp->store_output, pp->output_root, pp->norm, pp->k_min, pp->k_max, pp->prec, pp->file_par, true);

  const std::vector<cbl::glob::FuncGrid> interp = cbl::modelling::numbercounts::sigmaM_dlnsigmaM (pp->Mass_vector, cosmo, pp->kk, Pk, "Spline", pp->k_max);

  // Compute the counts
  std::vector<double> number_counts(pp->edges_x.size()-1);

  for (size_t j=0; j<pp->edges_x.size()-1; j++) {
    number_counts[j] = cbl::modelling::numbercounts::counts_proxy(pp->fz, pp->z_error, pp->proxy_error, pp->transfer_func, pp->z_min, pp->z_max, pp->edges_x[j], pp->edges_x[j+1], cosmo, cluster, pp->area_rad, pp->model_MF, pp->model_bias, pp->store_output, pp->Delta, pp->isDelta_Vir, interp[0], interp[1], pp->proxy_pivot, pp->z_pivot, pp->mass_pivot, pp->log_base, pp->SF_weights[j]);
  }

  return number_counts;
}



// ===========================================================================================


std::vector<double> cbl::modelling::numbercounts::number_counts_proxy_classic (const std::vector<double> proxy, const std::shared_ptr<void> inputs, std::vector<double> &parameter)
{
  (void)proxy;
  
  // structure contaning the required input data
  shared_ptr<STR_NC_data_model> pp = static_pointer_cast<STR_NC_data_model>(inputs);

  // redefine the cosmology
  cbl::cosmology::Cosmology cosmo = *pp->cosmology;

  // redefine the cluster object
  cbl::catalogue::Cluster cluster = *pp->cluster;

  // set the cosmological parameters
  for (size_t i=0; i<pp->Cpar.size(); ++i)
    cosmo.set_parameter(pp->Cpar[i], parameter[i]);

  // set the cluster parameters
  cluster.set_alpha_scaling_rel(parameter[pp->Cpar.size()]);
  cluster.set_beta_scaling_rel(parameter[pp->Cpar.size()+1]);
  cluster.set_gamma_scaling_rel(parameter[pp->Cpar.size()+2]);
  cluster.set_scatter0_scaling_rel(parameter[pp->Cpar.size()+3]);
  cluster.set_scatterM_scaling_rel(parameter[pp->Cpar.size()+4]);
  cluster.set_scatterM_exponent_scaling_rel(parameter[pp->Cpar.size()+5]);
  cluster.set_scatterz_scaling_rel(parameter[pp->Cpar.size()+6]);
  cluster.set_scatterz_exponent_scaling_rel(parameter[pp->Cpar.size()+7]);
  cluster.set_zbias(parameter[pp->Cpar.size()+8]);
  cluster.set_proxybias(parameter[pp->Cpar.size()+9]);
  cluster.set_zerror(parameter[pp->Cpar.size()+10]);
  cluster.set_proxyerror(parameter[pp->Cpar.size()+11]);

  // compute the power spectrum
  std::vector<double> Pk = cosmo.Pk_DM(pp->kk, pp->method_Pk, false, 0., pp->store_output, pp->output_root, pp->norm, pp->k_min, pp->k_max, pp->prec, pp->file_par, true);

  const std::vector<cbl::glob::FuncGrid> interp = cbl::modelling::numbercounts::sigmaM_dlnsigmaM (pp->Mass_vector, cosmo, pp->kk, Pk, "Spline", pp->k_max);

  // Compute the counts
  std::vector<double> number_counts(pp->edges_x.size()-1);

  for (size_t j=0; j<pp->edges_x.size()-1; j++) {
    number_counts[j] = cbl::modelling::numbercounts::counts_proxy_classic(pp->fz, pp->z_error, pp->proxy_error, pp->transfer_func, pp->z_min, pp->z_max, pp->edges_x[j], pp->edges_x[j+1], cosmo, cluster, pp->area_rad, pp->model_MF, pp->model_bias, pp->store_output, pp->Delta, pp->isDelta_Vir, interp[0], interp[1], pp->proxy_pivot, pp->z_pivot, pp->mass_pivot, pp->log_base, pp->SF_weights[j]);
  }

  return number_counts;
}


// ===========================================================================================


double cbl::modelling::numbercounts::fz_Ez (const std::vector<double> x, const std::shared_ptr<void> cosmo)
{
  cbl::cosmology::Cosmology cosmology = *std::static_pointer_cast<cbl::cosmology::Cosmology>(cosmo);
  return cosmology.HH(x[0])/cosmology.HH(x[1]);
}


// ===========================================================================================


double cbl::modelling::numbercounts::fz_direct (const std::vector<double> x, const std::shared_ptr<void> cosmo)
{
  (void)cosmo;
  return (1+x[0])/(1+x[1]);
}


// ===========================================================================================


double cbl::modelling::numbercounts::no_transfer (const double Mass, const double Sigma, const double redshift, const std::string model_bias, const double Delta, const std::string method_SS, std::shared_ptr<void> cosmo)
{
  (void)Mass; (void)Sigma; (void)redshift; (void)model_bias; (void)Delta; (void)method_SS; (void)cosmo;
  return 1.;
}


// ===========================================================================================


double cbl::modelling::numbercounts::bias_transfer (const double Mass, const double Sigma, const double redshift, const std::string model_bias, const double Delta, const std::string method_SS, std::shared_ptr<void> cosmo)
{
  cbl::cosmology::Cosmology cosmology = *std::static_pointer_cast<cbl::cosmology::Cosmology>(cosmo);
  return cosmology.bias_halo(Mass, Sigma, redshift, model_bias, true, "test", "Linear", Delta, -1, -1, 1.e-4, 100, 1.e-2, method_SS, cbl::par::defaultString, true);
}


// ===========================================================================================


double cbl::modelling::numbercounts::return_absolute_error (const std::vector<double> x)
{
  return x[0];
}


// ===========================================================================================


double cbl::modelling::numbercounts::absolute_from_relative_error (const std::vector<double> x)
{
  return x[0]*x[1];
}
