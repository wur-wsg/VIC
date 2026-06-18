# Variable Infiltration Capacity (VIC-WUR) Model

[![Documentation](https://readthedocs.org/projects/vicwur/badge/?version=latest)](http://vicwur.readthedocs.org/en/latest/) [![License](https://img.shields.io/badge/license-GPLv2-blue.svg)](https://raw.githubusercontent.com/UW-Hydro/VIC/master/LICENSE.txt) [![DOI](https://zenodo.org/badge/7766/BramDr/VIC.svg)](https://zenodo.org/badge/latestdoi/7766/BramDr/VIC)

----------

!!! note
    You are viewing the documentation for VIC-WUR. VIC-WUR extends the VIC model with several modules to simulate the human impact on global water resources and crop growth (using WOFOST). Only VIC-WUR described here, for further descriptions of VIC you are referred to the [VIC github page of UW-Hydro](https://github.com/UW-Hydro/VIC)
    
VIC-WUR is a macroscale hydrologic model that simulates the human impact on global water resources and crop growth (using WOFOST). VIC-WUR is based on VIC-5, which solves full water and energy balances. 

VIC-WUR is a research model and has been applied globally. The VIC model is distributed under the [GNU GPL v2.0](http://www.gnu.org/licenses/gpl-2.0.html) license. If you make use of this model, please acknowledge the appropriate references listed on the [VIC-WUR documentation website](http://vicwur.readthedocs.org).

Development and maintenance of the current official version of the VIC-WUR model is led by the [Water Systems and Global Change group | Departement of Environmental sciences | Wageningen University](https://www.wur.nl/en/Research-Results/Chair-groups/Environmental-Sciences/Water-Systems-and-Global-Change-Group.htm). Every new application addresses new problems and conditions that the model may not currently be able to handle, and as such the model is always under development. The VIC-WUR model is an open source development project, which means that contributions are welcome, including to the VIC-WUR documentation.

All development activity is coordinated via the [VIC-WUR github page](https://github.com/wur-wsg/VIC), where you can also find all archived, current, beta, and development versions of the model.

-----

## VIC-WUR-MODFLOW simulation

This branch includes an experimental VIC-WUR-MODFLOW coupling workflow in
`vic/plugins/modflow`. The main entry point is
`vic/plugins/modflow/vic_global_online_full.py`, which runs the coupled model
month by month. For each monthly step, the workflow prepares the VIC global
parameter file, runs the VIC image driver, post-processes VIC groundwater
recharge and discharge for MODFLOW 6, runs MODFLOW through FloPy, and writes
MODFLOW baseflow back to the VIC extra forcing file for the next time step. In
FOC runs, capillary rise from MODFLOW is also written back to the VIC state
file.

The default directory layout assumes the coupled project root is
`/lustre/nobackup/WUR/ESG/liu297/vic_coupled`, with static input data under
`Data` and simulation output under `Simulation`. These defaults can be
overridden with environment variables such as `VIC_COUPLED_PROJECT_ROOT`,
`VIC_EXE`, `MF6_EXE`, `VIC_MF_DATA_ROOT`, `VIC_MF_SIMULATION_ROOT`,
`VIC_MF_RESULT_DIR`, and `VIC_MF_STATE_DIR`.

Example run:

```bash
cd /lustre/nobackup/WUR/ESG/liu297/vic_coupled/VIC
python vic/plugins/modflow/vic_global_online_full.py \
  --start-date 1979-01-01 \
  --end-date 1979-12-01 \
  --mode nat \
  --coupling foc \
  --vic-out-suffix 5min_nogl
```

Supported modes are `nat` and `human`; supported coupling types are `foc` and
`poc`. The VIC executable defaults to
`vic/drivers/image/vic_image_coupled.exe`, and the MODFLOW 6 executable defaults
to `Data/MODFLOW/executable/mf6` unless `VIC_EXE` or `MF6_EXE` is set.
