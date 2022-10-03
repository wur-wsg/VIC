/******************************************************************************
 * @section DESCRIPTION
 *
 * Water-use setup functions (from input file)
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
* @brief   Setup remote mapping
******************************************/
void
wu_set_receiving(void)
{
    extern domain_struct           global_domain;
    extern domain_struct           local_domain;
    extern plugin_filenames_struct plugin_filenames;
    extern wu_con_struct          *wu_con;

    int                           *ivar;
    int                           *receiving_id;
    size_t                        *nreceiving_tmp;
    size_t                         error_count;

    size_t                         i;
    size_t                         j;
    size_t                         k;

    size_t                         d2count[2];
    size_t                         d2start[2];
    size_t                         d3count[3];
    size_t                         d3start[3];

    ivar = malloc(local_domain.ncells_active * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");
    receiving_id = malloc(local_domain.ncells_active * sizeof(*receiving_id));
    check_alloc_status(receiving_id, "Memory allocation error.");
    nreceiving_tmp = malloc(local_domain.ncells_active * sizeof(*nreceiving_tmp));
    check_alloc_status(nreceiving_tmp, "Memory allocation error.");

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    d3start[0] = 0;
    d3start[1] = 0;
    d3start[2] = 0;
    d3count[0] = 1;
    d3count[1] = global_domain.n_ny;
    d3count[2] = global_domain.n_nx;

    get_scatter_nc_field_int(&(plugin_filenames.wateruse), "receiving_id",
                             d2start, d2count, receiving_id);

    error_count = 0;
    for (i = 0; i < local_domain.ncells_active; i++) {
        nreceiving_tmp[i] = 0;
    }

    for (k = 0; k < plugin_options.NWURECEIVING; k++) {
        d3start[0] = k;

        get_scatter_nc_field_int(&(plugin_filenames.wateruse), "receiving",
                                 d3start, d3count, ivar);

        for (i = 0; i < local_domain.ncells_active; i++) {
            if(ivar[i] > 0){
                for(j = 0; j < local_domain.ncells_active; j++){
                    if(ivar[i] != receiving_id[j]){
                        continue;
                    }
                    
                    if(nreceiving_tmp[j] >= wu_con[j].nreceiving){
                        log_err("number of receiving cells (%zu) is larger "
                                "than specified in the parameter file (%zu)",
                                nreceiving_tmp[j], wu_con[j].nreceiving);
                    }
                    wu_con[j].receiving[nreceiving_tmp[j]] = i;
                    nreceiving_tmp[j]++;
                    break;
                }
            }
        }
    }
     
    for (i = 0; i < local_domain.ncells_active; i++) {
        if (nreceiving_tmp[i] != wu_con[i].nreceiving) {
            error_count++;
        }
        wu_con[i].nreceiving = nreceiving_tmp[i];
    }

    if (error_count > 0) {
        log_warn("No receiving cells were found for %zu cells; "
                 "Probably the ID was outside of the mask or "
                 "the ID was not set; "
                 "Removing receiving cells",
                 error_count);
    }

    free(nreceiving_tmp);
    free(ivar);
    free(receiving_id);
}

/******************************************
* @brief   Set the upstream-downstream order for water use
******************************************/
void
wu_set_routing_order()
{
    extern domain_struct    local_domain;
    extern rout_con_struct *rout_con;
    extern wu_con_struct *wu_con;
    extern size_t          *routing_order;

    bool                    *done_tmp;
    bool                    *done_fin;
    size_t                  rank;
    bool                    has_upstream;
    bool                    has_command;

    size_t                  i;
    size_t                  j;

    done_tmp = malloc(local_domain.ncells_active * sizeof(*done_tmp));
    check_alloc_status(done_tmp, "Memory allocation error.");
    done_fin = malloc(local_domain.ncells_active * sizeof(*done_fin));
    check_alloc_status(done_fin, "Memory allocation error.");
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        done_tmp[i] = false;
        done_fin[i] = false;
    }

    // Set cell_order_local for node
    rank = 0;
    while (rank < local_domain.ncells_active) {
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (done_fin[i]) {
                continue;
            }

            // count number of upstream cells that are not processed yet
            has_upstream = false;
            for (j = 0; j < rout_con[i].Nupstream; j++) {
                if (!done_fin[rout_con[i].upstream[j]]) {
                    has_upstream = true;
                    break;
                }
            }

            if (has_upstream) {
                continue;
            }
            
            has_command = false;
            for(j = 0; j < wu_con[i].nreceiving; j++){
                if (!done_fin[wu_con[i].receiving[j]]) {
                    has_command = true;
                    break;
                }
            }

            if (has_command) {
                continue;
            }

            // if no upstream, add as next order
            routing_order[rank] = i;
            done_tmp[i] = true;
            rank++;

            if (rank > local_domain.ncells_active) {
                log_err("Error in ordering and ranking cells");
            }
        }
        for (i = 0; i < local_domain.ncells_active; i++) {
            if (done_tmp[i] == true) {
                done_fin[i] = true;
            }
        }
    }
    
    free(done_tmp);
    free(done_fin);
}

/******************************************
* @brief   Check the upstream-downstream order for water use
******************************************/
void
wu_check_routing_order()
{
    extern domain_struct    local_domain;
    extern rout_con_struct *rout_con;
    extern wu_con_struct   *wu_con;
    extern size_t          *routing_order;

    size_t                 *inverse_order;

    size_t                  i;
    size_t                  j;
    size_t                  iDownstream;
    size_t                  iReceiving;

    inverse_order = malloc(local_domain.ncells_active * sizeof(*inverse_order));
    check_alloc_status(inverse_order, "Memory allocation error.");
    
    for(i = 0; i < local_domain.ncells_active; i++){
        inverse_order[routing_order[i]] = i;
    }
    
    for(i = 0; i < local_domain.ncells_active; i++) {
        iDownstream = rout_con[i].downstream;
        
        for(j = 0; j < wu_con[i].nreceiving; j++){
            iReceiving = wu_con[i].receiving[j];
            
            if(inverse_order[iReceiving] >= inverse_order[iDownstream]){
                log_err("Error in routing order for water-use. "
                        "Receiving cell %zu of source cell %zu "
                        "has a greater or equal order "
                        "than the source downstream cell %zu "
                        "(%zu >= %zu)",
                        iReceiving, i, iDownstream, 
                        inverse_order[iReceiving], 
                        inverse_order[iDownstream]
                        )
            }
        }
    }
    
    
    free(inverse_order);
}

/******************************************
* @brief   Setup the water-use module
******************************************/
void
wu_init(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern int                     mpi_rank;

    int                            status;

    if (plugin_options.REMOTE_WITH) {
        // open parameter file
        if (mpi_rank == VIC_MPI_ROOT) {
            status = nc_open(plugin_filenames.wateruse.nc_filename, NC_NOWRITE,
                             &(plugin_filenames.wateruse.nc_id));
            check_nc_status(status, "Error opening %s",
                            plugin_filenames.wateruse.nc_filename);
        }
        
        wu_set_receiving();
        wu_set_routing_order();
        wu_check_routing_order();

        // close parameter file
        if (mpi_rank == VIC_MPI_ROOT) {
            status = nc_close(plugin_filenames.wateruse.nc_id);
            check_nc_status(status, "Error closing %s",
                            plugin_filenames.wateruse.nc_filename);
        }
    }
}
