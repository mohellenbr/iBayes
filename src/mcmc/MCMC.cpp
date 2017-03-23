// iBayes - Elastic Bayesian Inference Framework with Sparse Grid
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

#include <mcmc/MCMC.hpp>

using namespace std;

MCMC::MCMC(
		ForwardModel* forwardmodel,
		const string& observed_data_file,
		double rand_walk_size_domain_percent)
{
	this->model = forwardmodel;
	this->input_size = model->get_input_size();
	this->output_size = model->get_output_size();

	this->rand_walk_size.reset(new double[input_size]);
	double dmin, dmax;
	for (size_t i=0; i < input_size; i++) {
		model->get_input_space(i, dmin, dmax);
		rand_walk_size[i] = (dmax-dmin) * rand_walk_size_domain_percent;
	}

	// Get observed data and noise
	this->observed_data.reset(
			ForwardModel::get_observed_data(observed_data_file, output_size,
					this->observed_data_noise));

	// Compute posterior sigma
	this->pos_sigma = ForwardModel::compute_posterior_sigma(
			this->observed_data.get(), output_size, observed_data_noise);
}


double* MCMC::gen_random_sample()
{
	double* sample = new double[input_size];
	mt19937 gen (chrono::system_clock::now().time_since_epoch().count());
	double dmin, dmax;
	for (size_t i=0; i < input_size; i++) {
		model->get_input_space(i, dmin, dmax);
		uniform_real_distribution<double> udist(dmin,dmax);
		sample[i] = udist(gen);
	}
	return sample;
}


bool MCMC::read_last_sample_pos(
		fstream& fin,
		double* point,
		double& pos)
{
	fin.seekg(0, fin.end);
	size_t len = fin.tellg(); // Get length of file

	// Loop backwards
	char c;
	string line;
	for (long int i=len-2; i > 0; i--) {
		fin.seekg(i);
		c = fin.get();
		if (c == '\r' || c == '\n') {
			std::getline(fin, line);
			istringstream iss(line);
			vector<string> tokens {istream_iterator<string>{iss}, istream_iterator<string>{}};
			size_t num = tokens.size();

			if (num <= 0) {// skip empty line
				continue;
			} else {
				for (int k=0; k < num-1; k++) {
					point[k] = stod(tokens[k]);
				}
				pos = stod(tokens[num-1]); // last token is pos
				fin.seekg(0, fin.end);
				return true;
			} //end if ntokens
		}
	}//end for
	fin.seekg(0, fin.end);
	return false;
}


void MCMC::write_sample_pos(
		fstream& fout,
		const double* point,
		double pos)
{
	for (size_t i=0; i < input_size; i++) {
		fout << point[i] << " ";
	}
	fout << pos << endl;
	return;
}


void MCMC::one_step_single_dim(
		int dim,
		double& pos,
		double* p,
		double* d)
{
	double pos_tmp = 0.0;
	double dmin, dmax;
	double p_dim_old = p[dim]; /// When reject a proposal, we need ot restore p[dim];

	// Initialize random generators
	mt19937 gen(chrono::system_clock::now().time_since_epoch().count());
	normal_distribution<double> ndist(p[dim], rand_walk_size[dim]);
	uniform_real_distribution<double> udist(0.0,1.0);

	// 1. Draw a proposal (we only update p[dim])
	p[dim] = ndist(gen);
	model->get_input_space(dim, dmin, dmax);
	while ((p[dim] < dmin) || (p[dim] > dmax)) /// ensure p[dim] is in range
		p[dim] = ndist(gen);

	// 2. Compute acceptance rate
	model->run(p, d);
	pos_tmp = ForwardModel::compute_posterior(observed_data.get(), d, output_size, pos_sigma);
	double acc = fmin(1.0, pos_tmp/pos);

	// 3. Accept or reject proposal
	if (udist(gen) <= acc) {
		// Case accept:
		// p is already updated, only update pos
		pos = pos_tmp;
	} else {
		// Case reject:
		// restore p[dim], pos stays the same
		p[dim] = p_dim_old;
	}
	return;
}












