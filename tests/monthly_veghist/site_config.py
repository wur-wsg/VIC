"""Site-specific facts for the monthly veg-history test suite.

Every machine-dependent value the suite needs is read from the environment,
which a site profile in `sites/` is responsible for setting.  Nothing here
carries a path to a particular machine, so running on a new cluster means
writing a new `sites/<name>.sh`, not editing Python.

    source sites/anunna.sh      # or sites/snellius.sh
"""
import os
import shlex
import subprocess
import sys


def python_interpreter():
    """Absolute interpreter for the analysis environment.

    Falls back to the interpreter currently running, which is correct when the
    suite was itself launched with the right one.  A bare `python3` is never
    used: on Anunna that resolves to the wrong conda environment.
    """
    return os.environ.get("VIC_TEST_PYTHON") or sys.executable


def mpi_command(exe, global_file, nranks=1):
    """Build the launcher command line for a VIC run.

    The launcher differs per site -- `mpirun -np` on Anunna, `srun -n` on
    Snellius -- so it is supplied as a prefix that is completed with the rank
    count here.
    """
    if nranks > 1:
        prefix = (os.environ.get("VIC_TEST_MPI_RUN_MULTI")
                  or os.environ.get("VIC_TEST_MPI_RUN")
                  or "mpirun -np")
    else:
        prefix = os.environ.get("VIC_TEST_MPI_RUN") or "mpirun -np"
    return shlex.split(prefix) + [str(nranks), exe, "-g", global_file]


def run_vic(exe, global_file, log_path, nranks=1, check=False):
    """Run VIC, capturing everything to log_path.  Returns (returncode, log)."""
    cmd = mpi_command(exe, global_file, nranks)
    with open(log_path, "w") as log:
        proc = subprocess.run(cmd, stdout=log, stderr=subprocess.STDOUT)
    text = open(log_path).read()
    if check and proc.returncode != 0:
        raise RuntimeError("VIC failed for %s (rc=%d); see %s"
                           % (global_file, proc.returncode, log_path))
    return proc.returncode, text


def describe():
    """Site facts actually in effect, for the run record."""
    return {
        "python": python_interpreter(),
        "mpi_run": os.environ.get("VIC_TEST_MPI_RUN", "mpirun -np (default)"),
        "mpi_run_multi": os.environ.get("VIC_TEST_MPI_RUN_MULTI",
                                        "(same as mpi_run)"),
        "ld_library_path": os.environ.get("LD_LIBRARY_PATH", ""),
        "template_domain": os.environ.get("VIC_TEST_TEMPLATE_DOMAIN", ""),
        "template_params": os.environ.get("VIC_TEST_TEMPLATE_PARAMS", ""),
    }
