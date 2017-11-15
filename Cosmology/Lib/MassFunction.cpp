/********************************************************************
 *  Copyright (C) 2010 by Federico Marulli                          *
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
 *  @file Cosmology/Lib/MassFunction.cpp
 *
 *  @brief Methods of the class Cosmology used to model the mass
 *  function
 *
 *  This file contains the implementation of the methods of the class
 *  Cosmology used to model the mass function of dark matter haloes
 *
 *  @author Federico Marulli 
 *
 *  @author federico.marulli3@unbo.it
 */

#include "Cosmology.h"

using namespace cosmobl;


// =====================================================================================


double cosmobl::cosmology::Cosmology::mass_function (const double Mass, const double redshift, const string model_MF, const string method_SS, const string output_root, const double Delta, const string interpType, const int norm, const double k_min, const double k_max, const double prec, const string input_file, const bool is_parameter_file, const bool default_delta, const double delta_t) 
{
  double fact = (m_unit) ? 1 : m_hh;
  double MASS = Mass*fact;
  
  const bool unit1 = true;

  double SSS = sigma2M(MASS, method_SS, 0., output_root, interpType, k_max, input_file, is_parameter_file, unit1);
  double Sigma = sqrt(SSS);
  double Dln_Sigma = dnsigma2M(1, MASS, method_SS, 0., output_root, interpType, k_max, input_file, is_parameter_file, unit1)*(MASS/(2.*SSS));

  double MF = m_MF_generator(MASS, Sigma, Dln_Sigma, redshift, model_MF, Delta, default_delta, delta_t)*pow(fact, 4.);

  if (m_fNL!=0 && default_delta) MF *= MF_correction(MASS, redshift, model_MF, output_root, interpType, norm, k_min, k_max, prec, input_file, is_parameter_file);
  else if (m_fNL!=0 && !default_delta) ErrorCBL("Non Gaussianity still not available for user-defined density contrast threshold!");

  return MF;
}


// =====================================================================================


double cosmobl::cosmology::Cosmology::m_mass_function (const double Mass, shared_ptr<void> mass_function_params) 
{
  shared_ptr<glob::STR_MF> pp = static_pointer_cast<glob::STR_MF>(mass_function_params);

  double fact = (m_unit) ? 1 : m_hh;
  double MASS = Mass*fact;

  const bool unit1 = true;

  double SSS = sigma2M(MASS, pp->method_SS, 0., pp->output_root, pp->interpType, pp->k_max, pp->input_file, pp->is_parameter_file, unit1);
  double Sigma = sqrt(SSS);
  double Dln_Sigma = dnsigma2M(1, MASS, pp->method_SS, 0., pp->output_root, pp->interpType, pp->k_max, pp->input_file, pp->is_parameter_file, unit1)*(MASS/(2.*SSS));

  double MF = m_MF_generator(MASS, Sigma, Dln_Sigma, pp->redshift, pp->model_MF, pp->Delta,pp-> default_delta, pp->delta_t)*pow(fact, 4.);

  if (m_fNL!=0 && pp->default_delta) MF *= MF_correction(MASS, pp->redshift, pp->model_MF, pp->output_root, pp->interpType, pp->norm, pp->k_min, pp->k_max, pp->prec, pp->input_file, pp->is_parameter_file);
  else if (m_fNL!=0 && !pp->default_delta) ErrorCBL("Non Gaussianity still not available for user-defined density contrast threshold!");

  return MF;
}


// =====================================================================================


double cosmobl::cosmology::Cosmology::mass_function_fast (const double Mass, const double redshift, const string model_MF, const string method_SS, const string output_root, const double Delta, const string interpType, const int norm, const double k_min, const double k_max, const double prec, const string input_file, const bool is_parameter_file) 
{
  double fact = (m_unit) ? 1 : m_hh;
  double MASS = Mass*fact;
  

  // ---------- read the grid file ---------- 
  
  const string file_grid = create_grid_sigmaM(method_SS, 0., output_root, interpType, k_max);

  ifstream fin(file_grid.c_str()); checkIO(fin, file_grid); 

  double MMass, Sigma, Dln_Sigma;
  vector<double> mass, sigma, dln_sigma;
  while (fin >>MMass>>Sigma>>Dln_Sigma) {
    mass.push_back(MMass);
    sigma.push_back(Sigma);
    dln_sigma.push_back(Dln_Sigma);
  } 


  // ---------- compute the MF ---------- 
   
  double sig = interpolated(MASS, mass, sigma, "Steffen");
  double dlsig = interpolated(MASS, mass, dln_sigma, "Steffen");
  
  double MF = m_MF_generator(MASS, sig, dlsig, redshift, model_MF, Delta)*pow(fact, 4.);
  if (std::isnan(MF)) { string Err = "Error in cosmobl::cosmology::Cosmology::mass_function_fast of MassFunction.cpp: MF = " + conv(MF,par::fDP3) + "!"; ErrorCBL(Err); }

  if (m_fNL!=0) MF *= MF_correction(MASS, redshift, method_SS, output_root, interpType, norm, k_min, k_max, prec, input_file, is_parameter_file);

  return MF;  
}


// =====================================================================================


double cosmobl::cosmology::Cosmology::mass_function (const double Mass, const double Sigma, const double Dln_Sigma, const double redshift, const string model_MF, const string output_root, const double Delta, const string interpType, const int norm, const double k_min, const double k_max, const double prec, const string method_SS, const string input_file, const bool is_parameter_file) 
{
  double fact = (m_unit) ? 1 : m_hh;
  double MASS = Mass*fact;

  double MF = m_MF_generator(MASS, Sigma, Dln_Sigma, redshift, model_MF, Delta)*pow(fact, 4.);
  
  if (m_fNL!=0) MF *= MF_correction(MASS, redshift, method_SS, output_root, interpType, norm, k_min, k_max, prec, input_file, is_parameter_file);

  return MF;
}


// =====================================================================================


double cosmobl::cosmology::Cosmology::m_MF_generator (const double Mass, const double Sigma, const double Dln_Sigma, const double redshift, const string model_MF, const double Delta, const bool default_delta, const double delta_t) 
{ 
  const double deltacz = (default_delta) ? deltac(redshift) : fabs(delta_t*DD(redshift)/DD(0.));
  const double sigmaz = Sigma*DD(redshift)/DD(0.);
  
  const double RHO = rho_m(0., true); 
  
  if (model_MF=="PS") // Press & Schechter
    return sqrt(2./par::pi)*RHO/(Mass*Mass)*deltacz/sigmaz*fabs(Dln_Sigma)*exp(-(deltacz*deltacz)*0.5/(sigmaz*sigmaz));
   
  else if (model_MF=="ST") { // Sheth & Tormen
    double aa = 0.707;
    double qq = 0.3;
    double AA = 0.3222;
    return AA*sqrt(2.*aa/par::pi)*RHO/(Mass*Mass)*deltacz/sigmaz*(1+pow(sigmaz/(sqrt(aa)*deltacz),2.*qq))*fabs(Dln_Sigma)*exp(-(aa*deltacz*deltacz)*0.5/(sigmaz*sigmaz));
  }
  
  else if (model_MF=="Jenkins") // Jenkins et al. (2001)
    return 0.315*exp(-pow(fabs(log(1./sigmaz)+0.61),3.8))*RHO/(Mass*Mass)*fabs(Dln_Sigma);
  
  else if (model_MF=="Warren") { // Warren et al. (2006) 
    double AA = 0.7234;
    double aa = 1.625;
    double bb = 0.2538;
    double cc = 1.1982;
    return AA*(pow(sigmaz,-aa)+bb)*exp(-cc/(sigmaz*sigmaz))*RHO/(Mass*Mass)*fabs(Dln_Sigma);
  }
  
  else if (model_MF=="Reed") { // Reed et al. (2007) 
    double cc = 1.08;
    double aa = 0.764/cc;
    double pp = 0.3;
    double AA = 0.3222;
    double Fatt = AA*sqrt(2.*aa/par::pi);
    double n_eff = -6.*Dln_Sigma-3.;
    double G1 = exp(-pow(log(1./sigmaz)-0.4,2)/0.72);
    double G2 = exp(-pow(log(1./sigmaz)-0.75,2)/0.08);
    return Fatt*RHO/(Mass*Mass)*deltacz/sigmaz*(1+pow(sigmaz*sigmaz/(aa*deltacz*deltacz),pp)+0.6*G1+0.4*G2)*fabs(Dln_Sigma)*exp(-(cc*aa*deltacz*deltacz)*0.5/(sigmaz*sigmaz)-(0.03/pow(n_eff+3.,2)*pow(deltacz/sigmaz,0.6)));
  }

  else if (model_MF=="Pan") { // Pan (2007)
    double alpha = 0.435; // check!!!
    return 4.*alpha/sqrt(2.*par::pi)*RHO/(Mass*Mass)*deltacz/pow(sigmaz,2.*alpha)*fabs(Dln_Sigma)*exp(-(deltacz*deltacz)*0.5/pow(sigmaz,4.*alpha));
  }

  else if (model_MF=="ShenH") { // halo MF, Shen et al. (2006) // check!!!
    double alpha = -0.55;
    double beta = -0.56;
    double ni = pow(deltacz/sigmaz,2);
    double ni_fni = sqrt(ni/(2.*par::pi))*exp(-ni*pow(1.-beta*pow(ni,alpha),2)*0.5)*(1.-beta/pow(ni,-alpha)*(1+alpha+(alpha*(alpha+1.))*0.5));
    return ni_fni*RHO/(Mass*Mass)*fabs(Dln_Sigma)*2;
  }
  
  else if (model_MF=="ShenF") { // filament MF, Shen et al. (2006) // check!!!
    double alpha = -0.28;
    double beta = -0.012;
    double ni = pow(deltacz/sigmaz,2);
    double ni_fni = sqrt(ni/(2.*par::pi))*exp(-ni*pow(1.-beta*pow(ni,alpha),2)*0.5)*(1.-beta/pow(ni,-alpha)*(1+alpha+(alpha*(alpha+1.))*0.5));
    return ni_fni*RHO/(Mass*Mass)*fabs(Dln_Sigma)*2;
  }

  else if (model_MF=="ShenS") { // sheet MF, Shen et al. (2006) // check!!!
    double alpha = -0.61;
    double beta = 0.45;
    double ni = pow(deltacz/sigmaz,2);
    double ni_fni = sqrt(ni/(2.*par::pi))*exp(-ni*pow(1.-beta*pow(ni,alpha),2)*0.5)*(1.-beta/pow(ni,-alpha)*(1+alpha+(alpha*(alpha+1.))*0.5));
    return fabs(ni_fni*RHO/(Mass*Mass)*fabs(Dln_Sigma)*2); // check!!!
  }

  else if (model_MF=="Tinker") { // Tinker et al. (2008)

    //if (redshift>2) WarningMsg("Attention: the Tinker mass function has been tested for z<~2!");

    double A0, a0, b0, c0;

    if      (Delta==200)  {A0 = 0.186; a0 = 1.47; b0 = 2.57; c0 = 1.19;}
    else if (Delta==300)  {A0 = 0.200; a0 = 1.52; b0 = 2.25; c0 = 1.27;}
    else if (Delta==400)  {A0 = 0.212; a0 = 1.56; b0 = 2.05; c0 = 1.34;}
    else if (Delta==600)  {A0 = 0.218; a0 = 1.61; b0 = 1.87; c0 = 1.45;}
    else if (Delta==800)  {A0 = 0.248; a0 = 1.87; b0 = 1.59; c0 = 1.58;}
    else if (Delta==1200) {A0 = 0.255; a0 = 2.13; b0 = 1.51; c0 = 1.80;}
    else if (Delta==1600) {A0 = 0.260; a0 = 2.30; b0 = 1.46; c0 = 1.97;}
    else if (Delta==2400) {A0 = 0.260; a0 = 2.53; b0 = 1.44; c0 = 2.24;}
    else if (Delta==3200) {A0 = 0.260; a0 = 2.66; b0 = 1.41; c0 = 2.44;}
    else {
      A0 = (Delta<1600) ? 0.1*log10(Delta)-0.05 : 0.26;
      a0 = 1.43+pow(max(log10(Delta)-2.3,0.),1.5); // check!!!
      b0 = 1.+pow(log10(Delta)-1.6,-1.5);
      c0 = 1.2+pow(max(log10(Delta)-2.35,0.),1.6); // check!!!
    }

    double alpha = pow(10.,-pow(max(0.75/log10(Delta/75.),0.),1.2)); // check!!!
    double AA = A0*pow(1.+redshift,-0.14);
    double aa = a0*pow(1.+redshift,-0.06);
    double bb = b0*pow(1.+redshift,-alpha);
    double cc = c0;

    return AA*(pow(sigmaz/bb,-aa)+1.)*exp(-cc/(sigmaz*sigmaz))*RHO/(Mass*Mass)*fabs(Dln_Sigma);
  }

  else if (model_MF=="Crocce") { // Crocce et al. (2010)
    double AA = 0.58*pow(1.+redshift,-0.13);
    double aa = 1.37*pow(1.+redshift,-0.15);
    double bb = 0.3*pow(1.+redshift,-0.084);
    double cc = 1.036*pow(1.+redshift,-0.024);
    return AA*(pow(sigmaz,-aa)+bb)*exp(-cc*pow(sigmaz,-2))*RHO/(Mass*Mass)*fabs(Dln_Sigma);
  }
  
  else if (model_MF=="Angulo_FOF") // FoF MF, Angulo et al. (2012)
    return 0.201*(pow(2.08/sigmaz,1.7)+1)*exp(-1.172/(sigmaz*sigmaz))*RHO/(Mass*Mass)*fabs(Dln_Sigma);
 
  else if (model_MF=="Angulo_Sub") // SUBFIND MF, Angulo et al. (2012)
    return 0.265*(pow(1.675/sigmaz,1.9)+1)*exp(-1.4/(sigmaz*sigmaz))*RHO/(Mass*Mass)*fabs(Dln_Sigma);

  else if (model_MF=="Bhattacharya") { // Bhattacharya et al. (2011) // check!!!
    double aa = 0.788/pow(1+redshift,0.01);
    double qq = 1.795;
    double AA = 0.333/pow(1+redshift,0.11);
    double pp = 0.807;
    double nu = deltacz/sigmaz;
    //if (redshift>=2) aa = 0.799/pow(1+redshift,0.024); 
    return RHO/(Mass*Mass)*fabs(Dln_Sigma)*AA*sqrt(2/par::pi)*exp(-nu*nu*0.5*aa)*(1+pow(1/(nu*nu*aa),pp))*pow(nu*sqrt(aa),qq);
  }

  else if (model_MF=="Courtin") { // Courtin et al. (2010) 
    double aa = 0.695;
    double qq = 0.1;
    double AA = 0.348; 
    return AA*sqrt(2.*aa/par::pi)*RHO/(Mass*Mass)*deltacz/sigmaz*(1+pow(sigmaz/(sqrt(aa)*deltacz),2.*qq))*fabs(Dln_Sigma)*exp(-(aa*deltacz*deltacz)*0.5/(sigmaz*sigmaz));
  }

  else if (model_MF=="Manera") { // Manera et al. (2010)
    
    double aa, qq;
    if (fabs(redshift-0.)<1.e-30) {
      aa = 0.709;
      qq = 0.248;
    }
    else if (fabs(redshift-0.5)<1.e-30) {
      aa = 0.724;
      qq = 0.241;
    }
    else 
      ErrorCBL("Error in cosmobl::cosmology::Cosmology::m_MF_generator of MassFunction.cpp! The Manera mass function has been tested only for z=0 and 0.5!");
    
    return 0.3222*sqrt(2.*aa/par::pi)*RHO/(Mass*Mass)*deltacz/sigmaz*(1+pow(sigmaz/(sqrt(aa)*deltacz),2.*qq))*fabs(Dln_Sigma)*exp(-(aa*deltacz*deltacz)*0.5/(sigmaz*sigmaz));
  }

  else if (model_MF=="Watson_FOF") { // FoF MF, Watson et al. (2012) 
    double a = 0.282;
    double b = 1.406;
    double c = 2.163;
    double d = 1.210;
    return a*(pow((b/sigmaz),c)+1)*exp(-d/(sigmaz*sigmaz))*RHO/(Mass*Mass)*fabs(Dln_Sigma);
  }

  else if (model_MF=="Watson_SOH") { // Spherical Overdensity halo MF, Watson et al. (2012) 
    double Omega = OmegaM(redshift);
    double C = exp(0.023*(Delta/178-1));
    double d = -0.456*Omega-0.139;
    double p = 0.072;
    double q = 2.13;
    double Gamma = C*pow((Delta/178),d)*exp(p*(1-Delta/178)/pow(sigmaz,q));
    double A = 0.194;
    double alpha = 2.267;
    double beta = 1.805;
    double gamma = 1.287;
    if (redshift>0 && redshift<6) {
      A = Omega*(1.097*pow((1+redshift),-3.216)+0.074);
      alpha = Omega*(3.136*pow((1+redshift),-3.056)+2.349);
      beta = Omega*(5.907*pow((1+redshift),-3.599)+2.344);
      gamma = 1.318;
    }
    if (redshift>=6) {
      A = 0.563;
      alpha = 0.874;
      beta = 3.810;
      gamma = 1.453;
    }
    return RHO/(Mass*Mass)*fabs(Dln_Sigma)*Gamma*A*(pow((beta/sigmaz),alpha)+1.)*exp(-gamma/(sigmaz*sigmaz));
  }

  else if (model_MF=="Peacock") { // Peacock et al. (2007)
    double aa = 1.529;
    double bb = 0.704;
    double cc = 0.412;
    double nu = deltacz/sigmaz;
    return nu*exp(-cc*nu*nu)/pow(1+aa*pow(nu,bb),2)*(bb*aa*pow(nu,bb-1)+2*cc*nu*(1+aa* pow(nu,bb)))*RHO/(Mass*Mass)*fabs(Dln_Sigma);
  }

  else return ErrorCBL("Error in cosmobl::cosmology::Cosmology::m_MF_generator of MassFunction.cpp: model_MF not allowed!");  
}


// =====================================================================================


double cosmobl::cosmology::Cosmology::n_haloes (const double Mass_min, const double Mass_max, const double z_min, const double z_max, const bool angle_rad, const string model_MF, const string method_SS, const string output_root, const double Delta, const string interpType, const double k_max, const string input_file, const bool is_parameter_file) 
{

  // ---------- read the grid file ---------- 

  const string file_grid = create_grid_sigmaM(method_SS, 0., output_root,interpType, k_max, input_file, is_parameter_file);

  ifstream fin(file_grid.c_str()); checkIO(fin, file_grid); 
  
  double Mass, Sigma, Dln_Sigma;
  vector<double> mass, sigma, dlnsigma;
  while (fin >>Mass>>Sigma>>Dln_Sigma) {
    if (Mass_min<Mass && Mass<Mass_max) {
      mass.push_back(Mass);
      sigma.push_back(Sigma);
      dlnsigma.push_back(Dln_Sigma);
    }
  }


  // ---------- compute the number of haloes ---------- 
  
  int step_z = 100;
  double delta_z = (z_max-z_min)/step_z; 
  double redshift = z_min;
  double N_haloes = 0.;

  for (int Red=0; Red<step_z; Red++) {

    double Int = 0.;
    for (unsigned int k=0; k<mass.size()-1; k++) 
      Int += mass_function(mass[k], sigma[k], dlnsigma[k], redshift, model_MF, output_root, Delta)*dV_dZdOmega(redshift, angle_rad)*(mass[k+1]-mass[k]);

    N_haloes += Int*delta_z;

    redshift += delta_z;
  }
  
  return N_haloes;
}


// =====================================================================================


double cosmobl::cosmology::Cosmology::n_haloes (const double Mass_min, const double Mass_max, const double Volume, const double redshift, const string model_MF, const string method_SS, const int nbin_mass, const string output_root, const double Delta, const string interpType, const int norm, const double k_min, const double k_max, const double prec, const string input_file, const bool is_parameter_file, const bool default_delta, const double delta_t) 
{
  glob::STR_MF mass_function_par;

  mass_function_par.redshift = redshift;
  mass_function_par.model_MF = model_MF ;
  mass_function_par.method_SS = method_SS;
  mass_function_par.output_root = output_root;
  mass_function_par.Delta = Delta;
  mass_function_par.interpType = interpType;
  mass_function_par.norm = norm; 
  mass_function_par.k_min = k_min; 
  mass_function_par.k_max = k_max;
  mass_function_par.prec = prec;
  mass_function_par.input_file = input_file;
  mass_function_par.is_parameter_file = is_parameter_file;
  mass_function_par.default_delta = default_delta;
  mass_function_par.delta_t = delta_t;

  auto pp = make_shared<glob::STR_MF>(mass_function_par);

  if (nbin_mass==0) {  
    function<double(double)> func = bind(&Cosmology::m_mass_function, this, std::placeholders::_1, pp);
    return Volume*gsl::GSL_integrate_qag(func, Mass_min, Mass_max, prec);
  }

  else if (nbin_mass>0)
  {
    vector<double> mass = logarithmic_bin_vector(nbin_mass, Mass_min, Mass_max);
    vector<double> dndm(nbin_mass);

    for (int i=0; i<nbin_mass; i++)
      dndm[i] = m_mass_function(mass[i], pp);

    glob::FuncGrid mass_function_interpolator(mass, dndm, interpType);

    return Volume*mass_function_interpolator.integrate_qag(Mass_min, Mass_max, prec);
  }
  
  else
    ErrorCBL("Error in n_haloes of MassFunction.cpp! nbinmass must be >=0");

  return 0;
}


// =====================================================================================


double cosmobl::cosmology::Cosmology::MhaloMin (const int n_halo, const double Area, const bool angle_rad, const double z_min, const double z_max, const double Mmax, const double lgM1_guess, const double lgM2_guess, const string model_MF, const string method_SS, const string output_root, const double Delta, const string interpType, const double k_max, const string input_file, const bool is_parameter_file) const
{
  cosmobl::classfunc::func_MhaloMin func(m_Omega_matter, m_Omega_baryon, m_Omega_neutrinos, m_massless_neutrinos, m_massive_neutrinos, m_Omega_DE, m_Omega_radiation, m_hh, m_scalar_amp, m_scalar_pivot, m_n_spec, m_w0, m_wa, m_fNL, m_type_NG, m_tau, m_model, m_unit, n_halo, Area, angle_rad, z_min, z_max, Mmax, model_MF, method_SS, output_root, Delta, interpType, k_max, input_file, is_parameter_file);
  
  function<double(double) > ff = bind(&cosmobl::classfunc::func_MhaloMin::operator(), func, std::placeholders::_1);
  double prec = 0.0001;
  double lgM = gsl::GSL_root_brent (ff, 0., lgM1_guess, lgM2_guess, prec);

  return pow(10., lgM);
}


// =====================================================================================


double cosmobl::cosmology::Cosmology::unevolved_mass_function (const double mass_accr) const
{
  const double aa = -0.8;
  const double mm = mass_accr/aa; // mass_accr = m/M
  const double yn = log(0.3); // log(0.21) check!!!

  return aa*log(-mm)+log(exp(2.*par::pi*pow(mm,3.)))+yn;
}


// =====================================================================================


double cosmobl::cosmology::Cosmology::n_haloes_selection_function (const double Mass_min, const double Mass_max, const double z_min, const double z_max, const bool angle_rad, const string model_MF, const string method_SS, const string selection_function_file, const vector<int> cols, const string output_root, const double Delta, const bool isDelta_vir, const string interpType, const double k_max, const string input_file, const bool is_parameter_file) 
{

  // ---------- read the grid file ---------- 

  const string file_grid = create_grid_sigmaM(method_SS, 0., output_root,interpType, k_max, input_file, is_parameter_file);

  ifstream fin(file_grid.c_str()); checkIO(fin, file_grid); 
  
  double Mass, Sigma, Dln_Sigma;
  vector<double> mass, sigma, dlnsigma;
  while (fin >>Mass>>Sigma>>Dln_Sigma) {
    if (Mass_min<Mass && Mass<Mass_max) {
      mass.push_back(Mass);
      sigma.push_back(Sigma);
      dlnsigma.push_back(Dln_Sigma);
    }
  }
  fin.clear(); fin.close();

  glob::FuncGrid interp_sigma(mass, sigma, "Spline");
  glob::FuncGrid interp_dlnsigma(mass, dlnsigma, "Spline");

  // ---------- read the selection function ---------- 

  vector<double> mass_SF, redshift_SF;
  vector<vector<double>> selection_function;

  read_matrix(selection_function_file, mass_SF, redshift_SF, selection_function, cols);
  function<double(const double, const double)> interp_SF = bind(interpolated_2D, std::placeholders::_1, std::placeholders::_2, mass_SF, redshift_SF, selection_function, "Linear"); 

  // ---------- compute the number of haloes ---------- 

  vector<vector<double>> limits = {{Mass_min, Mass_max}, {z_min, z_max}};
  
  int step_z = 100;
  double delta_z = (z_max-z_min)/step_z; 
  double redshift = z_min;
  double N_haloes = 0.;
  //double norm_SF = 0.;


  for (int Red=0; Red<step_z; Red++) {
    double Int = 0.;
    for (unsigned int k=0; k<mass.size()-1; k++) {
      double DD = (isDelta_vir) ? Delta_vir(Delta, redshift) : Delta;
      double SF = interp_SF(mass[k], redshift);
      Int += SF*mass_function(mass[k], sigma[k], dlnsigma[k], redshift, model_MF, output_root, DD)*dV_dZdOmega(redshift, angle_rad)*(mass[k+1]-mass[k]);
    }

    N_haloes += Int*delta_z;

    redshift += delta_z;
  }

  return N_haloes;
}


// =====================================================================================


vector<double> cosmobl::cosmology::Cosmology::mass_function_vector (const vector<double> mass, const double z_min, const double z_max, const string model_MF, const string method_SS, const string output_root, const double Delta, const bool isDelta_vir, const string interpType, const double k_max, const string input_file, const bool is_parameter_file) 
{
  vector<double> MF(mass.size());

  // ---------- read the grid file ---------- 

  const string file_grid = create_grid_sigmaM(method_SS, 0., output_root,interpType, k_max, input_file, is_parameter_file);

  ifstream fin(file_grid.c_str()); checkIO(fin, file_grid); 
  
  double Mass, Sigma, Dln_Sigma;
  vector<double> _mass, _sigma, _dlnsigma;
  while (fin >>Mass>>Sigma>>Dln_Sigma) {
      _mass.push_back(Mass);
      _sigma.push_back(Sigma);
      _dlnsigma.push_back(Dln_Sigma);
  }
  fin.clear(); fin.close();

  glob::FuncGrid interp_sigma(_mass, _sigma, "Spline");
  glob::FuncGrid interp_dlnsigma(_mass, _dlnsigma, "Spline");

  // ---------- compute the number of haloes ---------- 

  //int step_z = 100;
  //double delta_z = (z_max-z_min)/step_z; 

  double VV = Volume(z_min, z_max, 1.);

  for (unsigned int k=0; k<mass.size(); k++) {
    double sigma = interp_sigma(mass[k]);
    double dlnsigma = interp_dlnsigma(mass[k]);

    auto func = [&] (const double redshift) 
    {
      double DD = (isDelta_vir) ? Delta_vir(Delta, redshift) : Delta;
      return mass_function(mass[k], sigma, dlnsigma, redshift, model_MF, output_root, DD)*dV_dZdOmega(redshift, false);
    };
    MF[k] = gsl::GSL_integrate_qag(func, z_min, z_max)/VV;
  }

  return MF;
}


// =====================================================================================


vector<double> cosmobl::cosmology::Cosmology::mass_function_selection_function_vector (const vector<double> mass, const double z_min, const double z_max, const string model_MF, const string method_SS, const string selection_function_file, const vector<int> cols, const string output_root, const double Delta, const bool isDelta_vir, const string interpType, const double k_max, const string input_file, const bool is_parameter_file) 
{
  vector<double> MF(mass.size());

  // ---------- read the grid file ---------- 

  const string file_grid = create_grid_sigmaM(method_SS, 0., output_root,interpType, k_max, input_file, is_parameter_file);

  ifstream fin(file_grid.c_str()); checkIO(fin, file_grid); 
  
  double Mass, Sigma, Dln_Sigma;
  vector<double> _mass, _sigma, _dlnsigma;
  while (fin >>Mass>>Sigma>>Dln_Sigma) {
      _mass.push_back(Mass);
      _sigma.push_back(Sigma);
      _dlnsigma.push_back(Dln_Sigma);
  }
  fin.clear(); fin.close();

  glob::FuncGrid interp_sigma(_mass, _sigma, "Spline");
  glob::FuncGrid interp_dlnsigma(_mass, _dlnsigma, "Spline");

  // ---------- read the selection function ---------- 

  vector<double> mass_SF, redshift_SF;
  vector<vector<double>> selection_function;

  read_matrix(selection_function_file, mass_SF, redshift_SF, selection_function, cols);
  function<double(const double, const double)> interp_SF = bind(interpolated_2D, std::placeholders::_1, std::placeholders::_2, mass_SF, redshift_SF, selection_function, "Linear"); 

  // ---------- compute the number of haloes ---------- 

  //int step_z = 100;
  //double delta_z = (z_max-z_min)/step_z; 

  double VV = Volume(z_min, z_max, 1.);

  for (unsigned int k=0; k<mass.size(); k++) {
    double sigma = interp_sigma(mass[k]);
    double dlnsigma = interp_dlnsigma(mass[k]);

    auto func = [&] (const double redshift) 
    {
      double SF = interp_SF(mass[k], redshift);
      double DD = (isDelta_vir) ? Delta_vir(Delta, redshift) : Delta;
      return SF*mass_function(mass[k], sigma, dlnsigma, redshift, model_MF, output_root, DD)*dV_dZdOmega(redshift, false);
    };

    MF[k] = gsl::GSL_integrate_qag(func, z_min, z_max)/VV; ///gsl::GSL_integrate_qag(funcD, z_min, z_max);  
  }

  return MF;
}


// =====================================================================================


vector<double> cosmobl::cosmology::Cosmology::redshift_distribution_haloes (const double z_min, const double z_max, const int step_z, const double Area_degrees, const double Mass_min, const double Mass_max, const string model_MF, const string method_SS, const string output_root, const double Delta, const bool isDelta_vir, const string interpType, const double k_max, const string input_file, const bool is_parameter_file) 
{

  // ---------- read the grid file ---------- 

  const string file_grid = create_grid_sigmaM(method_SS, 0., output_root,interpType, k_max, input_file, is_parameter_file);

  ifstream fin(file_grid.c_str()); checkIO(fin, file_grid); 
  
  double Mass, Sigma, Dln_Sigma;
  vector<double> mass, sigma, dlnsigma;
  while (fin >>Mass>>Sigma>>Dln_Sigma) {
    if (Mass_min<Mass && Mass<Mass_max) {
      mass.push_back(Mass);
      sigma.push_back(Sigma);
      dlnsigma.push_back(Dln_Sigma);
    }
  }
  fin.clear(); fin.close();

  glob::FuncGrid interp_sigma(mass, sigma, "Spline");
  glob::FuncGrid interp_dlnsigma(mass, dlnsigma, "Spline");

  // ---------- compute the number of haloes ---------- 

  vector<double> redshift_distribution(step_z, 0);
  double delta_z = (z_max-z_min)/step_z; 
  double redshift = z_min;

  for (int Red=0; Red<step_z; Red++) {

    vector<vector<double>> integration_limits(2);
    integration_limits[0] = {redshift, redshift+delta_z};
    integration_limits[1] = {Mass_min, Mass_max};

    auto integrand = [&] (const vector<double> parameters) 
    {
      double redshift = parameters[0];
      double mass = parameters[1];

      double DD = (isDelta_vir) ? Delta_vir(Delta, redshift) : Delta;
      double sigma = interp_sigma(mass);
      double dlnsigma = interp_dlnsigma(mass);
      return Area_degrees*dV_dZdOmega(redshift, false)*mass_function(mass, sigma, dlnsigma, redshift, model_MF, output_root, DD);
    };

    cosmobl::cuba::CUBAwrapper CubaIntegrand(integrand, 2);

    redshift_distribution[Red] = CubaIntegrand.IntegrateVegas(integration_limits);
    redshift += delta_z;
  }

  return redshift_distribution;
}


// =====================================================================================


vector<double> cosmobl::cosmology::Cosmology::redshift_distribution_haloes_selection_function (const double z_min, const double z_max, const int step_z, const double Area_degrees, const double Mass_min, const double Mass_max, const string model_MF, const string method_SS, const string selection_function_file, const vector<int> cols, const string output_root, const double Delta, const bool isDelta_vir, const string interpType, const double k_max, const string input_file, const bool is_parameter_file) 
{

  // ---------- read the grid file ---------- 

  const string file_grid = create_grid_sigmaM(method_SS, 0., output_root,interpType, k_max, input_file, is_parameter_file);

  ifstream fin(file_grid.c_str()); checkIO(fin, file_grid); 
  
  double Mass, Sigma, Dln_Sigma;
  vector<double> mass, sigma, dlnsigma;
  while (fin >>Mass>>Sigma>>Dln_Sigma) {
    if (Mass_min<Mass && Mass<Mass_max) {
      mass.push_back(Mass);
      sigma.push_back(Sigma);
      dlnsigma.push_back(Dln_Sigma);
    }
  }
  fin.clear(); fin.close();

  glob::FuncGrid interp_sigma(mass, sigma, "Spline");
  glob::FuncGrid interp_dlnsigma(mass, dlnsigma, "Spline");

  // ---------- read the selection function ---------- 

  vector<double> mass_SF, redshift_SF;
  vector<vector<double>> selection_function;

  read_matrix(selection_function_file, mass_SF, redshift_SF, selection_function, cols);
  //function<double(const double, const double)> interp_SF = bind(interpolated_2D, std::placeholders::_1, std::placeholders::_2, mass_SF, redshift_SF, selection_function, "Linear"); 

  glob::FuncGrid2D interp_SF( mass_SF, redshift_SF, selection_function, "Linear");

  // ---------- compute the number of haloes ---------- 

  vector<double> redshift_distribution(step_z, 0);
  double delta_z = (z_max-z_min)/(step_z); 
  double redshift = z_min;

  for (int Red=0; Red<step_z; Red++) {

    vector<vector<double>> integration_limits(2);
    integration_limits[0] = {redshift, redshift+delta_z};
    integration_limits[1] = {Mass_min, Mass_max};

    auto integrand = [&] (const vector<double> parameters) 
    {
      double redshift = parameters[0];
      double mass = parameters[1];

      double DD = (isDelta_vir) ? Delta_vir(Delta, redshift) : Delta;
      double sigma = interp_sigma(mass);
      double dlnsigma = interp_dlnsigma(mass);
      double SF = interp_SF(mass, redshift);
      return SF*Area_degrees*dV_dZdOmega(redshift, false)*mass_function(mass, sigma, dlnsigma, redshift, model_MF, output_root, DD);
    };

    cosmobl::cuba::CUBAwrapper CubaIntegrand(integrand, 2);

    redshift_distribution[Red] = CubaIntegrand.IntegrateVegas(integration_limits);
    redshift += delta_z;
  }

  return redshift_distribution;
}


// =====================================================================================


double cosmobl::cosmology::Cosmology::mean_redshift_haloes_selection_function (const double z_min, const double z_max, const double Mass_min, const double Mass_max, const string model_MF, const string method_SS, const string selection_function_file, const vector<int> cols, const string output_root, const double Delta, const bool isDelta_vir, const string interpType, const double k_max, const string input_file, const bool is_parameter_file) 
{

  // ---------- read the grid file ---------- 

  const string file_grid = create_grid_sigmaM(method_SS, 0., output_root,interpType, k_max, input_file, is_parameter_file);

  ifstream fin(file_grid.c_str()); checkIO(fin, file_grid); 
  
  double Mass, Sigma, Dln_Sigma;
  vector<double> mass, sigma, dlnsigma;
  while (fin >>Mass>>Sigma>>Dln_Sigma) {
    if (Mass_min<Mass && Mass<Mass_max) {
      mass.push_back(Mass);
      sigma.push_back(Sigma);
      dlnsigma.push_back(Dln_Sigma);
    }
  }
  fin.clear(); fin.close();

  glob::FuncGrid interp_sigma(mass, sigma, "Spline");
  glob::FuncGrid interp_dlnsigma(mass, dlnsigma, "Spline");

  // ---------- read the selection function ---------- 

  vector<double> mass_SF, redshift_SF;
  vector<vector<double>> selection_function;

  read_matrix(selection_function_file, mass_SF, redshift_SF, selection_function, cols);

  glob::FuncGrid2D interp_SF( mass_SF, redshift_SF, selection_function, "Linear");

  // ---------- compute the mean redshift ---------- 

  auto integrand_num = [&] (const vector<double> parameters) {
    double redshift = parameters[0];
    double mass = parameters[1];
    double sigma = interp_sigma(mass);
    double dlnsigma = interp_dlnsigma(mass);
    double SF = interp_SF(mass, redshift);

    double DD = (isDelta_vir) ? Delta_vir(Delta, redshift) : Delta;
    double MF = mass_function(mass, sigma, dlnsigma, redshift, model_MF, output_root, DD);
    return redshift*MF*SF*dV_dZdOmega(redshift, false);
  };

  auto integrand_denom = [&] (const vector<double> parameters) {

    double redshift = parameters[0];
    double mass = parameters[1];
    double sigma = interp_sigma(mass);
    double dlnsigma = interp_dlnsigma(mass);
    double SF = interp_SF(mass, redshift);

    double DD = (isDelta_vir) ? Delta_vir(Delta, redshift) : Delta;
    double MF = mass_function(mass, sigma, dlnsigma, redshift, model_MF, output_root, DD);
    return MF*SF*dV_dZdOmega(redshift, false);
  };

  cosmobl::cuba::CUBAwrapper IntegrandNum(integrand_num, 2);
  cosmobl::cuba::CUBAwrapper IntegrandDenom(integrand_denom, 2);

  vector<vector<double>> limits = {{z_min, z_max}, {Mass_min, Mass_max}};

  return IntegrandNum.IntegrateVegas(limits)/IntegrandDenom.IntegrateVegas(limits);

}


