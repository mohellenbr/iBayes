#include <tools/Config.hpp>
#include <tools/Parallel.hpp>
#include <tools/ErrorAnalysis.hpp>
#include <model/NS.hpp>
#include <mcmc/MCMC.hpp>
#include <mcmc/MetropolisHastings.hpp>
#include <mcmc/ParallelTempering.hpp>
#include <surrogate/SGI.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

const int ITERMAX = 10; // Cap the # of grid refinement (to prevent infinite loop)

int main(int argc, char** argv)
{
	Config cfg (argc, argv);
	Parallel par;
	par.mpi_init(argc, argv);

	/*************************
	 * Initialization Phase
	 *************************/
	double tic = MPI_Wtime();
	if (par.is_master()) {
		cfg.print_config(); // Show current config values
		printf("\nMain: BEGIN wall.time(sec) %.6f\n", MPI_Wtime()-tic);
	}
	// Forward model
	NS ns (cfg);
	// Surrogate model
	SGI sgi (cfg, par, ns);
	// Error analysis object
	ErrorAnalysis ea (cfg, par, ns, sgi);
	// Only Master need test points
	if (par.is_master()) {
		// Produce visualization with default obs locations (true locations)
		ns.sim();
		//ea.add_test_points(20);
		ea.add_test_points(20, cfg.get_param_string("ea_test_point_file"));
		ea.write_test_points(cfg.get_param_string("global_output_path") + "/test_points_r" + std::to_string(par.rank) + ".txt");
		ea.print_test_points();
	}
	MPI_Barrier(MPI_COMM_WORLD);

	/****************************************************
	 *	Surrogate Construction: multiple elasitc phases
	 ****************************************************/
	double tol = cfg.get_param_double("sgi_tol");
	for (int iter = 0; iter < ITERMAX; ++iter) {

		if (par.is_master()) {
			printf("\nMain: SGI phase %d | wall.time(sec) %.6f\n", iter, MPI_Wtime()-tic);
		}

		sgi.build();
		if (ea.eval_model_master(tol)) break;
	}
	MPI_Barrier(MPI_COMM_WORLD);

	/**************
	 * MCMC Phase
	 **************/
	if (par.is_master()) {
		printf("\nMain: MCMC phase wall.time(sec) %.6f\n", MPI_Wtime()-tic);
	}
	// MCMC
	MCMC* mcmc = new ParallelTempering(cfg, par, sgi);
	mcmc->run(cfg.get_param_sizet("mcmc_num_samples"), sgi.get_maxpos() );

	if (par.is_master()) {
		printf("\nMain: END wall.time(sec) %.6f\n", MPI_Wtime()-tic);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	par.mpi_final();
	return 0;
}
