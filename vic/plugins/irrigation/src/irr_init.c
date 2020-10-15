/******************************************************************************
 * @section DESCRIPTION
 *
 * Irrigation setup functions (from input file)
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
* @brief   Setup irrigation mapping
******************************************/
void
irr_set_mapping(void)
{
    extern domain_struct              local_domain;
    extern plugin_option_struct       plugin_options;
    extern veg_con_struct           **veg_con;
    extern irr_con_struct           **irr_con;
    extern irr_con_map_struct        *irr_con_map;

    size_t                            i;
    size_t                            j;
    int                            irr_index;
    int                            veg_index;
    size_t                            veg_class;
    
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < plugin_options.NIRRTYPES; j++){
            irr_index = irr_con_map[i].iidx[j];
            veg_index = irr_con_map[i].vidx[j];
            
            if (irr_index != NODATA_VEG) {
                veg_class = veg_con[i][veg_index].veg_class;
                
                irr_con[i][irr_index].irr_class = j;
                irr_con[i][irr_index].veg_index = veg_index;
                irr_con[i][irr_index].veg_class = veg_class;
            }
        }
    }
}

/******************************************
* @brief   Setup paddy irrigation
******************************************/
void
irr_set_paddy(void)
{
    extern domain_struct local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern plugin_option_struct       plugin_options;
    extern irr_con_map_struct  *irr_con_map;
    extern int mpi_rank;
    extern MPI_Comm                MPI_COMM_VIC;
    
    int *ivar;
    
    size_t i;
    size_t j;
    int    status;
    
    size_t  d1count[1];
    size_t  d1start[1];
    
    d1start[0] = 0;
    d1count[0] = plugin_options.NIRRTYPES;
    
    ivar = malloc(plugin_options.NIRRTYPES * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");
    
    if(mpi_rank == VIC_MPI_ROOT){
        get_nc_field_int(&(plugin_filenames.irrigation), 
                "paddy", d1start, d1count, ivar);
    }
    
    status = MPI_Bcast(ivar, plugin_options.NIRRTYPES, MPI_INT,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            if(ivar[irr_con[i][j].irr_class] == 1){
                irr_con[i][j].paddy = true;
            }
        }
    }
    
    free(ivar);
}

/******************************************
* @brief   Setup groundwater fraction and efficiency
******************************************/
void
irr_set_info(void)
{
    extern domain_struct local_domain;
    extern domain_struct global_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern irr_con_map_struct  *irr_con_map;
    extern irr_con_struct  **irr_con;
    
    double *dvar;
    
    size_t i;
    size_t j;
    
    size_t  d2count[2];
    size_t  d2start[2];
    
    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;
    
    dvar = malloc(local_domain.ncells_active * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");
    
    get_scatter_nc_field_double(&(plugin_filenames.irrigation), 
            "groundwater_fraction", d2start, d2count, dvar);

    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            if(irr_con[i][j].paddy){
                irr_con[i][j].groundwater_fraction = 0.0;
            } else {
                irr_con[i][j].groundwater_fraction = dvar[i];
            }
        }
    }
    
    get_scatter_nc_field_double(&(plugin_filenames.irrigation), 
            "irrigation_efficiency", d2start, d2count, dvar);

    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < irr_con_map[i].ni_active; j++){
            if(irr_con[i][j].paddy){
                irr_con[i][j].irrigation_efficiency = 1.0;
            } else {
                irr_con[i][j].irrigation_efficiency = dvar[i];
            }
        }
    }
    
    free(dvar);
}

/******************************************
* @brief   Setup the irrigation module
******************************************/
void
irr_init(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern int                     mpi_rank;

    int                            status;

    // open parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(plugin_filenames.irrigation.nc_filename, NC_NOWRITE,
                         &(plugin_filenames.irrigation.nc_id));
        check_nc_status(status, "Error opening %s",
                        plugin_filenames.irrigation.nc_filename);
    }

    irr_set_mapping();
    irr_set_paddy();
    irr_set_info();

    // close parameter file
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(plugin_filenames.irrigation.nc_id);
        check_nc_status(status, "Error closing %s",
                        plugin_filenames.irrigation.nc_filename);
    }
}
