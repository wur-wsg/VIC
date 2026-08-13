#!/bin/bash
#SBATCH --job-name=monthly_veghist_smoke
#SBATCH --output=monthly_veghist_smoke_%j.out
#SBATCH --error=monthly_veghist_smoke_%j.err
#SBATCH --time=01:00:00
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=1
#SBATCH --mem=8G
#
# Smoke test for the monthly vegetation-history forcing interface.
#
# Runs the whole suite: monthly-vs-daily equivalence, MPI 1-vs-4 rank, restart
# continuity, malformed-input error paths, and optionally a backward
# compatibility check against a baseline binary.
#
# Every machine-specific value comes from a site profile in sites/; nothing
# below names a particular cluster.
#
#   VIC_TEST_SITE     which sites/<name>.sh to source   (default: anunna)
#   VIC_EXE           the binary under test             (required)
#   BASELINE_EXE      optional pre-change binary
#   SKIP_CASE         set to 1 to reuse an existing case (cross-site runs)
#
# Submit with:
#   VIC_TEST_SITE=snellius VIC_EXE=/path/to/vic_image.exe sbatch smoke_monthly_veghist.slurm
#
# Anunna note: if the local Slurm client is unavailable, submit through
#   ssh -o BatchMode=yes login201 /shared/apps/slurm/24.11.6/bin/sbatch <path>
#
# Snellius note: the budget only covers cbuild, fat_genoa, fat_rome and
# staging, so add e.g. `#SBATCH --partition=fat_genoa --account=lwsei16647`
# or pass them on the command line. This case is tiny; it is a correctness
# check, not a benchmark.

set -euo pipefail

# Locating ourselves is not as simple as dirname "$0": sbatch copies the script
# into a node-local spool directory before running it, so BASH_SOURCE points at
# the copy and the sites/ directory is not next to it. Prefer an explicit
# override, fall back to BASH_SOURCE when it really is the checkout, and ask
# Slurm for the submitted path otherwise.
if [ -n "${VIC_TEST_DIR:-}" ]; then
    TEST_DIR="${VIC_TEST_DIR}"
elif [ -d "$(dirname "${BASH_SOURCE[0]}")/sites" ]; then
    TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
elif [ -n "${SLURM_JOB_ID:-}" ]; then
    SUBMITTED=$(scontrol show job "${SLURM_JOB_ID}" \
                | tr ' ' '\n' | awk -F= '/^Command=/{print $2}')
    TEST_DIR="$(cd "$(dirname "${SUBMITTED}")" && pwd)"
else
    echo "Cannot locate the test directory; set VIC_TEST_DIR." >&2
    exit 1
fi

SITE="${VIC_TEST_SITE:-anunna}"
SITE_FILE="${TEST_DIR}/sites/${SITE}.sh"

if [ ! -f "${SITE_FILE}" ]; then
    echo "No site profile ${SITE_FILE}" >&2
    echo "Available: $(ls "${TEST_DIR}/sites/" | tr '\n' ' ')" >&2
    exit 1
fi

# shellcheck source=/dev/null
source "${SITE_FILE}"

if [ -z "${VIC_EXE:-}" ]; then
    echo "VIC_EXE is not set. Point it at the vic_image.exe under test." >&2
    exit 1
fi

if [ ! -x "${VIC_EXE}" ]; then
    echo "VIC executable not found or not executable: ${VIC_EXE}" >&2
    echo "Build it first:  cd <VIC_ROOT>/vic/drivers/image && make model" >&2
    exit 1
fi

WORKDIR="${VIC_TEST_WORKDIR}/suite"
mkdir -p "${WORKDIR}"

ARGS=(--workdir "${WORKDIR}"
      --exe "${VIC_EXE}"
      --json "${WORKDIR}/monthly_veghist_summary.json")

if [ -n "${BASELINE_EXE:-}" ]; then
    ARGS+=(--baseline-exe "${BASELINE_EXE}")
fi

if [ "${SKIP_CASE:-0}" = "1" ]; then
    ARGS+=(--skip-case)
fi

echo "site      : ${SITE}"
echo "python    : ${VIC_TEST_PYTHON}"
echo "launcher  : ${VIC_TEST_MPI_RUN}"
echo "vic       : ${VIC_EXE}"
echo "workdir   : ${WORKDIR}"
echo "started   : $(date)"

"${VIC_TEST_PYTHON}" "${TEST_DIR}/run_all.py" "${ARGS[@]}"
STATUS=$?

echo "summary written to ${WORKDIR}/monthly_veghist_summary.json"
exit ${STATUS}
