/******************************************************************************
 * @section DESCRIPTION
 *
 * Landuse state functions: save and restore veg_con[].Cv
 *
 *****************************************************************************/

#include <vic_driver_image.h>
#include <plugin.h>

/******************************************
* @brief   Set landuse state metadata
******************************************/
void
lu_set_state_meta_data_info(void)
{
    extern metadata_struct state_metadata[];

    snprintf(state_metadata[N_STATE_VARS + STATE_LU_CV].varname,
             MAXSTRING, "%s", "STATE_LU_CV");
    snprintf(state_metadata[N_STATE_VARS + STATE_LU_CV].long_name,
             MAXSTRING, "%s", "lu_cv");
    snprintf(state_metadata[N_STATE_VARS + STATE_LU_CV].standard_name,
             MAXSTRING, "%s", "lu_cv");
    snprintf(state_metadata[N_STATE_VARS + STATE_LU_CV].units,
             MAXSTRING, "%s", "-");
    snprintf(state_metadata[N_STATE_VARS + STATE_LU_CV].description,
             MAXSTRING, "%s",
             "vegetation coverage fraction");
}

/******************************************
* @brief   Set state variable NC dimensions for landuse variables
******************************************/
void
lu_set_nc_state_var_info(nc_file_struct *nc,
                         size_t          varid)
{
    switch (varid) {
    case N_STATE_VARS + STATE_LU_CV:
        nc->nc_vars[varid].nc_dims = 3;
        nc->nc_vars[varid].nc_dimids[0] = nc->veg_dimid;
        nc->nc_vars[varid].nc_dimids[1] = nc->nj_dimid;
        nc->nc_vars[varid].nc_dimids[2] = nc->ni_dimid;
        nc->nc_vars[varid].nc_counts[0] = 1;
        nc->nc_vars[varid].nc_counts[1] = nc->nj_size;
        nc->nc_vars[varid].nc_counts[2] = nc->ni_size;
        break;
    }
}

/******************************************
* @brief   Store landuse state
******************************************/
void
lu_store(nc_file_struct *state_file)
{
    extern domain_struct        local_domain;
    extern option_struct        options;
    extern veg_con_map_struct  *veg_con_map;
    extern veg_con_struct     **veg_con;

    size_t         i;
    size_t         iVeg;
    int            vidx;
    double        *dvar = NULL;
    size_t         d3start[3];
    nc_var_struct *nc_var;

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error");

    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;

    /* ----- STATE_LU_CV ----- */
    nc_var = &(state_file->nc_vars[N_STATE_VARS + STATE_LU_CV]);
    for (iVeg = 0; iVeg < options.NVEGTYPES; iVeg++) {
        d3start[0] = iVeg;
        for (i = 0; i < local_domain.ncells_active; i++) {
            vidx = veg_con_map[i].vidx[iVeg];
            if (vidx >= 0) {
                dvar[i] = veg_con[i][vidx].Cv;
            } else {
                dvar[i] = state_file->d_fillvalue;
            }
        }
        gather_put_nc_field_double(state_file->nc_id,
                                   nc_var->nc_varid,
                                   state_file->d_fillvalue,
                                   d3start, nc_var->nc_counts, dvar);
    }

    free(dvar);
}

/******************************************
* @brief   Restore landuse state
******************************************/
void
lu_restore(void)
{
    extern domain_struct        global_domain;
    extern domain_struct        local_domain;
    extern option_struct        options;
    extern veg_con_map_struct  *veg_con_map;
    extern veg_con_struct     **veg_con;
    extern filenames_struct     filenames;
    extern metadata_struct      state_metadata[];

    size_t  i;
    size_t  iVeg;
    int     vidx;
    double *dvar = NULL;
    size_t  d3start[3];
    size_t  d3count[3];

    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error");

    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    /* ----- STATE_LU_CV ----- */
    for (iVeg = 0; iVeg < options.NVEGTYPES; iVeg++) {
        d3start[0] = iVeg;
        get_scatter_nc_field_double(
            &(filenames.init_state),
            state_metadata[N_STATE_VARS + STATE_LU_CV].varname,
            d3start, d3count, dvar);
        for (i = 0; i < local_domain.ncells_active; i++) {
            vidx = veg_con_map[i].vidx[iVeg];
            if (vidx >= 0) {
                veg_con[i][vidx].Cv = dvar[i];
                veg_con_map[i].Cv[iVeg] = dvar[i];
            }
        }
    }

    free(dvar);
}
