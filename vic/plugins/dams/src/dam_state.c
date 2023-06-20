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
}

/******************************************
* @brief   Initialize state dimension size and ids
******************************************/
void
dam_set_nc_state_file_info(nc_file_struct *nc_state_file)
{
    extern plugin_option_struct       plugin_options;

    nc_state_file->dam_dimid = MISSING;
    nc_state_file->dam_size = plugin_options.NDAMTYPES;

}

/******************************************
* @brief   Add state dimensions
******************************************/
void
dam_add_state_dim(char           *filename,
                   nc_file_struct *nc_state_file)
{
    int status;

    // Define dimension
    status = nc_def_dim(nc_state_file->nc_id, "dam_class",
                        nc_state_file->dam_size,
                        &(nc_state_file->dam_dimid));
    check_nc_status(status, "Error defining dam_class in %s", filename);
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

    // Initialize
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
    //int    *ivar;
    int    status;
    //int    dam_index;
    //int    dam_class;
    //size_t i;
    //size_t j;

    // Initialize
    dstart[0] = 0;
    dcount[0] = nc_state_file->dam_size;

    status = nc_inq_varid(nc_state_file->nc_id,
                          "dam_class", &var_id);
    check_nc_status(status, "Unable to find variable \"%s\" in %s",
                    "dam_class",
                    filename);

    // Fill dimension variable
    // ivar = malloc(nc_state_file->dam_size * sizeof(*ivar));
    // check_alloc_status(ivar, "Memory allocation error");

    // TODO: check this. only works when dam_class is sequence starting from 1 to # with interval of 1.
    // Maybe better to store the real dam_class values 
    
    // for (i = 0; i < global_domain.ncells_active; i++){
    //     for (j = 0; j < plugin_options.NDAMTYPES; j++) {
    //         dam_index = dam_con_map[i].didx[j];

    //         if (dam_index != NODATA_DAM) {
    //             dam_class = dam_con[i][dam_index].dam_class;
    //             ivar[j] =  dam_class;
    //         }
    //     }
    // }

    status = nc_put_vara_int(nc_state_file->nc_id, var_id, dstart,
                             dcount, dam_classes);
    check_nc_status(status, "Error writing dam_class id in %s",
                    filename);
    //free(ivar);
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
        // Reset from 2D gridded to 1D
        nc->nc_vars[varid].nc_dims = 1;
        nc->nc_vars[varid].nc_counts[1] = 0;
        nc->nc_vars[varid].nc_dimids[1] = -1;
        // Overwrite the first dim
        nc->nc_vars[varid].nc_dimids[0] = nc->dam_dimid;
        nc->nc_vars[varid].nc_counts[0] = 1;
        //nc->nc_vars[varid].nc_counts[0] = nc->dam_size;
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
    size_t                            d3start[1];
    nc_var_struct                     *nc_var;
    int                               status;

    // write state variables

    // allocate memory for variables to be stored
    dvar = malloc(1 * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error");

    // initialize starts and counts
    d3start[0] = 0;

    nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_DAM_STORAGE]);

    for (j = 0; j < plugin_options.NDAMTYPES ; j++) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            dam_index = dam_con_map[i].didx[j];
            if (dam_index != NODATA_DAM){
                //dam_class = dam_con[i][dam_index].dam_class;
                d3start[0] = j;
                dvar[0] = (double) dam_var[i][dam_index].storage;
                status = nc_put_vara_double(state_file->nc_id, nc_var->nc_varid, d3start, nc_var->nc_counts, dvar);
                check_nc_status(status, "Error writing values.");
            }  
        }
    }
    
    // gather_put_nc_field_double(state_file->nc_id,
    //         nc_var->nc_varid,
    //         state_file->d_fillvalue,
    //         d3start, nc_var->nc_counts, dvar);

    free(dvar);
}