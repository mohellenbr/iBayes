// eBayes - Elastic Bayesian Inference Framework with iMPI
// Copyright (C) 2015-today Ao Mo-Hellenbrand
//
// All copyrights remain with the respective authors.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef TOOLS_ERRORANALYSIS_HPP_
#define TOOLS_ERRORANALYSIS_HPP_

#include <tools/Config.hpp>
#include <tools/Parallel.hpp>
#include <model/ForwardModel.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip> //for std::setprecision
#include <vector>
#include <random>
#include <cmath> //for isinf() isnan()
#include <cstdlib> //for atof()


class ErrorAnalysis {
private:
	Config const&	cfg;
	Parallel&		par;
	ForwardModel&	fullmodel;
	ForwardModel&	surrogate;

	std::vector< std::vector<double> >	test_points;
	std::vector< std::vector<double> >	test_points_data;

public:
	~ErrorAnalysis() {}

	ErrorAnalysis(
			Config const& c,
			Parallel& p,
			ForwardModel& f,
			ForwardModel& s)
		: cfg(c), par(p), fullmodel(f), surrogate(s) {}

	void add_test_points(std::size_t n, std::string test_point_file="");

	void add_test_point_at(std::vector<double> const& m);

	void copy_test_points(ErrorAnalysis const& that);

	void read_test_points(std::string infile);

	void write_test_points(std::string outfile);

	void print_test_points();

	std::string vec_to_string(std::vector<double> const& v);

	double compute_surrogate_error();

	double compute_surrogate_error_at(std::vector<double> const& m);

	bool eval_model_master(double tol);
	bool eval_model_spmd(double tol);
};
#endif /* TOOLS_ERRORANALYSIS_HPP_ */
