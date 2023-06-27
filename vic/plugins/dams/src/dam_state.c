/******************************************************************************
 * @section DESCRIPTION
 *
 * Dam state functions
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#include <vic_driver_image.h>
#include <plugin.h>


/******************************************
* @brief   Set state metadata
******************************************/
void
dam_set_state_meta_data_info(void)
{
    extern metadata_struct state_metadata[];

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].varname,
             MAXSTRING, "%s", "STATE_DAM_STORAGE");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].long_name,
             MAXSTRING, "%s", "dam storage");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].standard_name,
             MAXSTRING, "%s", "dam storage");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].units, MAXSTRING,
             "%s", "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].description,
             MAXSTRING, "%s", "storage in dam/reservoir");

    
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_INFLOW].varname,
             MAXSTRING, "%s", "STATE_DAM_INFLOW");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_INFLOW].long_name,
             MAXSTRING, "%s", "total inflow");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_INFLOW].standard_name,
             MAXSTRING, "%s", "total inflow");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_INFLOW].units, MAXSTRING,
             "%s", "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_INFLOW].description,
             MAXSTRING, "%s", "accumulated inflow over accumulation steps.");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_DEMAND].varname,
             MAXSTRING, "%s", "STATE_DAM_DEMAND");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_DEMAND].long_name,
             MAXSTRING, "%s", "total demand");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_DEMAND].standard_name,
             MAXSTRING, "%s", "total demand");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_DEMAND].units, MAXSTRING,
             "%s", "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_DEMAND].description,
             MAXSTRING, "%s", "accumulated demand over accumulation steps.");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_EFR].varname,
             MAXSTRING, "%s", "STATE_DAM_EFR");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_EFR].long_name,
             MAXSTRING, "%s", "total eft");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_EFR].standard_name,
             MAXSTRING, "%s", "total eft");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_EFR].units, MAXSTRING,
             "%s", "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_EFR].description,
             MAXSTRING, "%s", "accumulated EFR over accumulation steps.");
    
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS].varname,
             MAXSTRING, "%s", "STATE_DAM_MONTHS");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS].long_name,
             MAXSTRING, "%s", "months");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS].standard_name,
             MAXSTRING, "%s", "months");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS].units, MAXSTRING,
             "%s", "1");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS].description,
             MAXSTRING, "%s", "number of months running during simulation period.");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STEPS].varname,
             MAXSTRING, "%s", "STATE_DAM_STEPS");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STEPS].long_name,
             MAXSTRING, "%s", "steps");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STEPS].standard_name,
             MAXSTRING, "%s", "steps");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STEPS].units, MAXSTRING,
             "%s", "1");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STEPS].description,
             MAXSTRING, "%s", "accumulation steps of the current month");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].varname,
             MAXSTRING, "%s", "STATE_DAM_OP_MONTH");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].long_name,
             MAXSTRING, "%s", "operational month");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].standard_name,
             MAXSTRING, "%s", "operational_month");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].units, MAXSTRING,
             "%s", "1");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].description,
             MAXSTRING, "%s", "start of dam operational year [month]");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].varname,
             MAXSTRING, "%s", "STATE_DAM_OP_RELEASE");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].long_name,
             MAXSTRING, "%s", "operational release");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].standard_name,
             MAXSTRING, "%s", "operational_release");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].units, MAXSTRING,
             "%s", "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].description,
             MAXSTRING, "%s", "operational release");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].varname,
             MAXSTRING, "%s", "STATE_DAM_OP_STORAGE");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].long_name,
             MAXSTRING, "%s", "operational storage");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].standard_name,
             MAXSTRING, "%s", "operational_storage");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].units, MAXSTRING,
             "%s", "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].description,
             MAXSTRING, "%s", "operational release");
    
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].varname,
             MAXSTRING, "%s", "STATE_DAM_HISTORY_INFLOW");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].long_name,
             MAXSTRING, "%s", "historical inflow");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].standard_name,
             MAXSTRING, "%s", "historical_inflow");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].units, MAXSTRING,
             "%s", "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].description,
             MAXSTRING, "%s", "historical inflow");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].varname,
             MAXSTRING, "%s", "STATE_DAM_HISTORY_DEMAND");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].long_name,
             MAXSTRING, "%s", "historical demand");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].standard_name,
             MAXSTRING, "%s", "historical_demand");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].units, MAXSTRING,
             "%s", "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].description,
             MAXSTRING, "%s", "historical demand");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].varname,
             MAXSTRING, "%s", "STATE_DAM_HISTORY_EFR");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].long_name,
             MAXSTRING, "%s", "historical efr");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].standard_name,
             MAXSTRING, "%s", "historical_efr");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].units, MAXSTRING,
             "%s", "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].description,
             MAXSTRING, "%s", "historical environmental flow requirement");

}

/******************************************
* @brief   Initialize state dimension size and ids
******************************************/
void
dam_set_nc_state_file_info(nc_file_struct *nc_state_file)
{
    extern plugin_option_struct       plugin_options;
    // dam classes
    nc_state_file->dam_dimid = MISSING;
    nc_state_file->dam_size = plugin_options.NDAMTYPES;
    // dam history
    nc_state_file->hist_dimid = MISSING;
    nc_state_file->hist_size = MONTHS_PER_YEAR * DAM_HIST_YEARS;
    // months
    nc_state_file->month_dimid = MISSING;
    nc_state_file->month_size = MONTHS_PER_YEAR;

}

/******************************************
* @brief   Add state dimensions
******************************************/
void
dam_add_state_dim(char           *filename,
                   nc_file_struct *nc_state_file)
{
    int status;

    // Define dimension dam class
    status = nc_def_dim(nc_state_file->nc_id, "dam_class",
                        nc_state_file->dam_size,
                        &(nc_state_file->dam_dimid));
    check_nc_status(status, "Error defining dam_class in %s", filename);

    // Define dimension history
    status = nc_def_dim(nc_state_file->nc_id, "history",
                        nc_state_file->hist_size,
                        &(nc_state_file->hist_dimid));
    check_nc_status(status, "Error defining history in %s", filename);

    // Define dimension month
    status = nc_def_dim(nc_state_file->nc_id, "month",
                        nc_state_file->month_size,
                        &(nc_state_file->month_dimid));
    check_nc_status(status, "Error defining month in %s", filename);
}

/******************************************
* @brief   Add state dimension variables
******************************************/
void
dam_add_state_dim_var(char           *filename,
                       nc_file_struct *nc_state_file)
{
    int dimids[MAXDIMS];
    int var_id;
    int status;

    // Initialize dam_class
    dimids[0] = nc_state_file->dam_dimid;

    // Define dimension variable
    status = nc_def_var(nc_state_file->nc_id, "dam_class",
                        NC_INT, 1, dimids, &(var_id));
    check_nc_status(status, "Error defining dam_class variable in %s",
                    filename);
    status = nc_put_att_text(nc_state_file->nc_id, var_id,
                             "long_name",
                             strlen("dam_class"), "dam_class");
    check_nc_status(status, "Error adding attribute in %s", filename);
    status = nc_put_att_text(nc_state_file->nc_id, var_id,
                             "standard_name",
                             strlen("dam_class"),
                             "dam_class");
    check_nc_status(status, "Error adding attribute in %s", filename);

    // Initialize history
    dimids[0] = nc_state_file->hist_dimid;

    // Define dimension variable
    status = nc_def_var(nc_state_file->nc_id, "history",
                        NC_INT, 1, dimids, &(var_id));
    check_nc_status(status, "Error defining history variable in %s",
                    filename);
    status = nc_put_att_text(nc_state_file->nc_id, var_id,
                             "long_name",
                             strlen("history"), "history");
    check_nc_status(status, "Error adding attribute in %s", filename);
    status = nc_put_att_text(nc_state_file->nc_id, var_id,
                             "standard_name",
                             strlen("history"),
                             "history");
    check_nc_status(status, "Error adding attribute in %s", filename);

    // Initialize months
    dimids[0] = nc_state_file->month_dimid;

    // Define dimension variable
    status = nc_def_var(nc_state_file->nc_id, "month",
                        NC_INT, 1, dimids, &(var_id));
    check_nc_status(status, "Error defining month variable in %s",
                    filename);
    status = nc_put_att_text(nc_state_file->nc_id, var_id,
                             "long_name",
                             strlen("month"), "month");
    check_nc_status(status, "Error adding attribute in %s", filename);
    status = nc_put_att_text(nc_state_file->nc_id, var_id,
                             "standard_name",
                             strlen("month"),
                             "month");
    check_nc_status(status, "Error adding attribute in %s", filename);
    
}

/******************************************
* @brief   Add state dimension variable data
******************************************/
void
dam_add_state_dim_var_data(char           *filename,
                            nc_file_struct *nc_state_file)
{   
    //extern dam_con_struct    **dam_con;
    extern int               *dam_classes;

    size_t dstart[MAXDIMS];
    size_t dcount[MAXDIMS];
    int    var_id;
    int    status;
    size_t i;
    int history[MONTHS_PER_YEAR * DAM_HIST_YEARS];
    int month[MONTHS_PER_YEAR];

    // Initialize dam_class
    dstart[0] = 0;
    dcount[0] = nc_state_file->dam_size;

    status = nc_inq_varid(nc_state_file->nc_id,
                          "dam_class", &var_id);
    check_nc_status(status, "Unable to find variable \"%s\" in %s",
                    "dam_class",
                    filename);

    status = nc_put_vara_int(nc_state_file->nc_id, var_id, dstart,
                             dcount, dam_classes);
    check_nc_status(status, "Error writing dam_class id in %s",
                    filename);

    // Initialize history
    dstart[0] = 0;
    dcount[0] = nc_state_file->hist_size;

    for(i=0; i<nc_state_file->hist_size; i++){
        history[i] = (int) i;
    }

    status = nc_inq_varid(nc_state_file->nc_id,
                          "history", &var_id);
    check_nc_status(status, "Unable to find variable \"%s\" in %s",
                    "history",
                    filename);

    status = nc_put_vara_int(nc_state_file->nc_id, var_id, dstart,
                             dcount, history);
    check_nc_status(status, "Error writing history in %s",
                    filename);

    // Initialize months
    dstart[0] = 0;
    dcount[0] = nc_state_file->month_size;

    for(i=0; i<nc_state_file->month_size; i++){
        month[i] = (int) i;
    }

    status = nc_inq_varid(nc_state_file->nc_id,
                          "month", &var_id);
    check_nc_status(status, "Unable to find variable \"%s\" in %s",
                    "month",
                    filename);

    status = nc_put_vara_int(nc_state_file->nc_id, var_id, dstart,
                             dcount, month);
    check_nc_status(status, "Error writing month in %s",
                    filename);


}

/******************************************
* @brief   Set state variable dimensions
******************************************/
void
dam_set_nc_state_var_info(nc_file_struct *nc,
                           size_t          varid)
{
    // Set the number of dimensions and dimids for each state variable
    switch (varid) {
        case N_STATE_VARS + STATE_DAM_STORAGE:
        case N_STATE_VARS + STATE_DAM_INFLOW:
        case N_STATE_VARS + STATE_DAM_DEMAND:
        case N_STATE_VARS + STATE_DAM_EFR:
            // Reset from 2D gridded to 1D
            nc->nc_vars[varid].nc_dims = 1;
            nc->nc_vars[varid].nc_counts[1] = 0;
            nc->nc_vars[varid].nc_dimids[1] = -1;
            // Overwrite the first dim
            nc->nc_vars[varid].nc_dimids[0] = nc->dam_dimid;
            nc->nc_vars[varid].nc_counts[0] = 1;
            nc->nc_vars[varid].nc_type = NC_DOUBLE;
            //nc->nc_vars[varid].nc_counts[0] = nc->dam_size;
            break;
        case N_STATE_VARS + STATE_DAM_MONTHS:
        case N_STATE_VARS + STATE_DAM_STEPS:
        case N_STATE_VARS + STATE_DAM_OP_MONTH:
            // Reset from 2D gridded to 1D
            nc->nc_vars[varid].nc_dims = 1;
            nc->nc_vars[varid].nc_counts[1] = 0;
            nc->nc_vars[varid].nc_dimids[1] = -1;
            // Overwrite the first dim
            nc->nc_vars[varid].nc_dimids[0] = nc->dam_dimid;
            nc->nc_vars[varid].nc_counts[0] = 1;
            nc->nc_vars[varid].nc_type = NC_INT;
            break;
        case N_STATE_VARS + STATE_DAM_HISTORY_EFR:
        case N_STATE_VARS + STATE_DAM_HISTORY_INFLOW:
        case N_STATE_VARS + STATE_DAM_HISTORY_DEMAND:
            // Reset from 2D gridded to 2D time series
            nc->nc_vars[varid].nc_dims = 2;
            nc->nc_vars[varid].nc_counts[0] = MONTHS_PER_YEAR * DAM_HIST_YEARS;
            nc->nc_vars[varid].nc_dimids[0] = nc->hist_dimid; 
            // Overwrite the first dim
            nc->nc_vars[varid].nc_dimids[1] = nc->dam_dimid;
            nc->nc_vars[varid].nc_counts[1] = 1;
            nc->nc_vars[varid].nc_type = NC_DOUBLE;
            break;
        case N_STATE_VARS + STATE_DAM_OP_STORAGE:
        case N_STATE_VARS + STATE_DAM_OP_RELEASE:
            // Reset from 2D gridded to 2D time series
            nc->nc_vars[varid].nc_dims = 2;
            nc->nc_vars[varid].nc_counts[0] = MONTHS_PER_YEAR;
            nc->nc_vars[varid].nc_dimids[0] = nc->month_dimid; 
            // Overwrite the first dim
            nc->nc_vars[varid].nc_dimids[1] = nc->dam_dimid;
            nc->nc_vars[varid].nc_counts[1] = 1;
            nc->nc_vars[varid].nc_type = NC_DOUBLE;
            break;
    }
}

/******************************************
* @brief   Store states
******************************************/
void
dam_store(nc_file_struct *state_file)
{
    extern domain_struct              local_domain;
    extern plugin_option_struct       plugin_options;
    extern dam_var_struct             **dam_var;
    extern dam_con_map_struct         *dam_con_map;

    size_t                            i;
    size_t                            j;
    int                               dam_index;
    //int                               dam_class;
    double                            *dvar = NULL;
    // double                            *dhistvar = NULL;
    // double                            *dmonthvar = NULL;
    int                               *ivar = NULL;
    size_t                            d1start[1];
    size_t                            d2start[2];
    nc_var_struct                     *nc_var;
    int                               status;

    // write state variables

    // allocate memory for variables to be stored
    dvar = malloc(1 * sizeof(*dvar));
    // dhistvar = malloc(MONTHS_PER_YEAR * DAM_HIST_YEARS * sizeof(*dhistvar));
    // dmonthvar = malloc(MONTHS_PER_YEAR * sizeof(dmonthvar));
    ivar = malloc(1 * sizeof(*ivar));
    check_alloc_status(dvar, "Memory allocation error");

    // initialize starts and counts
    d1start[0] = 0;
    d2start[0] = 0;
    d2start[1] = 0;

    nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_STORAGE]);

    for (j = 0; j < plugin_options.NDAMTYPES ; j++) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            dam_index = dam_con_map[i].didx[j];
            if (dam_index != NODATA_DAM){
                //dam_class = dam_con[i][dam_index].dam_class;
                d1start[0] = j;
                d2start[1] = j;
                // storage
                nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_STORAGE]);
                dvar[0] = (double) dam_var[i][dam_index].storage;
                status = nc_put_vara_double(state_file->nc_id, nc_var->nc_varid, d1start, nc_var->nc_counts, dvar);
                check_nc_status(status, "Error writing STATE_DAM_STORAGE.");
                // total inflow
                nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_INFLOW]);
                dvar[0] = (double) dam_var[i][dam_index].total_inflow;
                status = nc_put_vara_double(state_file->nc_id, nc_var->nc_varid, d1start, nc_var->nc_counts, dvar);
                check_nc_status(status, "Error writing STATE_DAM_INFLOW.");
                // total demand
                nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_DEMAND]);
                dvar[0] = (double) dam_var[i][dam_index].total_demand;
                status = nc_put_vara_double(state_file->nc_id, nc_var->nc_varid, d1start, nc_var->nc_counts, dvar);
                check_nc_status(status, "Error writing STATE_DAM_DEMAND.");
                // total efr
                nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_EFR]);
                dvar[0] = (double) dam_var[i][dam_index].total_efr;
                status = nc_put_vara_double(state_file->nc_id, nc_var->nc_varid, d1start, nc_var->nc_counts, dvar);
                check_nc_status(status, "Error writing STATE_DAM_EFR.");
                // months
                nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_MONTHS]);
                ivar[0] = (int) dam_var[i][dam_index].months_running;
                status = nc_put_vara_int(state_file->nc_id, nc_var->nc_varid, d1start, nc_var->nc_counts, ivar);
                check_nc_status(status, "Error writing STATE_DAM_MONTHS.");
                // steps
                nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_STEPS]);
                ivar[0] = (int) dam_var[i][dam_index].register_steps;
                status = nc_put_vara_int(state_file->nc_id, nc_var->nc_varid, d1start, nc_var->nc_counts, ivar);
                check_nc_status(status, "Error writing STATE_DAM_STEPS.");
                // operational month
                nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_OP_MONTH]);
                ivar[0] = (int) dam_var[i][dam_index].op_month;
                status = nc_put_vara_int(state_file->nc_id, nc_var->nc_varid, d1start, nc_var->nc_counts, ivar);
                check_nc_status(status, "Error writing STATE_DAM_OP_MONTH.");
                //operational release
                nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_OP_RELEASE]);
                //dmonthvar = dam_var[i][dam_index].op_release;
                status = nc_put_vara_double(state_file->nc_id, nc_var->nc_varid, d2start, nc_var->nc_counts,  dam_var[i][dam_index].op_release);
                check_nc_status(status, "Error writing STATE_DAM_OP_RELEASE.");
                //operational storage
                nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_OP_STORAGE]);
                // dmonthvar = dam_var[i][dam_index].op_storage;
                status = nc_put_vara_double(state_file->nc_id, nc_var->nc_varid, d2start, nc_var->nc_counts, dam_var[i][dam_index].op_storage);
                check_nc_status(status, "Error writing STATE_DAM_OP_STORAGE.");
                //hist demand
                nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND]);
                // dhistvar = dam_var[i][dam_index].history_demand;
                status = nc_put_vara_double(state_file->nc_id, nc_var->nc_varid, d2start, nc_var->nc_counts, dam_var[i][dam_index].history_demand);
                check_nc_status(status, "Error writing STATE_DAM_HIST_DEMAND.");
                //hist efr
                nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_HISTORY_EFR]);
                // dhistvar = dam_var[i][dam_index].history_efr;
                status = nc_put_vara_double(state_file->nc_id, nc_var->nc_varid, d2start, nc_var->nc_counts, dam_var[i][dam_index].history_efr);
                check_nc_status(status, "Error writing STATE_DAM_HIST_EFR.");
                //hist inflow
                nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW]);
                // dhistvar = dam_var[i][dam_index].history_inflow;
                status = nc_put_vara_double(state_file->nc_id, nc_var->nc_varid, d2start, nc_var->nc_counts, dam_var[i][dam_index].history_inflow);
                check_nc_status(status, "Error writing STATE_DAM_HIST_INFLOW.");
            }  
        }
    }
    
    free(dvar);
    //free(dhistvar);
    //free(dmonthvar);
    free(ivar);
}