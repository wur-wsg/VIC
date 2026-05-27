/******************************************************************************
 * @section DESCRIPTION
 *
 * Irrigation state functions: save and restore irr_var state
 * (received and leftover water per irrigation type and snow band)
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
* @brief   Set irrigation state metadata
******************************************/
void
irr_set_state_meta_data_info(void)
{
    extern metadata_struct state_metadata[];

    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_LEFTOVER].varname,
             MAXSTRING, "%s", "STATE_IRR_LEFTOVER");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_LEFTOVER].long_name,
             MAXSTRING, "%s", "irr_leftover");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_LEFTOVER].standard_name,
             MAXSTRING, "%s", "irr_leftover");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_LEFTOVER].units,
             MAXSTRING, "%s", "mm");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_LEFTOVER].description,
             MAXSTRING, "%s",
             "leftover irrigation water carried to next timestep");

    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_RECEIVED].varname,
             MAXSTRING, "%s", "STATE_IRR_RECEIVED");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_RECEIVED].long_name,
             MAXSTRING, "%s", "irr_received");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_RECEIVED].standard_name,
             MAXSTRING, "%s", "irr_received");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_RECEIVED].units,
             MAXSTRING, "%s", "mm");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_RECEIVED].description,
             MAXSTRING, "%s",
             "received irrigation water for current timestep");

    // STATE_IRR_OFFSET
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_OFFSET].varname,
             MAXSTRING, "%s", "STATE_IRR_OFFSET");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_OFFSET].long_name,
             MAXSTRING, "%s", "irr_offset");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_OFFSET].standard_name,
             MAXSTRING, "%s", "irr_offset");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_OFFSET].units,
             MAXSTRING, "%s", "-");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_OFFSET].description,
             MAXSTRING, "%s",
             "irrigation season start offset counter (days since Cv change)");

    // STATE_IRR_PREV_CV
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_PREV_CV].varname,
             MAXSTRING, "%s", "STATE_IRR_PREV_CV");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_PREV_CV].long_name,
             MAXSTRING, "%s", "irr_prev_Cv");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_PREV_CV].standard_name,
             MAXSTRING, "%s", "irr_prev_Cv");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_PREV_CV].units,
             MAXSTRING, "%s", "-");
    snprintf(state_metadata[N_STATE_VARS + STATE_IRR_PREV_CV].description,
             MAXSTRING, "%s",
             "previous vegetation coverage fraction for irrigation season detection");
}

/******************************************
* @brief   Initialize state file dimension size and id
******************************************/
void
irr_set_nc_state_file_info(nc_file_struct *nc_state_file)
{
    extern plugin_option_struct plugin_options;

    nc_state_file->irr_dimid = MISSING;
    nc_state_file->irr_size = plugin_options.NIRRTYPES;
}

/******************************************
* @brief   Add irrigation_type dimension to state file
******************************************/
void
irr_add_state_dim(char           *filename,
                  nc_file_struct *nc_state_file)
{
    int status;

    status = nc_def_dim(nc_state_file->nc_id, "irrigation_type",
                        nc_state_file->irr_size,
                        &(nc_state_file->irr_dimid));
    check_nc_status(status, "Error defining irrigation_type dim in %s",
                    filename);
}

/******************************************
* @brief   Add irrigation_type dimension variable
******************************************/
void
irr_add_state_dim_var(char           *filename,
                      nc_file_struct *nc_state_file)
{
    int dimids[MAXDIMS];
    int var_id;
    int status;

    dimids[0] = nc_state_file->irr_dimid;

    status = nc_def_var(nc_state_file->nc_id, "irrigation_type",
                        NC_INT, 1, dimids, &var_id);
    check_nc_status(status, "Error defining irrigation_type variable in %s",
                    filename);
    status = nc_put_att_text(nc_state_file->nc_id, var_id,
                             "long_name",
                             strlen("irrigation_type"), "irrigation_type");
    check_nc_status(status, "Error adding attribute in %s", filename);
}

/******************************************
* @brief   Write irrigation_type dimension data (0, 1, 2, ...)
******************************************/
void
irr_add_state_dim_var_data(char           *filename,
                           nc_file_struct *nc_state_file)
{
    size_t  dstart[MAXDIMS];
    size_t  dcount[MAXDIMS];
    int     var_id;
    int    *ivar;
    int     status;
    size_t  i;

    dstart[0] = 0;
    dcount[0] = nc_state_file->irr_size;

    status = nc_inq_varid(nc_state_file->nc_id, "irrigation_type", &var_id);
    check_nc_status(status, "Unable to find variable \"irrigation_type\" in %s",
                    filename);

    ivar = malloc(nc_state_file->irr_size * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error");

    for (i = 0; i < nc_state_file->irr_size; i++) {
        ivar[i] = (int) i;
    }
    status = nc_put_vara_int(nc_state_file->nc_id, var_id, dstart,
                             dcount, ivar);
    check_nc_status(status, "Error writing irrigation_type in %s", filename);
    free(ivar);
}

/******************************************
* @brief   Set state variable NC dimensions for irrigation variables
*          Layout: [irr_type][band][ny][nx]
******************************************/
void
irr_set_nc_state_var_info(nc_file_struct *nc,
                          size_t          varid)
{
    switch (varid) {
    case N_STATE_VARS + STATE_IRR_LEFTOVER:
    case N_STATE_VARS + STATE_IRR_RECEIVED:
    case N_STATE_VARS + STATE_IRR_OFFSET:
    case N_STATE_VARS + STATE_IRR_PREV_CV:
        nc->nc_vars[varid].nc_dims = 4;
        nc->nc_vars[varid].nc_dimids[0] = nc->irr_dimid;
        nc->nc_vars[varid].nc_dimids[1] = nc->band_dimid;
        nc->nc_vars[varid].nc_dimids[2] = nc->nj_dimid;
        nc->nc_vars[varid].nc_dimids[3] = nc->ni_dimid;
        nc->nc_vars[varid].nc_counts[0] = 1;
        nc->nc_vars[varid].nc_counts[1] = 1;
        nc->nc_vars[varid].nc_counts[2] = nc->nj_size;
        nc->nc_vars[varid].nc_counts[3] = nc->ni_size;
        break;
    }
}

/******************************************
* @brief   Store irrigation state
*          Writes irr_var[cell][iIrr][iBand].leftover and .received
*          as [NIRRTYPES][SNOW_BAND][ny][nx] fields.
******************************************/
void
irr_store(nc_file_struct *state_file)
{
    extern domain_struct        local_domain;
    extern option_struct        options;
    extern plugin_option_struct plugin_options;
    extern irr_con_map_struct  *irr_con_map;
    extern irr_var_struct    ***irr_var;

    size_t         i;
    size_t         iIrr;
    size_t         iBand;
    double        *dvar = NULL;
    size_t         d4start[4];
    nc_var_struct *nc_var;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error");

    d4start[0] = 0;
    d4start[1] = 0;
    d4start[2] = 0;
    d4start[3] = 0;

    /* ----- STATE_IRR_LEFTOVER ----- */
    nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_IRR_LEFTOVER]);
    for (iIrr = 0; iIrr < plugin_options.NIRRTYPES; iIrr++) {
        d4start[0] = iIrr;
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            d4start[1] = iBand;
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iIrr < irr_con_map[i].ni_active) {
                    dvar[i] = irr_var[i][iIrr][iBand].leftover;
                } else {
                    dvar[i] = state_file->d_fillvalue;
                }
            }
            gather_put_nc_field_double(state_file->nc_id,
                                       nc_var->nc_varid,
                                       state_file->d_fillvalue,
                                       d4start, nc_var->nc_counts, dvar);
        }
    }

    /* ----- STATE_IRR_RECEIVED ----- */
    nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_IRR_RECEIVED]);
    for (iIrr = 0; iIrr < plugin_options.NIRRTYPES; iIrr++) {
        d4start[0] = iIrr;
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            d4start[1] = iBand;
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iIrr < irr_con_map[i].ni_active) {
                    dvar[i] = irr_var[i][iIrr][iBand].received;
                } else {
                    dvar[i] = state_file->d_fillvalue;
                }
            }
            gather_put_nc_field_double(state_file->nc_id,
                                       nc_var->nc_varid,
                                       state_file->d_fillvalue,
                                       d4start, nc_var->nc_counts, dvar);
        }
    }

    /* ----- STATE_IRR_OFFSET ----- */
    nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_IRR_OFFSET]);
    for (iIrr = 0; iIrr < plugin_options.NIRRTYPES; iIrr++) {
        d4start[0] = iIrr;
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            d4start[1] = iBand;
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iIrr < irr_con_map[i].ni_active) {
                    dvar[i] = (double) irr_var[i][iIrr][iBand].offset;
                } else {
                    dvar[i] = state_file->d_fillvalue;
                }
            }
            gather_put_nc_field_double(state_file->nc_id,
                                       nc_var->nc_varid,
                                       state_file->d_fillvalue,
                                       d4start, nc_var->nc_counts, dvar);
        }
    }

    /* ----- STATE_IRR_PREV_CV ----- */
    nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_IRR_PREV_CV]);
    for (iIrr = 0; iIrr < plugin_options.NIRRTYPES; iIrr++) {
        d4start[0] = iIrr;
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            d4start[1] = iBand;
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iIrr < irr_con_map[i].ni_active) {
                    dvar[i] = irr_var[i][iIrr][iBand].prev_Cv;
                } else {
                    dvar[i] = state_file->d_fillvalue;
                }
            }
            gather_put_nc_field_double(state_file->nc_id,
                                       nc_var->nc_varid,
                                       state_file->d_fillvalue,
                                       d4start, nc_var->nc_counts, dvar);
        }
    }

    /* ----- STATE_IRR_OFFSET ----- */
    nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_IRR_OFFSET]);
    for (iIrr = 0; iIrr < plugin_options.NIRRTYPES; iIrr++) {
        d4start[0] = iIrr;
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            d4start[1] = iBand;
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iIrr < irr_con_map[i].ni_active) {
                    dvar[i] = (double) irr_var[i][iIrr][iBand].offset;
                } else {
                    dvar[i] = state_file->d_fillvalue;
                }
            }
            gather_put_nc_field_double(state_file->nc_id,
                                       nc_var->nc_varid,
                                       state_file->d_fillvalue,
                                       d4start, nc_var->nc_counts, dvar);
        }
    }

    /* ----- STATE_IRR_PREV_CV ----- */
    nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_IRR_PREV_CV]);
    for (iIrr = 0; iIrr < plugin_options.NIRRTYPES; iIrr++) {
        d4start[0] = iIrr;
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            d4start[1] = iBand;
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iIrr < irr_con_map[i].ni_active) {
                    dvar[i] = irr_var[i][iIrr][iBand].prev_Cv;
                } else {
                    dvar[i] = state_file->d_fillvalue;
                }
            }
            gather_put_nc_field_double(state_file->nc_id,
                                       nc_var->nc_varid,
                                       state_file->d_fillvalue,
                                       d4start, nc_var->nc_counts, dvar);
        }
    }

    free(dvar);
}

/******************************************
* @brief   Restore irrigation state
*          Reads irr_var[cell][iIrr][iBand].leftover and .received
*          from [NIRRTYPES][SNOW_BAND][ny][nx] fields in the state file.
******************************************/
void
irr_restore(void)
{
    extern domain_struct        global_domain;
    extern domain_struct        local_domain;
    extern option_struct        options;
    extern plugin_option_struct plugin_options;
    extern irr_con_map_struct  *irr_con_map;
    extern irr_var_struct    ***irr_var;
    extern filenames_struct     filenames;
    extern metadata_struct      state_metadata[];

    size_t  i;
    size_t  iIrr;
    size_t  iBand;
    double *dvar = NULL;
    size_t  d4start[4];
    size_t  d4count[4];

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error");

    d4start[0] = 0;
    d4start[1] = 0;
    d4start[2] = 0;
    d4start[3] = 0;
    d4count[0] = 1;
    d4count[1] = 1;
    d4count[2] = global_domain.n_ny;
    d4count[3] = global_domain.n_nx;

    /* ----- STATE_IRR_LEFTOVER ----- */
    for (iIrr = 0; iIrr < plugin_options.NIRRTYPES; iIrr++) {
        d4start[0] = iIrr;
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            d4start[1] = iBand;
            get_scatter_nc_field_double(
                &(filenames.init_state),
                state_metadata[N_STATE_VARS + STATE_IRR_LEFTOVER].varname,
                d4start, d4count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iIrr < irr_con_map[i].ni_active) {
                    irr_var[i][iIrr][iBand].leftover = dvar[i];
                }
            }
        }
    }

    /* ----- STATE_IRR_RECEIVED ----- */
    for (iIrr = 0; iIrr < plugin_options.NIRRTYPES; iIrr++) {
        d4start[0] = iIrr;
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            d4start[1] = iBand;
            get_scatter_nc_field_double(
                &(filenames.init_state),
                state_metadata[N_STATE_VARS + STATE_IRR_RECEIVED].varname,
                d4start, d4count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iIrr < irr_con_map[i].ni_active) {
                    irr_var[i][iIrr][iBand].received = dvar[i];
                }
            }
        }
    }


    /* ----- STATE_IRR_OFFSET ----- */
    for (iIrr = 0; iIrr < plugin_options.NIRRTYPES; iIrr++) {
        d4start[0] = iIrr;
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            d4start[1] = iBand;
            get_scatter_nc_field_double(
                &(filenames.init_state),
                state_metadata[N_STATE_VARS + STATE_IRR_OFFSET].varname,
                d4start, d4count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iIrr < irr_con_map[i].ni_active) {
                    irr_var[i][iIrr][iBand].offset = (size_t) dvar[i];
                }
            }
        }
    }

    /* ----- STATE_IRR_PREV_CV ----- */
    for (iIrr = 0; iIrr < plugin_options.NIRRTYPES; iIrr++) {
        d4start[0] = iIrr;
        for (iBand = 0; iBand < options.SNOW_BAND; iBand++) {
            d4start[1] = iBand;
            get_scatter_nc_field_double(
                &(filenames.init_state),
                state_metadata[N_STATE_VARS + STATE_IRR_PREV_CV].varname,
                d4start, d4count, dvar);
            for (i = 0; i < local_domain.ncells_active; i++) {
                if (iIrr < irr_con_map[i].ni_active) {
                    irr_var[i][iIrr][iBand].prev_Cv = dvar[i];
                }
            }
        }
    }

    free(dvar);
}
