# Site facts for Snellius (SURF).
#
# Source this before running the monthly veg-history test suite:
#     source sites/snellius.sh
#
# Values follow vic-infra `sites/snellius.yaml`. If the two disagree, that file
# is authoritative -- it is the one that gets verified against the machine.
#
# NOTE: Snellius runs OpenMPI 5.0.3 where Anunna runs MPICH 3.1.3. That is a
# change of MPI implementation, not a version bump, so results from this suite
# are worth comparing across the two sites rather than assumed to match.

# Absolute interpreter. Snellius deliberately runs no `conda init`, so nothing
# is activated in a login shell and the absolute path is the only interface.
export VIC_TEST_PYTHON=/gpfs/home2/sliu/miniforge3/envs/vic_global/bin/python

# netCDF comes from modules here rather than from a conda environment, so
# LD_LIBRARY_PATH is set by the module system and must not be hand-rolled.
module load 2024
module load foss/2024a netCDF/4.9.2-gompi-2024a netCDF-Fortran/4.6.1-gompi-2024a

# MPI launcher. srun takes -n where mpirun takes -np.
#
# srun is the production launcher, but it needs an allocation: on a login node
# it would either queue or fail. This case is tiny and costs nothing to run
# interactively, and a correctness check on a login node is worth having
# without spending SBU, so fall back to mpirun when there is no job around us.
if [ -n "${SLURM_JOB_ID:-}" ]; then
    export VIC_TEST_MPI_RUN="srun -n"
    export VIC_TEST_MPI_RUN_MULTI="srun -n"
else
    export VIC_TEST_MPI_RUN="mpirun -np"
    export VIC_TEST_MPI_RUN_MULTI="mpirun --oversubscribe -np"
fi

# No template inputs here on purpose. The synthetic case is built on Anunna
# from its Indus parameter set and copied across as data; regenerating it from
# a different template would defeat the point of comparing the two sites on an
# identical case. Run with --skip-case and point --workdir at the copy.
unset VIC_TEST_TEMPLATE_DOMAIN
unset VIC_TEST_TEMPLATE_PARAMS

# Project space, not scratch: scratch is purged after 14 days and this case is
# small enough that the quota is irrelevant.
export VIC_TEST_WORKDIR=/projects/0/prjs2087/vic/simulations/monthly_veghist_test
