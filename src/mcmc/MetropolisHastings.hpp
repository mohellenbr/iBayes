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

#ifndef MCMC_METROPOLISHASTINGS_HPP_
#define MCMC_METROPOLISHASTINGS_HPP_

#include <mcmc/MCMC.hpp>
#include <tools/Config.hpp>
#include <tools/Parallel.hpp>
#include <model/ForwardModel.hpp>

#include <string>
#include <vector>
#include <memory>
#include <random>
#include <chrono>
#include <cmath>

class MetropolisHastings : public MCMC
{
public:
	~MetropolisHastings() {}

	MetropolisHastings(
			Config const& c,
			Parallel & p,
			ForwardModel & m) : MCMC(c, p, m) {}

	void run(
			std::size_t num_samples,
			std::vector<double> const& init_samplepos = std::vector<double>()); // optional init vector
};
#endif /* MCMC_METROPOLISHASTINGS_HPP_ */
