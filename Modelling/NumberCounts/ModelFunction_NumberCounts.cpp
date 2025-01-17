/********************************************************************
 *  Copyright (C) 2016 by Federico Marulli and Alfonso Veropalumbo  *
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
 *  Modelling/NumberCounts/ModelFunction_NumberCounts.cpp
 *
 *  @brief Functions to model the number counts
 *
 *  This file contains the implementation of the functions used to
 *  model the monopole of the number counts
 *
 *  @authors Federico Marulli, Alfonso Veropalumbo
 *
 *  @authors federico.marulli3@unibo.it, alfonso.veropalumbo@unibo.it
 */


#include "ModelFunction_NumberCounts.h"

using namespace std;

using namespace cbl;


// ===========================================================================================


double cbl::modelling::numbercounts::Filter_sigmaR (const double kk, const double radius)
{
  return pow(TopHat_WF(kk*radius),2);
}


// ===========================================================================================


double cbl::modelling::numbercounts::Filter_dsigmaR (const double kk, const double radius)
{
  return 2.*cbl::TopHat_WF(kk*radius)*cbl::TopHat_WF_D1(kk*radius)*kk;
}


// ===========================================================================================


void cbl::modelling::numbercounts::sigmaM_dlnsigmaM (double &sigmaM, double &dlnsigmaM, const double mass, const cbl::glob::FuncGrid interp_Pk, const double kmax, const double rho)
{
  double norm =  1./(2.*pow(par::pi, 2));
  double dRdM_fact = pow(3./(4.*par::pi*rho), 1./3.);

  double RR =  Radius(mass, rho);
  double dRdM =  dRdM_fact*pow(mass, -2./3.)/3.;

  auto integrand_sigmaR = [&] (const double kk)
  {
    return kk*kk*interp_Pk(kk)*Filter_sigmaR(kk, RR);
  };

  sigmaM = norm*cbl::wrapper::gsl::GSL_integrate_cquad(integrand_sigmaR, 1.e-4, kmax, 1.e-5);

  auto integrand_dsigmaR = [&] (const double kk)
  {
    return kk*kk*interp_Pk(kk)*Filter_dsigmaR(kk, RR);
  };

  dlnsigmaM = norm*cbl::wrapper::gsl::GSL_integrate_cquad(integrand_dsigmaR, 1.e-4, kmax, 1.e-5)*dRdM*(mass/(2*sigmaM));
  sigmaM = sqrt(sigmaM);
}


// ===========================================================================================


void cbl::modelling::numbercounts::sigmaM_dlnsigmaM (std::vector<double> &sigmaM, std::vector<double> &dlnsigmaM, const std::vector<double> mass, const std::vector<double> kk, const std::vector<double> Pk, const std::string interpType, const double kmax, const double rho)
{
  double norm =  1./(2.*pow(par::pi, 2));
  double dRdM_fact = pow(3./(4.*par::pi*rho), 1./3.);
  cbl::glob::FuncGrid Pk_interp(kk, Pk, interpType);
  sigmaM.resize(mass.size(), 0);
  dlnsigmaM.resize(mass.size(), 0);

  for (size_t i=0; i<mass.size(); i++) {

    double RR =  Radius(mass[i], rho);
    double dRdM =  dRdM_fact*pow(mass[i], -2./3.)/3.;

    auto integrand_sigmaR = [&] (const double kk)
    {
      return kk*kk*Pk_interp(kk)*Filter_sigmaR(kk, RR);
    };

    sigmaM[i] = norm*cbl::wrapper::gsl::GSL_integrate_cquad(integrand_sigmaR, 1.e-4, kmax, 1.e-5);

    auto integrand_dsigmaR = [&] (const double kk)
    {
      return kk*kk*Pk_interp(kk)*Filter_dsigmaR(kk, RR);
    };

    dlnsigmaM[i] = norm*cbl::wrapper::gsl::GSL_integrate_cquad(integrand_dsigmaR, 1.e-4, kmax, 1.e-5)*dRdM*(mass[i]/(2*sigmaM[i]));
    sigmaM[i] = sqrt(sigmaM[i]);
  }
}


// ===========================================================================================


std::vector<cbl::glob::FuncGrid> cbl::modelling::numbercounts::sigmaM_dlnsigmaM (const std::vector<double> mass, cbl::cosmology::Cosmology cosmology, const std::vector<double> kk, const std::vector<double> Pk, const std::string interpType, const double kmax)
{
  const double rho = cosmology.rho_m(0., true);

  vector<double> sigmaM, dlnsigmaM; 

  cbl::modelling::numbercounts::sigmaM_dlnsigmaM (sigmaM, dlnsigmaM, mass, kk, Pk, interpType, kmax, rho);

  vector<cbl::glob::FuncGrid> interp(2);
  interp[0] = cbl::glob::FuncGrid(mass, sigmaM, interpType);
  interp[1] = cbl::glob::FuncGrid(mass, dlnsigmaM, interpType);

  return interp;
}


// ===========================================================================================


double cbl::modelling::numbercounts::mass_function (const double mass, cbl::cosmology::Cosmology cosmology, const double redshift, const std::string model_MF, const bool store_output, const double Delta, const bool isDelta_vir, const cbl::glob::FuncGrid interp_Pk, const double kmax)
{
  const double rho = cosmology.rho_m(0., true);

  double sigmaM, dlnsigmaM;

  sigmaM_dlnsigmaM (sigmaM, dlnsigmaM, ((cosmology.unit()) ? mass : mass*cosmology.hh()), interp_Pk, kmax, rho);
  double _Delta = (isDelta_vir) ? cosmology.Delta_vir(Delta, redshift) : Delta;

  return cosmology.mass_function(mass, sigmaM, dlnsigmaM, redshift, model_MF, store_output, cbl::par::defaultString, _Delta);
}


// ===========================================================================================


std::vector<double> cbl::modelling::numbercounts::mass_function (const std::vector<double> mass, cbl::cosmology::Cosmology cosmology, const double redshift, const std::string model_MF, const bool store_output, const double Delta, const bool isDelta_vir, const std::vector<double> kk, const std::vector<double> Pk, const std::string interpType, const double kmax)
{
  vector<double> mass_function(mass.size());
  
  const double rho = cosmology.rho_m(0., true);

  vector<double> _mass = mass;

  if (!cosmology.unit())
    for(size_t i=0; i<mass.size(); i++)
      _mass[i] = mass[i]*cosmology.hh();

  vector<double> sigmaM, dlnsigmaM;
  cbl::modelling::numbercounts::sigmaM_dlnsigmaM (sigmaM, dlnsigmaM, _mass, kk, Pk, interpType, kmax, rho);

  double _Delta = (isDelta_vir) ? cosmology.Delta_vir(Delta, redshift) : Delta;

  for (size_t i=0; i<mass.size(); i++) 
    mass_function[i] = cosmology.mass_function(mass[i], sigmaM[i], dlnsigmaM[i], redshift, model_MF, store_output, cbl::par::defaultString, _Delta);

  return mass_function;
}


// ===========================================================================================


std::vector<std::vector<double>> cbl::modelling::numbercounts::mass_function (const std::vector<double> redshift, const std::vector<double> mass, cbl::cosmology::Cosmology cosmology, const std::string model_MF, const bool store_output, const double Delta, const bool isDelta_vir, const std::vector<double> kk, const std::vector<double> Pk, const std::string interpType, const double kmax)
{
  vector<vector<double>> mass_function(redshift.size(), vector<double>(mass.size()));
  
  const double rho = cosmology.rho_m(0., true);

  vector<double> _mass = mass;

  if (!cosmology.unit()) 
    for(size_t i=0; i<mass.size(); i++)
      _mass[i] = mass[i]*cosmology.hh();

  vector<double> sigmaM, dlnsigmaM;
  cbl::modelling::numbercounts::sigmaM_dlnsigmaM(sigmaM, dlnsigmaM, _mass, kk, Pk, interpType, kmax, rho);

  for (size_t j=0; j<redshift.size(); j++) 
    for (size_t i=0; i<mass.size(); i++) 
      mass_function[j][i] = cosmology.mass_function(mass[i], sigmaM[i], dlnsigmaM[i], redshift[j], model_MF, store_output, par::defaultString, ((isDelta_vir) ? cosmology.Delta_vir(Delta, redshift[j]) : Delta));

  return mass_function;
}


// ===========================================================================================


double cbl::modelling::numbercounts::number_counts (const double redshift_min, const double redshift_max, const double Mass_min, const double Mass_max, cbl::cosmology::Cosmology cosmology, const double Area, const std::string model_MF, const bool store_output, const double Delta, const bool isDelta_vir, const cbl::glob::FuncGrid interp_sigmaM, const  cbl::glob::FuncGrid interp_DlnsigmaM)
{
  double fact = (cosmology.unit()) ? 1 : cosmology.hh();
  double nc = 0;

  auto integrand = [&cosmology,&fact,&model_MF,&isDelta_vir,&Delta,&Area,&interp_sigmaM,&interp_DlnsigmaM,&store_output] (const std::vector<double> x)
		   {
		     double Mass = pow(10,x[0])*pow(10,14);
		     return cosmology.mass_function(Mass, interp_sigmaM(Mass*fact), interp_DlnsigmaM(Mass*fact), x[1], model_MF, store_output, cbl::par::defaultString, (isDelta_vir) ? cosmology.Delta_vir(Delta, x[1]) : Delta)*Area*cosmology.dV_dZdOmega(x[1], true) * pow(10,x[0]);
		   };
  std::vector<std::vector<double>> integration_limits(2);
  integration_limits[0] = {log10(Mass_min/pow(10,14)), log10(Mass_max/pow(10,14))};
  integration_limits[1] = {redshift_min, redshift_max};

  cbl::wrapper::cuba::CUBAwrapper CW(integrand, 2);
  nc = CW.IntegrateVegas(integration_limits, false);
  
  return nc * pow(10,14) * log(10);
}

/*
double cbl::modelling::numbercounts::number_counts (const double redshift_min, const double redshift_max, const double Mass_min, const double Mass_max, cbl::cosmology::Cosmology cosmology, const double Area, const std::string model_MF, const bool store_output, const double Delta, const bool isDelta_vir, const cbl::glob::FuncGrid interp_sigmaM, const  cbl::glob::FuncGrid interp_DlnsigmaM)
{
  double npt_redshift=100; double npt_mass=1;
  double fact = (cosmology.unit()) ? 1 : cosmology.hh();

  double deltaz = (redshift_max-redshift_min)/(npt_redshift);
  double deltaLogM = (log10(Mass_max)-log10(Mass_min))/(npt_mass);

  vector<double> MM(npt_mass), deltaM(npt_mass), sigmaM(npt_mass), dlnsigmaM(npt_mass);
  for (int j=0; j<npt_mass; j++) {
    double M1 = pow(10., log10(Mass_min)+j*deltaLogM);
    double M2 = pow(10., log10(Mass_min)+(j+1)*deltaLogM);
    MM[j] = pow(10., log10(Mass_min)+(j+0.5)*deltaLogM);
    deltaM[j] = M2-M1;
    sigmaM[j] = interp_sigmaM(MM[j]*fact);
    dlnsigmaM[j] = interp_DlnsigmaM(MM[j]*fact);
  }

  double nc = 0.;

  for (int i=0; i<npt_redshift; i++) {
    double zz = redshift_min+(i+0.5)*deltaz;
    double _Delta = (isDelta_vir) ? cosmology.Delta_vir(Delta, zz) : Delta;
    double dV_dZ = Area*cosmology.dV_dZdOmega(zz, true);

    double Int = 0;
    for (int j=0; j<npt_mass; j++) 
      Int += cosmology.mass_function(MM[j], sigmaM[j], dlnsigmaM[j], zz, model_MF, store_output, cbl::par::defaultString, _Delta)*deltaM[j]*dV_dZ;
    nc += Int*deltaz;

  }

  return nc;
}
*/
  /*
  (void)npt_redshift; (void)npt_mass;

  auto integrand_nc_z = [&] (const double redshift)
  {
    double _Delta = (isDelta_vir) ? cosmology.Delta_vir(Delta, redshift) : Delta;
    double dV_dZ = Area*cosmology.dV_dZdOmega(redshift, true);

    auto integrand_nc_m = [&] (const double mass)
    { 
      return cosmology.mass_function (mass, interp_sigmaM(mass), interp_DlnsigmaM(mass), redshift, model_MF, cbl::par::defaultString, _Delta)*dV_dZ;
    };

    return wrapper::gsl::GSL_integrate_qag(integrand_nc_m, Mass_min, Mass_max);
  };

  return wrapper::gsl::GSL_integrate_qag(integrand_nc_z, redshift_min, redshift_max);
  */


// ===========================================================================================


double cbl::modelling::numbercounts::counts_proxy (double (*fz)(std::vector<double>, std::shared_ptr<void>), double (*z_error)(std::vector<double>), double (*proxy_error)(std::vector<double>), double (*transfer_func)(const double, const double, const double, const std::string, const double, const std::string, std::shared_ptr<void>), const double redshift_min, const double redshift_max, const double proxy_min, const double proxy_max, cbl::cosmology::Cosmology cosmology, cbl::catalogue::Cluster cluster, const double Area, const std::string model_MF, const std::string model_bias, const bool store_output, const double Delta, const bool isDelta_vir, const cbl::glob::FuncGrid interp_sigmaM, const  cbl::glob::FuncGrid interp_DlnsigmaM, const double proxy_pivot, const double z_pivot, const double mass_pivot, const double log_base, const double weight)
{  
  double fact = (cosmology.unit()) ? 1 : cosmology.hh();
  std::shared_ptr<void> pp;
  auto cosmology_ptr = std::make_shared<cbl::cosmology::Cosmology>(cosmology);

  // Declare the normalized mass and the redshift, used as constants in integrand_P_M__z (which is called in integrand)
  double normM=0; double the_redsh=0;

  // P(M|z) integrand
  auto integrand_P_M__z = [&fz,&cosmology_ptr,&cosmology,&cluster,&log_base,&proxy_pivot,&z_pivot,&the_redsh,&normM,&pp] (const double x)
    {
      double log_lambda = log(x/proxy_pivot)/log(log_base);
      double log_f_z = log( fz({the_redsh, z_pivot}, cosmology_ptr) )/log(log_base);
      
      double mean = cluster.alpha_scaling_rel() + cluster.beta_scaling_rel()*log_lambda + cluster.gamma_scaling_rel()*log_f_z;
      double sigma = cluster.scatter0_scaling_rel() + cluster.scatterM_scaling_rel()*log_lambda + cluster.scatterz_scaling_rel()*log_f_z;
      double P_M__lambda_z = (cbl::gaussian(normM, pp, {mean,sigma}));      
      double P_lambda__z = cluster.Plambda_a() * pow(x,-cluster.Plambda_b()) * exp(-cluster.Plambda_c()*x);
      
      return P_M__lambda_z * P_lambda__z;
    };

  // Total integrand
  auto integrand = [&z_error,&proxy_error,&model_bias,&transfer_func,&fz,&cosmology_ptr,&cosmology,&cluster,&log_base,&proxy_pivot,&z_pivot,&mass_pivot,&proxy_min,&proxy_max,&redshift_min,&redshift_max,&integrand_P_M__z,&normM,&the_redsh,&interp_sigmaM,&interp_DlnsigmaM,&store_output,&fact,&model_MF,&Delta,&Area,&isDelta_vir,&pp] (const std::vector<double> x)
    {
      double Delta_ = (isDelta_vir) ? cosmology.Delta_vir(Delta, x[1]) : Delta;
      double Mass = pow(log_base,x[0])*mass_pivot;
      normM = x[0];
      the_redsh = x[1];

      // Compute P(M|lambda,z)
      double log_lambda = log(x[2]/proxy_pivot)/log(log_base);
      double log_f_z = log( fz({x[1], z_pivot}, cosmology_ptr) )/log(log_base);
      
      double mean = cluster.alpha_scaling_rel() + cluster.beta_scaling_rel()*log_lambda + cluster.gamma_scaling_rel()*log_f_z;
      double sigma = cluster.scatter0_scaling_rel() + cluster.scatterM_scaling_rel()*pow(log_lambda, cluster.scatterM_exponent_scaling_rel()) + cluster.scatterz_scaling_rel()*pow(log_f_z, cluster.scatterz_exponent_scaling_rel());
      double P_M__lambda_z = (cbl::gaussian(normM, pp, {mean,sigma}));

      // Compute P(lambda|z)
      double P_lambda__z = cluster.Plambda_a() * pow(x[2],-cluster.Plambda_b()) * exp(-cluster.Plambda_c()*x[2]);

      // Compute P(M|z)
      double P_M__z=0;
      if (P_M__lambda_z*P_lambda__z > 0){      
	P_M__z=cbl::wrapper::gsl::GSL_integrate_cquad(integrand_P_M__z,0.001,200);
      } else{
	P_M__z = 1;}

      // Compute the integrals of P(z|z) and P(lambda|lambda)
      double mean_Pz = x[1] + cluster.zbias() * (1+x[1]);
      double int_P_z = 0.5 * ( erf( (redshift_max - mean_Pz) / (sqrt(2)*z_error({cluster.zerror(), redshift_max})) ) - erf( (redshift_min - mean_Pz) / (sqrt(2)*z_error({cluster.zerror(), redshift_min})) ) );
      double int_P_lambda = 0.5 * ( erf( (proxy_max - x[2]) / (sqrt(2)*proxy_error({cluster.proxyerror(), proxy_max})) ) - erf( (proxy_min - x[2]) / (sqrt(2)*proxy_error({cluster.proxyerror(), proxy_min})) ) );
      
      return transfer_func(Mass, interp_sigmaM(Mass*fact), x[1], model_bias, Delta_, "EisensteinHu", cosmology_ptr) * cosmology.mass_function(Mass, interp_sigmaM(Mass*fact), interp_DlnsigmaM(Mass*fact), x[1], model_MF, store_output, cbl::par::defaultString, Delta_)*Area*cosmology.dV_dZdOmega(x[1], true) * pow(log_base,normM) * (P_M__lambda_z*P_lambda__z/P_M__z) * int_P_z * int_P_lambda;
    };
  
  // -------------------------------------------------------------

  // Find the minimum and maximum masses, given the parameters of the scaling relation
  double log_lambda_min = log(proxy_min/proxy_pivot)/log(log_base);
  double log_lambda_max = log(proxy_max/proxy_pivot)/log(log_base);
  double log_f_z_min = log( fz({redshift_min, z_pivot}, cosmology_ptr) )/log(log_base);
  double log_f_z_max = log( fz({redshift_max, z_pivot}, cosmology_ptr) )/log(log_base);

  double M1 = cluster.alpha_scaling_rel() + cluster.beta_scaling_rel()*log_lambda_min + cluster.gamma_scaling_rel()*log_f_z_min;
  double M2 = cluster.alpha_scaling_rel() + cluster.beta_scaling_rel()*log_lambda_max + cluster.gamma_scaling_rel()*log_f_z_min;
  double M3 = cluster.alpha_scaling_rel() + cluster.beta_scaling_rel()*log_lambda_min + cluster.gamma_scaling_rel()*log_f_z_max;
  double M4 = cluster.alpha_scaling_rel() + cluster.beta_scaling_rel()*log_lambda_max + cluster.gamma_scaling_rel()*log_f_z_max;

  double min1 = std::min(M1, M2);
  double min2 = std::min(min1, M3);
  double minM = std::min(min2, M4);
  double max1 = std::max(M1, M2);
  double max2 = std::max(max1, M3);
  double maxM = std::max(max2, M4);

  // Find the maximum value of the intrinsic scatter
  double s1 = cluster.scatter0_scaling_rel() + cluster.scatterM_scaling_rel()*pow(log_lambda_min, cluster.scatterM_exponent_scaling_rel()) + cluster.scatterz_scaling_rel()*pow(log_f_z_min, cluster.scatterz_exponent_scaling_rel());
  double s2 = cluster.scatter0_scaling_rel() + cluster.scatterM_scaling_rel()*pow(log_lambda_max, cluster.scatterM_exponent_scaling_rel()) + cluster.scatterz_scaling_rel()*pow(log_f_z_min, cluster.scatterz_exponent_scaling_rel());
  double s3 = cluster.scatter0_scaling_rel() + cluster.scatterM_scaling_rel()*pow(log_lambda_min, cluster.scatterM_exponent_scaling_rel()) + cluster.scatterz_scaling_rel()*pow(log_f_z_max, cluster.scatterz_exponent_scaling_rel());
  double s4 = cluster.scatter0_scaling_rel() + cluster.scatterM_scaling_rel()*pow(log_lambda_max, cluster.scatterM_exponent_scaling_rel()) + cluster.scatterz_scaling_rel()*pow(log_f_z_max, cluster.scatterz_exponent_scaling_rel());

  double maxs1 = std::max(s1, s2);
  double maxs2 = std::max(maxs1, s3);
  double max_intrinsic_scatter = std::max(maxs2, s4);

  // Define the integral limits
  int integral_dimension=3;
  std::vector<std::vector<double>> integration_limits(integral_dimension);
  integration_limits[0] = {minM-3.5*max_intrinsic_scatter, std::min(maxM+3.5*max_intrinsic_scatter,log(1.e16/mass_pivot)/log(log_base))};
  integration_limits[1] = {std::max(redshift_min - 3.5*z_error({cluster.zerror(), redshift_min}), 0.), redshift_max + 3.5*z_error({cluster.zerror(), redshift_max})};
  integration_limits[2] = {std::max(proxy_min - 3.5*proxy_error({cluster.proxyerror(), proxy_min}), 1.), proxy_max + 3.5*proxy_error({cluster.proxyerror(), proxy_max})};

  // Compute the integral
  cbl::wrapper::cuba::CUBAwrapper CW (integrand, integral_dimension);
  double nc;

  if (integration_limits[0][0] < integration_limits[0][1])
    nc = CW.IntegrateVegas(integration_limits,false);
  else
    nc = 0;

  return nc * mass_pivot * log(log_base) * weight;  
}



// ===========================================================================================


double cbl::modelling::numbercounts::counts_proxy_classic (double (*fz)(std::vector<double>, std::shared_ptr<void>), double (*z_error)(std::vector<double>), double (*proxy_error)(std::vector<double>), double (*transfer_func)(const double, const double, const double, const std::string, const double, const std::string, std::shared_ptr<void>), const double redshift_min, const double redshift_max, const double proxy_min, const double proxy_max, cbl::cosmology::Cosmology cosmology, cbl::catalogue::Cluster cluster, const double Area, const std::string model_MF, const std::string model_bias, const bool store_output, const double Delta, const bool isDelta_vir, const cbl::glob::FuncGrid interp_sigmaM, const  cbl::glob::FuncGrid interp_DlnsigmaM, const double proxy_pivot, const double z_pivot, const double mass_pivot, const double log_base, const double weight)
{  
  double fact = (cosmology.unit()) ? 1 : cosmology.hh();
  std::shared_ptr<void> pp;
  auto cosmology_ptr = std::make_shared<cbl::cosmology::Cosmology>(cosmology);
  
  // Total integrand
  auto integrand = [&z_error,&proxy_error,&model_bias,&transfer_func,&fz,&cosmology_ptr,&cosmology,&cluster,&log_base,&proxy_pivot,&z_pivot,&mass_pivot,&proxy_min,&proxy_max,&redshift_min,&redshift_max,&interp_sigmaM,&interp_DlnsigmaM,&store_output,&fact,&model_MF,&Delta,&Area,&isDelta_vir,&pp] (const std::vector<double> x)
    {
      double Delta_ = (isDelta_vir) ? cosmology.Delta_vir(Delta, x[1]) : Delta;
      double Mass = pow(log_base,x[0])*mass_pivot;
      double normM = pow(log_base,x[0]);

      // Compute P(lambda|M,z)
      double log_M = x[0];
      double log_f_z = log( fz({x[1], z_pivot}, cosmology_ptr) )/log(log_base);

      double mean = cluster.alpha_scaling_rel() + cluster.beta_scaling_rel()*log_M + cluster.gamma_scaling_rel()*log_f_z + log(proxy_pivot)/log(log_base);
      double sigma = cluster.scatter0_scaling_rel() + cluster.scatterM_scaling_rel()*pow(log_M, cluster.scatterM_exponent_scaling_rel()) + cluster.scatterz_scaling_rel()*pow(log_f_z, cluster.scatterz_exponent_scaling_rel());
      double P_lambda__M_z = (cbl::gaussian(log(x[2])/log(log_base), pp, {mean,sigma})) / (x[2]*log(log_base));
      
      // Compute the integrals of P(z|z) and P(lambda|lambda)
      double mean_Pz = x[1] + cluster.zbias() * (1+x[1]);
      double int_P_z = 0.5 * ( erf( (redshift_max - mean_Pz) / (sqrt(2)*z_error({cluster.zerror(), redshift_max})) ) - erf( (redshift_min - mean_Pz) / (sqrt(2)*z_error({cluster.zerror(), redshift_min})) ) );
      double int_P_lambda = 0.5 * ( erf( (proxy_max - x[2]) / (sqrt(2)*proxy_error({cluster.proxyerror(), proxy_max})) ) - erf( (proxy_min - x[2]) / (sqrt(2)*proxy_error({cluster.proxyerror(), proxy_min})) ) );
      
      return transfer_func(Mass, interp_sigmaM(Mass*fact), x[1], model_bias, Delta_, "EisensteinHu", cosmology_ptr) * cosmology.mass_function(Mass, interp_sigmaM(Mass*fact), interp_DlnsigmaM(Mass*fact), x[1], model_MF, store_output, cbl::par::defaultString, Delta_)*Area*cosmology.dV_dZdOmega(x[1], true) * normM * P_lambda__M_z * int_P_z * int_P_lambda;
    };
  
  // -------------------------------------------------------------

  // Find the minimum and maximum masses, given the parameters of the scaling relation
  double scatter = cluster.scatter0_scaling_rel();
  
  double log_lambda_min = std::max(log(proxy_min/proxy_pivot)/log(log_base) - 3.5*scatter, log(0.0001/proxy_pivot)/log(log_base));
  double log_lambda_max = log(proxy_max/proxy_pivot)/log(log_base) + 3.5*scatter;
  double log_f_z_min = log( fz({redshift_min, z_pivot}, cosmology_ptr) )/log(log_base);
  double log_f_z_max = log( fz({redshift_max, z_pivot}, cosmology_ptr) )/log(log_base);

  double M1 = (- cluster.alpha_scaling_rel() + log_lambda_min - cluster.gamma_scaling_rel()*log_f_z_min) / cluster.beta_scaling_rel();
  double M2 = (- cluster.alpha_scaling_rel() + log_lambda_max - cluster.gamma_scaling_rel()*log_f_z_min) / cluster.beta_scaling_rel();
  double M3 = (- cluster.alpha_scaling_rel() + log_lambda_min - cluster.gamma_scaling_rel()*log_f_z_max) / cluster.beta_scaling_rel();
  double M4 = (- cluster.alpha_scaling_rel() + log_lambda_max - cluster.gamma_scaling_rel()*log_f_z_max) / cluster.beta_scaling_rel();

  double min1 = std::min(M1, M2);
  double min2 = std::min(min1, M3);
  double minM_ = std::min(min2, M4);
  double max1 = std::max(M1, M2);
  double max2 = std::max(max1, M3);
  double maxM_ = std::max(max2, M4);

  double minM = std::max(minM_, log(1.e12/mass_pivot)/log(log_base));
  double maxM = std::min(maxM_, log(1.e16/mass_pivot)/log(log_base));
  
  // Define the integral limits
  int integral_dimension=3;
  std::vector<std::vector<double>> integration_limits(integral_dimension);
  integration_limits[0] = {minM, maxM};
  integration_limits[1] = {std::max(redshift_min - 3.5*z_error({cluster.zerror(), redshift_min}), 0.), redshift_max + 3.5*z_error({cluster.zerror(), redshift_max})};
  integration_limits[2] = {std::max(proxy_min - 3.5*proxy_error({cluster.proxyerror(), proxy_min}), 1.), proxy_max + 3.5*proxy_error({cluster.proxyerror(), proxy_max})};

  // Compute the integral
  cbl::wrapper::cuba::CUBAwrapper CW (integrand, integral_dimension);
  double nc;

  if (integration_limits[0][0] < integration_limits[0][1])
    nc = CW.IntegrateVegas(integration_limits,false);
  else
    nc = 0;
  
  return nc * mass_pivot * log(log_base) * weight;
}


// ===========================================================================================

std::vector<double> cbl::modelling::numbercounts::size_function (cbl::cosmology::Cosmology cosmology, const std::vector<double> radii, const double redshift, const std::string model, const double b_eff, double slope, double offset, const double deltav_NL, const double del_c, const std::string method_Pk, const double k_Pk_ratio, const bool store_output, const std::string output_root, const std::string interpType, const double k_max, const std::string input_file, const bool is_parameter_file)
{
  
  vector<double> size_function = cosmology.size_function(radii, redshift, model, b_eff, slope, offset, deltav_NL, del_c, method_Pk, k_Pk_ratio, store_output, output_root, interpType, k_max, input_file, is_parameter_file);

  return size_function;
  
}
