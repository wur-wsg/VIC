#!/bin/bash
#SBATCH --job-name=monthly_veghist_smoke
#SBATCH --output=/lustre/nobackup/WUR/ESG/liu297/vic_coupled/Simulation/monthly_veghist_test/slurm_%j.out
#SBATCH --error=/lustre/nobackup/WUR/ESG/liu297/vic_coupled/Simulation/monthly_veghist_test/slurm_%j.err
#SBATCH --time=01:00:00
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=1
#SBATCH --mem=8G
#
# Smoke test for the monthly vegetation-history forcing interface.
#
# Runs the whole suite: monthly-vs-daily equivalence, MPI 1-vs-4 rank,
# restart continuity, malformed-input error paths, and (optionally)
# a backward-compatibility check against a baseline binary.
#
# Submit with:
#   sbatch smoke_monthly_veghist.slurm
# or, if the local Slurm client is unavailable:
#   ssh -o BatchMode=yes login201 /shared/apps/slurm/24.11.6/bin/sbatch <path>

set -euo pipefail

VIC_WORKTREE=/lustre/nobackup/WUR/ESG/liu297/vic_coupled/VIC_monthly_veghist
TEST_DIR="${VIC_WORKTREE}/tests/monthly_veghist"
WORKDIR=/lustre/nobackup/WUR/ESG/liu297/vic_coupled/Simulation/monthly_veghist_test

VIC_EXE="${VIC_WORKTREE}/vic/drivers/image/vic_image.exe"
PYTHON_EXE=/home/WUR/liu297/miniconda3/envs/vic_global/bin/python

# netCDF runtime used by the build; without this the binary cannot find
# libnetcdf.so.19.
export LD_LIBRARY_PATH=/home/WUR/liu297/miniconda3/envs/nco_env/lib:${LD_LIBRARY_PATH:-}

# Optional: a binary built from the pre-change commit, used to prove that the
# legacy three-token FORCE_TYPE layout still produces identical results.
BASELINE_EXE="${BASELINE_EXE:-}"

mkdir -p "${WORKDIR}"

if [ ! -x "${VIC_EXE}" ]; then
    echo "VIC executable not found: ${VIC_EXE}" >&2
    echo "Build it first:  cd ${VIC_WORKTREE}/vic/drivers/image && make model" >&2
    exit 1
fi

ARGS=(--workdir "${WORKDIR}"
      --exe "${VIC_EXE}"
      --json "${WORKDIR}/monthly_veghist_summary.json")

if [ -n "${BASELINE_EXE}" ]; then
    ARGS+=(--baseline-exe "${BASELINE_EXE}")
fi

echo "Running monthly veg-history test suite at $(date)"
"${PYTHON_EXE}" "${TEST_DIR}/run_all.py" "${ARGS[@]}"
STATUS=$?

echo "Summary written to ${WORKDIR}/monthly_veghist_summary.json"
exit ${STATUS}
