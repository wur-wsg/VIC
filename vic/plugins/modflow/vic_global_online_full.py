#%%
import os 
import flopy
import numpy as np
import xarray as xr
import netCDF4 as nc
import subprocess  # for calling shell commands
from datetime import datetime, timedelta
from dateutil.relativedelta import relativedelta
import vic_runner as vr
#import support_function as sf
import config_module
from config_module import config_global_nat_foc
from config_module import config_global_nat_poc
from osgeo import gdal
#from netCDF4 import Dataset, date2num
import mf_run as mf
from time import time
#import multiprocessing
#%%

#%env LD_LIBRARY_PATH=/shared/legacyapps/netcdf/gcc/64/4.6.1/lib:$LD_LIBRARY_PATH   


def select_config(case_name, mode, coupling):
    config_lookup = {
        ('global', 'nat', 'foc'): config_module.config_global_nat_foc,
        ('global', 'nat', 'poc'): config_module.config_global_nat_poc,
        ('global', 'human', 'foc'): config_module.config_global_human_foc,
        ('global', 'human', 'poc'): config_module.config_global_human_poc,
    }
    if case_name == 'indus':
        return config_module.build_indus_config(
            humanimpact=(mode == 'human'),
            foc=(coupling == 'foc'),
        )
    try:
        return config_lookup[(case_name, mode, coupling)]
    except KeyError as exc:
        raise ValueError(
            f"Invalid combination of case={case_name}, mode={mode}, coupling={coupling}"
        ) from exc



def run_vic_modflow_coupled(start_date, end_date, config=None, vic_processes=8):
    """
    Run coupled VIC-MODFLOW simulation for a given time period
    
    Args:
        start_date (datetime): Start date of simulation
        end_date (datetime): End date of simulation
        config: Configuration object containing simulation parameters
    """
    # fallback to default config if not explicitly provided
    if config is None:
        config = config_module.config_global_nat_foc

    current_date = start_date

    # Loop over the dates
    while current_date <= end_date:
        loop_start_time = time()
        
        # Initialize extra forcing file for first timestep
        if current_date == start_date:
            vr.create_extra_forcing_file(current_date, config)

        print(f"Running VIC-MODFLOW coupled model for {current_date.strftime('%Y-%m')}", flush=True)

        # Run VIC
        print(f"Preparing VIC configuration for {current_date.strftime('%Y-%m')}", flush=True)
        config_file = vr.prepare_vic(current_date, config)    

        # Align with VIC output naming used by vic_runner/PostProcessVIC
        vic_output_file = os.path.join(
            config.paths.get_vic_result_dir(config.modestr, config.couplingstr),
            f'{config.modestr}_{config.couplingstr}_{config.vic_out_suffix}.{current_date.year}-{current_date.month:02d}.nc'
        )
        if not os.path.exists(vic_output_file):
            vr.run_vic(current_date, config, config_file, vic_processes)
            print(f"VIC simulation for {current_date.strftime('%Y-%m')} completed.", flush=True)
        else:
            print(f"VIC output file already exists for {current_date.strftime('%Y-%m')}. Skipping VIC run.", flush=True)

        # Process VIC output for MODFLOW input
        print(f"Processing VIC output for {current_date.strftime('%Y-%m')}", flush=True)
        ts_gwrecharge, ts_discharge = vr.PostProcessVIC(config, current_date)
        ts_gwabstract = vr.PostProcessVICPumping(config, current_date)
        
        # Run MODFLOW
        mfrun = mf.mfrun(config, current_date, ts_gwrecharge, ts_discharge, ts_gwabstract)
        mfrun.run_modflow() 
        print(f"MODFLOW simulation for {current_date.strftime('%Y-%m')} completed.", flush=True)

        # Process MODFLOW output and update files
        pp = mf.PostProcessMF(config, current_date)
        total_array = pp.get_baseflow_array()
        pp.update_vic_forcing(total_array)
        
        if config.foc:
            cpr_mm_month = pp.get_cpr_array()
            vr.update_statefile(current_date, config, cpr_mm_month)
            pp.export_cpr_to_nc(cpr_mm_month)
        
        # Calculate and print elapsed time
        loop_end_time = time()
        elapsed_seconds = loop_end_time - loop_start_time
        elapsed_hours = int(elapsed_seconds // 3600)
        elapsed_minutes = int((elapsed_seconds % 3600) // 60)
        print(f"Time elapsed for {current_date.strftime('%Y-%m')}: {elapsed_hours} hours {elapsed_minutes} minutes", flush=True)
        print(f"Completed processing for {current_date.strftime('%Y-%m')}", flush=True)

        # Advance to next month
        current_date += relativedelta(months=1)
        print(f"Moving to next time step: {current_date.strftime('%Y-%m')}", flush=True)


if __name__ == "__main__":
    import argparse

    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Run VIC-MODFLOW Coupled Model Simulation")
    # Named flags (preferred)
    parser.add_argument("--start-date", dest="start_date", type=str, help="Start date YYYY-MM-DD")
    parser.add_argument("--end-date", dest="end_date", type=str, help="End date YYYY-MM-DD")
    parser.add_argument("--case", dest="case_name", type=str, choices=["global", "indus"], help="Simulation case")
    parser.add_argument("--mode", dest="mode", type=str, choices=["nat", "human"], help="Simulation mode")
    parser.add_argument("--coupling", dest="coupling", type=str, choices=["foc", "poc"], help="Coupling type")
    parser.add_argument("--vic-out-suffix", dest="vic_out_suffix", type=str, help="Suffix after '<mode>_<coupling>_' for VIC OUTFILE")
    parser.add_argument(
        "--pumping-mode",
        dest="pumping_mode",
        choices=["off", "uncapped", "capped"],
        default="off",
        help="Groundwater pumping treatment for human-impact runs",
    )
    parser.add_argument(
        "--vic-processes",
        dest="vic_processes",
        type=int,
        default=8,
        help="Number of MPI processes used for each monthly VIC invocation",
    )
    # Positional (backward compatibility)
    parser.add_argument("pos_start_date", nargs="?", default="1979-01-01")
    parser.add_argument("pos_end_date", nargs="?", default="2010-01-01")
    parser.add_argument("pos_case_name", nargs="?", choices=["global", "indus"], default="global")
    parser.add_argument("pos_mode", nargs="?", choices=["nat", "human"], default="nat")
    parser.add_argument("pos_coupling", nargs="?", choices=["foc", "poc"], default="foc")
    parser.add_argument("pos_vic_out_suffix", nargs="?", default="5min_nogl")
    args = parser.parse_args()

    # Convert dates from string to datetime
    # Resolve values: prefer named flags, fall back to positional
    start_date_str = args.start_date or args.pos_start_date
    end_date_str = args.end_date or args.pos_end_date
    case_name = args.case_name or args.pos_case_name
    mode = args.mode or args.pos_mode
    coupling = args.coupling or args.pos_coupling
    vic_out_suffix = (args.vic_out_suffix or args.pos_vic_out_suffix)

    start_date = datetime.strptime(start_date_str, "%Y-%m-%d")
    end_date = datetime.strptime(end_date_str, "%Y-%m-%d")

    config = select_config(case_name, mode, coupling)
    config.set_pumping_mode(args.pumping_mode)
    if args.vic_processes <= 0:
        raise ValueError('--vic-processes must be positive')
    
    # Apply VIC output suffix from CLI
    config.set_vic_out_suffix(vic_out_suffix)

    print("\n" + "="*50, flush=True)
    print("=== VIC-MODFLOW Coupled Model Simulation ===", flush=True)
    print("="*50, flush=True)
    print("Simulation Period:", flush=True)
    print(f"Start: {start_date.strftime('%Y-%m-%d')}", flush=True)
    print(f"End:   {end_date.strftime('%Y-%m-%d')}", flush=True)
    print(f"Case:  {case_name}", flush=True)
    print(f"Mode:  {mode}", flush=True)
    print(f"Coupling: {coupling}", flush=True)
    print(f"VIC OUTFILE suffix: {vic_out_suffix}", flush=True)
    print(f"Pumping mode: {args.pumping_mode}", flush=True)
    print(f"VIC processes: {args.vic_processes}", flush=True)
    print("="*50 + "\n", flush=True)
    print(f"Starting simulation at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}", flush=True)

    try:
        run_vic_modflow_coupled(
            start_date,
            end_date,
            config,
            vic_processes=args.vic_processes,
        )
        print("Simulation completed successfully!")
    except Exception as e:
        print(f"Error during simulation: {str(e)}")
        raise
