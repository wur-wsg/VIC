# Site facts for Anunna (WUR).
#
# Source this before running the monthly veg-history test suite:
#     source sites/anunna.sh
#
# Every machine-specific fact used by the suite lives here. The Python scripts
# read them from the environment and have no built-in paths, so running on a
# new machine means writing a file like this one rather than editing code.

# Absolute interpreter. Never a bare `python3` here: Anunna's .bashrc activates
# the nco_env conda environment, so `python3` on PATH is 3.9 and not the
# intended analysis environment.
export VIC_TEST_PYTHON=/home/WUR/liu297/miniconda3/envs/vic_global/bin/python

# The netCDF runtime the VIC binary was linked against. Without this the binary
# fails with "error while loading shared libraries: libnetcdf.so.19".
export LD_LIBRARY_PATH=/home/WUR/liu297/miniconda3/envs/nco_env/lib:${LD_LIBRARY_PATH:-}

# MPI launcher, given as a prefix that is completed with the rank count.
# Anunna runs MPICH 3.1.3.
export VIC_TEST_MPI_RUN="mpirun -np"
# Used for the multi-rank consistency check. On a login node there are usually
# fewer free slots than ranks, hence --oversubscribe.
export VIC_TEST_MPI_RUN_MULTI="mpirun --oversubscribe -np"

# Known-good full-size inputs, used read-only as a structural template when
# building the synthetic case. These exist on Anunna only; on any other site,
# copy an already-built case across instead of regenerating it (the case is
# data, and data moves by file transfer).
export VIC_TEST_TEMPLATE_DOMAIN=/lustre/nobackup/WUR/ESG/liu297/vic_coupled/Data/VIC/domain/indus/domain_Indus.nc
export VIC_TEST_TEMPLATE_PARAMS=/lustre/nobackup/WUR/ESG/liu297/vic_coupled/Data/VIC/parameter/indus/VIC_params_Modis_calibrated_Indus.nc

# Where test output goes. Not backed up; safe to delete.
export VIC_TEST_WORKDIR=/lustre/nobackup/WUR/ESG/liu297/vic_coupled/Simulation/monthly_veghist_test
