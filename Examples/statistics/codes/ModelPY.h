// ====================================================================================
// Example code: write a C++ function and create a class inheriting from Model1D object
// ====================================================================================

#include "Cosmology.h"
#include "Posterior.h"

using namespace std;

std::string cbl::par::DirCosmo;
std::string cbl::par::DirLoc;

// this example model has 4 parameters: A, B, C, D; A and B are free parameters, C is fixed, D is a derived parameter
vector<double> model_function (const vector<double> x, const shared_ptr<void> modelInput, std::vector<double> &parameter)
{
  // the object Cosmology, used in this example to compute Omega_matter
  cbl::cosmology::Cosmology cosm = *static_pointer_cast<cbl::cosmology::Cosmology>(modelInput);

  vector<double> model(x.size(), 0.);
  for (size_t i=0; i<x.size(); ++i)
    model[i] = parameter[0]*x[i]+parameter[1]+parameter[2]*cosm.Omega_matter(); // the model

  parameter[3] = parameter[0]+parameter[1]+parameter[2]; // parameter[3] is a derived parameter

  return model;
}


// Use this function to pass all the model inputs, in this case an object of class cbl::cosmology::Cosmology
shared_ptr<cbl::statistics::Model1D> getModel1D (const cbl::cosmology::Cosmology cosmology) 
{
    // number of model parameters
    const int nparameters = 4;

    // names of the model parameters
    const vector<string> parNames = {"A", "B", "C", "D"};

    // vector containing the 4 model parameters
    vector<cbl::statistics::ParameterType> parType(nparameters-1, cbl::statistics::ParameterType::_Base_);
    parType.emplace_back(cbl::statistics::ParameterType::_Derived_);   

    // set the stuff used to construct the model: here an object of class cosmology, just as an example 
    auto ptr_modelInput = make_shared<cbl::cosmology::Cosmology>(cosmology);

    // construct the model
    const cbl::statistics::Model1D model(&model_function, nparameters, parType, parNames, ptr_modelInput);
    return make_shared<cbl::statistics::Model1D>(model);
}
