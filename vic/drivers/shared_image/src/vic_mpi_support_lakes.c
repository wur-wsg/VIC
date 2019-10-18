/******************************************************************************
 * @section DESCRIPTION
 *
 * MPI support routines for VIC
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

#include <vic_driver_shared_image.h>

/******************************************************************************
 * @brief   Decompose the domain for MPI operations
 * @details This function sets up the arrays needed to scatter and gather
 *          data from and to the master process to the individual mpi
 *          processes.
 *
 * @param ncells total number of cells
 * @param mpi_size number of mpi processes
 * @param mpi_lake_local_array_sizes address of integer array with number of
 *        cells assigned to each node (MPI_Scatterv:sendcounts and
 *        MPI_Gatherv:recvcounts)
 * @param mpi_lake_global_array_offsets address of integer array with offsets
 *        for sending and receiving data (MPI_Scatterv:displs and
 *        MPI_Gatherv:displs)
 * @param mpi_lake_mapping_array address of size_t array with indices to prepare
 *        an array on the master process for MPI_Scatterv or map back after
 *        MPI_Gatherv
 *****************************************************************************/
void
mpi_lake_decomp_domain()
{
    extern option_struct options;
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern lake_con_map_struct *lake_con_map;
    extern size_t        mpi_size;
    extern int           mpi_rank;
    extern int          *mpi_lake_global_array_offsets;
    extern int          *mpi_lake_local_array_sizes;
    extern size_t       *mpi_lake_mapping_array;
    
    int   *nlakes_global;
    int   *nlakes_local;
    int   *lakeid_global;
    int   *lakeid_local;
    
    size_t i;
    size_t j;
    size_t k;
    size_t n;
    int    id;
    int    max_id;
    bool   found;

    if (mpi_rank == VIC_MPI_ROOT){
        mpi_lake_local_array_sizes = calloc(mpi_size,
                                            sizeof(*mpi_lake_local_array_sizes));
        mpi_lake_global_array_offsets = calloc(mpi_size,
                                               sizeof(*
                                                        mpi_lake_global_array_offsets));
        mpi_lake_mapping_array = calloc(global_domain.nlakes_active, sizeof(*mpi_lake_mapping_array));
    }

    // gather number of lakes per node
    if (mpi_rank == VIC_MPI_ROOT){
        nlakes_global = malloc(mpi_size * sizeof(*nlakes_global));
    }
    nlakes_local = malloc(1 * sizeof(*nlakes_local));
            
    *nlakes_local = local_domain.nlakes_active;
    MPI_Gather(nlakes_local, 1, MPI_INT, nlakes_global, 1, MPI_INT, VIC_MPI_ROOT, MPI_COMM_VIC);
    
    // determine number of lakes per node
    if (mpi_rank == VIC_MPI_ROOT) {
        for (n = 0; n < mpi_size; n++) {
            mpi_lake_local_array_sizes[n] += nlakes_global[n];
        }

        // determine offsets to use for MPI_Scatterv and MPI_Gatherv
        for (n = 1; n < mpi_size; n++) {
            for (j = 0; j < n; j++) {
                mpi_lake_global_array_offsets[n] +=
                    mpi_lake_local_array_sizes[j];
            }
        }
    }
    
    // gather mapping array
    if (mpi_rank == VIC_MPI_ROOT){
        lakeid_global = malloc(global_domain.nlakes_active * sizeof(*lakeid_global));
    }
    lakeid_local = malloc(local_domain.nlakes_active * sizeof(*lakeid_local));
    
    k = 0;
    for (j = 0; j < options.NVEGTYPES; j++) {
        for (i = 0; i < local_domain.ncells_active; i++){
            id = lake_con_map[i].lake_id[j];
            if (id != NODATA_VEG) {
                lakeid_local[k] = id;
                k++;
            }
        }
    }
    MPI_Gatherv(lakeid_local, local_domain.nlakes_active, MPI_INT, 
                lakeid_global, mpi_lake_local_array_sizes, mpi_lake_global_array_offsets, 
                MPI_INT, VIC_MPI_ROOT, MPI_COMM_VIC);
    
    // check lake ids
    if (mpi_rank == VIC_MPI_ROOT){
        max_id = 0;
        for (k = 0; k < global_domain.nlakes_active; k++) {
            if (lakeid_global[k] > max_id) {
                max_id = lakeid_global[k];
            }
        }
        
        for(id = 0; id < max_id; id++){
            found = false;
            for (k = 0; k < global_domain.nlakes_active; k++) {
                if(lakeid_global[k] == id){
                    found = true;
                    break;
                }
            }
            if(!found){
                log_err("Lake_id is not sequential, "
                        "missing %d in range 0-%d",
                        id, max_id);
            }
        }
    }
    
    // set mapping array
    if (mpi_rank == VIC_MPI_ROOT){
        for (k = 0; k < global_domain.nlakes_active; k++) {
            mpi_lake_mapping_array[k] = lakeid_global[k];
        }
    }
    
    if (mpi_rank == VIC_MPI_ROOT){
        free(nlakes_global);
        free(lakeid_global);
    }
    free(nlakes_local);
    free(lakeid_local);
}

/******************************************************************************
 * @brief   Gather double precision variable
 * @details Values are gathered to the master node
 *****************************************************************************/
void
gather_field_double_lake_only(double  fillval,
                    double *dvar,
                    double *var)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_lake_global_array_offsets;
    extern int          *mpi_lake_local_array_sizes;
    extern size_t       *mpi_lake_mapping_array;
    int                  status;
    double              *dvar_gathered = NULL;
    size_t               grid_size;
    size_t               i;

    if (mpi_rank == VIC_MPI_ROOT) {
        grid_size = global_domain.nlakes_active;
        for (i = 0; i < grid_size; i++) {
            dvar[i] = fillval;
        }
        dvar_gathered =
            malloc(global_domain.nlakes_active * sizeof(*dvar_gathered));
        check_alloc_status(dvar_gathered, "Memory allocation error.");
    }
    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(var, local_domain.nlakes_active, MPI_DOUBLE,
                         dvar_gathered, mpi_lake_local_array_sizes,
                         mpi_lake_global_array_offsets, MPI_DOUBLE,
                         VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof(double), global_domain.nlakes_active, NULL,
            mpi_lake_mapping_array, dvar_gathered, dvar);
//        fprintf(LOG_DEST, "INID\tOUTID\tOLD\t\t\tNEW\n");
//        for(i = 0; i < global_domain.nlakes_active; i++){
//            fprintf(LOG_DEST, "%zu\t%zu\t%.3f\t\t\t%.3f\n",
//                    i, mpi_lake_mapping_array[i], dvar_gathered[i], dvar[i]);
//        }
        // cleanup
        free(dvar_gathered);
    }
}

/******************************************************************************
 * @brief   Gather and write double precision NetCDF field
 * @details Values are gathered to the master node and then written from the
 *          master node
 *****************************************************************************/
void
gather_put_nc_field_double_lake_only(int     nc_id,
                           int     var_id,
                           double  fillval,
                           size_t *start,
                           size_t *count,
                           double *var)
{
    extern int           mpi_rank;
    extern domain_struct global_domain;
    int                  status;
    size_t               grid_size;
    double              *dvar = NULL;

    // Allocate memory
    if (mpi_rank == VIC_MPI_ROOT) {
        grid_size = global_domain.nlakes_active;
        dvar = malloc(grid_size * sizeof(*dvar));
        check_alloc_status(dvar, "Memory allocation error.");
    }

    // Gather results from the nodes
    gather_field_double_lake_only(fillval, dvar, var);

    // Write to netcdf
    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_put_vara_double(nc_id, var_id, start, count, dvar);
        check_nc_status(status, "Error writing values.");
        // cleanup
        free(dvar);
    }
}

/******************************************************************************
 * @brief   Gather and write double precision NetCDF field
 * @details Values are gathered to the master node and then written from the
 *          master node
 *****************************************************************************/
void
gather_put_nc_field_float_lake_only(int     nc_id,
                          int     var_id,
                          float   fillval,
                          size_t *start,
                          size_t *count,
                          float  *var)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_lake_global_array_offsets;
    extern int          *mpi_lake_local_array_sizes;
    extern size_t       *mpi_lake_mapping_array;
    int                  status;
    float               *fvar = NULL;
    float               *fvar_gathered = NULL;
    size_t               grid_size;
    size_t               i;

    if (mpi_rank == VIC_MPI_ROOT) {
        grid_size = global_domain.nlakes_active;
        fvar = malloc(grid_size * sizeof(*fvar));
        check_alloc_status(fvar, "Memory allocation error.");
        for (i = 0; i < grid_size; i++) {
            fvar[i] = fillval;
        }
        fvar_gathered =
            malloc(global_domain.nlakes_active * sizeof(*fvar_gathered));
        check_alloc_status(fvar_gathered, "Memory allocation error.");
    }
    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(var, local_domain.nlakes_active, MPI_FLOAT,
                         fvar_gathered, mpi_lake_local_array_sizes,
                         mpi_lake_global_array_offsets, MPI_FLOAT,
                         VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "Error with gather of floats");

    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof(float), global_domain.nlakes_active, NULL,
            mpi_lake_mapping_array, fvar_gathered, fvar);

        // write to file
        status = nc_put_vara_float(nc_id, var_id, start, count, fvar);
        check_nc_status(status, "Error writing values");

        // cleanup
        free(fvar);
        free(fvar_gathered);
    }
}

/******************************************************************************
 * @brief   Gather and write integer NetCDF field
 * @details Values are gathered to the master node and then written from the
 *          master node
 *****************************************************************************/
void
gather_put_nc_field_int_lake_only(int     nc_id,
                        int     var_id,
                        int     fillval,
                        size_t *start,
                        size_t *count,
                        int    *var)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_lake_global_array_offsets;
    extern int          *mpi_lake_local_array_sizes;
    extern size_t       *mpi_lake_mapping_array;
    int                  status;
    int                 *ivar = NULL;
    int                 *ivar_gathered = NULL;
    size_t               grid_size;
    size_t               i;

    if (mpi_rank == VIC_MPI_ROOT) {
        grid_size = global_domain.nlakes_active;
        ivar = malloc(grid_size * sizeof(*ivar));
        check_alloc_status(ivar, "Memory allocation error.");

        for (i = 0; i < grid_size; i++) {
            ivar[i] = fillval;
        }

        ivar_gathered =
            malloc(global_domain.nlakes_active * sizeof(*ivar_gathered));
        check_alloc_status(ivar_gathered, "Memory allocation error.");
    }
    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(var, local_domain.nlakes_active, MPI_INT,
                         ivar_gathered, mpi_lake_local_array_sizes,
                         mpi_lake_global_array_offsets, MPI_INT,
                         VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof(int), global_domain.nlakes_active, NULL,
            mpi_lake_mapping_array,
            ivar_gathered, ivar);
        // write to file
        status = nc_put_vara_int(nc_id, var_id, start, count, ivar);
        check_nc_status(status, "Error writing values");

        // cleanup
        free(ivar);
        free(ivar_gathered);
    }
}

/******************************************************************************
 * @brief   Gather and write short integer NetCDF field
 * @details Values are gathered to the master node and then written from the
 *          master node
 *****************************************************************************/
void
gather_put_nc_field_short_lake_only(int        nc_id,
                          int        var_id,
                          short int  fillval,
                          size_t    *start,
                          size_t    *count,
                          short int *var)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_lake_global_array_offsets;
    extern int          *mpi_lake_local_array_sizes;
    extern size_t       *mpi_lake_mapping_array;
    int                  status;
    short int           *svar = NULL;
    short int           *svar_gathered = NULL;
    size_t               grid_size;
    size_t               i;

    if (mpi_rank == VIC_MPI_ROOT) {
        grid_size = global_domain.nlakes_active;
        svar = malloc(grid_size * sizeof(*svar));
        check_alloc_status(svar, "Memory allocation error.");

        for (i = 0; i < grid_size; i++) {
            svar[i] = fillval;
        }

        svar_gathered =
            malloc(global_domain.nlakes_active * sizeof(*svar_gathered));
        check_alloc_status(svar_gathered, "Memory allocation error.");
    }
    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(var, local_domain.nlakes_active, MPI_SHORT,
                         svar_gathered, mpi_lake_local_array_sizes,
                         mpi_lake_global_array_offsets, MPI_SHORT,
                         VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof(short int), global_domain.nlakes_active, NULL,
            mpi_lake_mapping_array,
            svar_gathered, svar);
        // write to file
        status = nc_put_vara_short(nc_id, var_id, start, count, svar);
        check_nc_status(status, "Error writing values");

        // cleanup
        free(svar);
        free(svar_gathered);
    }
}

/******************************************************************************
 * @brief   Gather and write signed character NetCDF field
 * @details Values are gathered to the master node and then written from the
 *          master node
 *****************************************************************************/
void
gather_put_nc_field_schar_lake_only(int     nc_id,
                          int     var_id,
                          char    fillval,
                          size_t *start,
                          size_t *count,
                          char   *var)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_lake_global_array_offsets;
    extern int          *mpi_lake_local_array_sizes;
    extern size_t       *mpi_lake_mapping_array;
    int                  status;
    signed char         *cvar = NULL;
    signed char         *cvar_gathered = NULL;
    size_t               grid_size;
    size_t               i;

    if (mpi_rank == VIC_MPI_ROOT) {
        grid_size = global_domain.nlakes_active;
        cvar = malloc(grid_size * sizeof(*cvar));
        check_alloc_status(cvar, "Memory allocation error.");

        for (i = 0; i < grid_size; i++) {
            cvar[i] = fillval;
        }

        cvar_gathered =
            malloc(global_domain.nlakes_active * sizeof(*cvar_gathered));
        check_alloc_status(cvar_gathered, "Memory allocation error.");
    }
    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(var, local_domain.nlakes_active, MPI_CHAR,
                         cvar_gathered, mpi_lake_local_array_sizes,
                         mpi_lake_global_array_offsets, MPI_CHAR,
                         VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof(char), global_domain.nlakes_active, NULL,
            mpi_lake_mapping_array,
            cvar_gathered, cvar);
        // write to file
        status = nc_put_vara_schar(nc_id, var_id, start, count, cvar);
        check_nc_status(status, "Error writing values");

        // cleanup
        free(cvar);
        free(cvar_gathered);
    }
}

/******************************************************************************
 * @brief   Scatter double precision variable
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void
scatter_field_double_lake_only(double *dvar,
                     double *var)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_lake_global_array_offsets;
    extern int          *mpi_lake_local_array_sizes;
    extern size_t       *mpi_lake_mapping_array;
    int                  status;
    double              *dvar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        dvar_mapped =
            malloc(global_domain.nlakes_active * sizeof(*dvar_mapped));
        check_alloc_status(dvar_mapped, "Memory allocation error.");

        // map to prepare for MPI_Scatterv
        map(sizeof(double), global_domain.nlakes_active, mpi_lake_mapping_array,
            NULL, dvar, dvar_mapped);
        free(dvar);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(dvar_mapped, mpi_lake_local_array_sizes,
                          mpi_lake_global_array_offsets, MPI_DOUBLE,
                          var, local_domain.nlakes_active, MPI_DOUBLE,
                          VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(dvar_mapped);
    }
}

/******************************************************************************
 * @brief   Read double precision NetCDF field from file and scatter
 * @details Read happens on the master node and is then scattered to the local
 *          nodes
 *****************************************************************************/
void
get_scatter_nc_field_double_lake_only(nameid_struct *nc_nameid,
                            char          *var_name,
                            size_t        *start,
                            size_t        *count,
                            double        *var)
{
    extern domain_struct global_domain;
    extern int           mpi_rank;
    double              *dvar = NULL;

    // Read variable from netcdf
    if (mpi_rank == VIC_MPI_ROOT) {
        dvar = malloc(global_domain.nlakes_active * sizeof(*dvar));
        check_alloc_status(dvar, "Memory allocation error.");

        get_nc_field_double(nc_nameid, var_name, start, count, dvar);
    }

    // Scatter results to nodes
    scatter_field_double_lake_only(dvar, var);
}

/******************************************************************************
 * @brief   Read single precision NetCDF field from file and scatter
 * @details Read happens on the master node and is then scattered to the local
 *          nodes
 *****************************************************************************/
void
get_scatter_nc_field_float_lake_only(nameid_struct *nc_nameid,
                           char          *var_name,
                           size_t        *start,
                           size_t        *count,
                           float         *var)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_lake_global_array_offsets;
    extern int          *mpi_lake_local_array_sizes;
    extern size_t       *mpi_lake_mapping_array;
    int                  status;
    float               *fvar = NULL;
    float               *fvar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        fvar = malloc(global_domain.nlakes_active * sizeof(*fvar));
        check_alloc_status(fvar, "Memory allocation error.");

        fvar_mapped =
            malloc(global_domain.nlakes_active * sizeof(*fvar_mapped));
        check_alloc_status(fvar_mapped, "Memory allocation error.");

        get_nc_field_float(nc_nameid, var_name, start, count, fvar);
        // map to prepare for MPI_Scatterv
        map(sizeof(float), global_domain.nlakes_active, mpi_lake_mapping_array,
            NULL,
            fvar, fvar_mapped);
        free(fvar);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(fvar_mapped, mpi_lake_local_array_sizes,
                          mpi_lake_global_array_offsets, MPI_FLOAT,
                          var, local_domain.nlakes_active, MPI_FLOAT,
                          VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(fvar_mapped);
    }
}

/******************************************************************************
 * @brief   Read integer NetCDF field from file and scatter
 * @details Read happens on the master node and is then scattered to the local
 *          nodes
 *****************************************************************************/
void
get_scatter_nc_field_int_lake_only(nameid_struct *nc_nameid,
                         char          *var_name,
                         size_t        *start,
                         size_t        *count,
                         int           *var)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_lake_global_array_offsets;
    extern int          *mpi_lake_local_array_sizes;
    extern size_t       *mpi_lake_mapping_array;
    int                  status;
    int                 *ivar = NULL;
    int                 *ivar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        ivar = malloc(global_domain.nlakes_active * sizeof(*ivar));
        check_alloc_status(ivar, "Memory allocation error.");

        ivar_mapped =
            malloc(global_domain.nlakes_active * sizeof(*ivar_mapped));
        check_alloc_status(ivar_mapped, "Memory allocation error.");

        get_nc_field_int(nc_nameid, var_name, start, count, ivar);
        // map to prepare for MPI_Scatterv
        map(sizeof(int), global_domain.nlakes_active, mpi_lake_mapping_array,
            NULL,
            ivar, ivar_mapped);
        free(ivar);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(ivar_mapped, mpi_lake_local_array_sizes,
                          mpi_lake_global_array_offsets, MPI_INT,
                          var, local_domain.nlakes_active, MPI_INT,
                          VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(ivar_mapped);
    }
}
