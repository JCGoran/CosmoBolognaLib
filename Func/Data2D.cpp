/********************************************************************
 *  Copyright (C) 2014 by Federico Marulli and Alfonso Veropalumbo  *
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
 *  @file Func/Data2D.cpp
 *
 *  @brief Methods of the class Data2D
 *
 *  This file contains the implementation of the methods of the class
 *  Data2D
 *
 *  @authors Federico Marulli, Alfonso Veropalumbo
 *
 *  @authors federico.marulli3@unbo.it, alfonso.veropalumbo@unibo.it
 */

#include "Data2D.h"

using namespace cosmobl;
using namespace data;
using namespace glob;


// ======================================================================================


cosmobl::data::Data2D::Data2D (const vector<double> x, const vector<double> y, const vector<vector<double> > fxy, const double xmin, const double xmax, const double ymin, const double ymax, const DataType dataType) : Data(dataType)
{
  m_x = x;
  m_y = y;
  m_fxy = fxy;

  find_index(m_x, xmin, xmax, m_x_down, m_x_up);
  find_index(m_y, ymin, ymax, m_y_down, m_y_up);
}

// ======================================================================================


cosmobl::data::Data2D::Data2D (const vector<double> x, const vector<double> y, const vector<vector<double> > fxy, const vector<vector<double> > error_fxy, const double xmin, const double xmax, const double ymin, const double ymax, const DataType dataType) : Data(dataType)
{
  m_x = x;
  m_y = y;
  m_fxy = fxy;
  m_error_fxy = error_fxy;

  find_index(m_x, xmin, xmax, m_x_down, m_x_up);
  find_index(m_y, ymin, ymax, m_y_down, m_y_up);
}


// ======================================================================================


void cosmobl::data::Data2D::set_limits (const double min, const double max, const bool axis)
{
  if (axis ==0) 
    find_index(m_x, min, max, m_x_down, m_x_up);
  
  else if (axis == 1)
    find_index(m_y, min, max, m_y_down, m_y_up);
  
  else
    ErrorCBL("Error in set_limits of Data2D, unrecognized option for axis");
}


// ======================================================================================


void cosmobl::data::Data2D::set_limits (const double xmin, const double xmax, const double ymin, const double ymax)
{
  set_limits(xmin, xmax, 0);
  set_limits(ymin, ymax, 1);
}


// ======================================================================================


double cosmobl::data::Data2D::covariance (const int i1, const int i2, const int j1, const int j2) const
{
  int i = i2+i1*m_x.size();
  int j = j2+j1*m_y.size();

  ErrorCBL("Work in progress in covariance of Data2D::covariance : work in progress!", ExitCode::_workInProgress_);
  
  return m_covariance_fxy[i][j];
}


// ======================================================================================


void cosmobl::data::Data2D::read (const string input_file, const int skipped_lines)
{
  (void)skipped_lines;
  
  ErrorCBL("Error in cosmobl::data::Data2D::read : work in progress!", ExitCode::_workInProgress_);

  ifstream fin(input_file.c_str()); checkIO(fin, input_file);
  
  fin.clear(); fin.close();
}


// ======================================================================================


void cosmobl::data::Data2D::write (const string dir, const string file, const string header, const bool full, const int rank) const 
{
  (void)rank;
  
  string file_out = dir+file;
  ofstream fout(file_out.c_str()); checkIO(fout, file_out);

  fout << "### " << header << " ###" << endl;

  for (size_t i=0; i<m_x.size(); ++i)
    for (size_t j=0; j<m_y.size(); ++j) 
      fout << setiosflags(ios::fixed) << setprecision(4) << setw(8) << m_x[i] << "  " << setw(8) << m_y[j] << "  " << setw(8) << m_fxy[i][j] << "  " << setw(8) << m_error_fxy[i][j] << endl;

 
  if (full) { // duplicate the information in the other 3 quadrants

    for (size_t i=0; i<m_x.size(); ++i)
      for (size_t j=0; j<m_y.size(); ++j) 
	fout << setiosflags(ios::fixed) << setprecision(4) << setw(8) << m_x[i] << "  " << setw(8) << -m_y[j] << "  " << setw(8) << m_fxy[i][j] << "  " << setw(8) << m_error_fxy[i][j] << endl;

    for (size_t i=0; i<m_x.size(); ++i)
      for (size_t j=0; j<m_y.size(); ++j) 
	fout << setiosflags(ios::fixed) << setprecision(4) << setw(8) << -m_x[i] << "  " << setw(8) << -m_y[j] << "  " << setw(8) << m_fxy[i][j] << "  " << setw(8) << m_error_fxy[i][j] << endl;

    for (size_t i=0; i<m_x.size(); ++i)
      for (size_t j=0; j<m_y.size(); ++j) 
	fout << setiosflags(ios::fixed) << setprecision(4) << setw(8) << -m_x[i] << "  " << setw(8) << m_y[j] << "  " << setw(8) << m_fxy[i][j] << "  " << setw(8) << m_error_fxy[i][j] << endl;
    
  }

  fout.close(); cout << endl; coutCBL << "I wrote the file: " << file_out << endl << endl;

}
