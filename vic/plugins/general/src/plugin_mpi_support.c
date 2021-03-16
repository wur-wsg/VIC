/******************************************************************************
 * @section DESCRIPTION
 *
 * MPI support routines for plugins
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

/******************************************************************************
 * @brief   Gather active double precision variable
 * @details Values are gathered to the master node
 *****************************************************************************/
void
gather_double(double *dvar,
              double *var_local)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_map_global_array_offsets;
    extern int          *mpi_map_local_array_sizes;
    extern size_t       *mpi_map_mapping_array;
    int                  status;
    double              *dvar_gathered = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        dvar_gathered =
            malloc(global_domain.ncells_active * sizeof(*dvar_gathered));
        check_alloc_status(dvar_gathered, "Memory allocation error.");
    }

    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(var_local, local_domain.ncells_active, MPI_DOUBLE,
                         dvar_gathered, mpi_map_local_array_sizes,
                         mpi_map_global_array_offsets, MPI_DOUBLE,
                         VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof(double), global_domain.ncells_active, NULL,
            mpi_map_mapping_array, dvar_gathered, dvar);

        // cleanup
        free(dvar_gathered);
    }
}

/******************************************************************************
 * @brief   Gather active double precision variable recursive
 * @details Values are gathered to the master node
 *****************************************************************************/
void
gather_double_2d(double **dvar,
                 double **var_local,
                 int      depth)
{
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;

    double              *tmp_global = NULL;
    double              *tmp_local = NULL;

    size_t               i;
    size_t               j;

    if (mpi_rank == VIC_MPI_ROOT) {
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");

    for (i = 0; i < (size_t) depth; i++) {
        for (j = 0; j < local_domain.ncells_active; j++) {
            tmp_local[j] = var_local[j][i];
        }

        gather_double(tmp_global, tmp_local);


        if (mpi_rank == VIC_MPI_ROOT) {
            for (j = 0; j < global_domain.ncells_active; j++) {
                dvar[j][i] = tmp_global[j];
            }
        }
    }

    if (mpi_rank == VIC_MPI_ROOT) {
        free(tmp_global);
    }
    free(tmp_local);
}

/******************************************************************************
 * @brief   Gather active integer variable
 * @details Values are gathered to the master node
 *****************************************************************************/
void
gather_int(int *ivar,
           int *ivar_local)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_map_global_array_offsets;
    extern int          *mpi_map_local_array_sizes;
    extern size_t       *mpi_map_mapping_array;
    int                  status;
    int                 *ivar_gathered = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        ivar_gathered =
            malloc(global_domain.ncells_active * sizeof(*ivar_gathered));
        check_alloc_status(ivar_gathered, "Memory allocation error.");
    }

    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(ivar_local, local_domain.ncells_active, MPI_INT,
                         ivar_gathered, mpi_map_local_array_sizes,
                         mpi_map_global_array_offsets, MPI_INT,
                         VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof(int), global_domain.ncells_active, NULL,
            mpi_map_mapping_array, ivar_gathered, ivar);

        // cleanup
        free(ivar_gathered);
    }
}

/******************************************************************************
 * @brief   Gather integer variable recursive
 * @details Values are gathered to the master node
 *****************************************************************************/
void
gather_int_2d(int **ivar,
              int **var_local,
              int   depth)
{
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;

    int                 *tmp_global = NULL;
    int                 *tmp_local = NULL;

    size_t               i;
    size_t               j;

    if (mpi_rank == VIC_MPI_ROOT) {
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");

    for (i = 0; i < (size_t) depth; i++) {
        for (j = 0; j < local_domain.ncells_active; j++) {
            tmp_local[j] = var_local[j][i];
        }

        gather_int(tmp_global, tmp_local);


        if (mpi_rank == VIC_MPI_ROOT) {
            for (j = 0; j < global_domain.ncells_active; j++) {
                ivar[j][i] = tmp_global[j];
            }
        }
    }

    if (mpi_rank == VIC_MPI_ROOT) {
        free(tmp_global);
    }
    free(tmp_local);
}

/******************************************************************************
 * @brief   Gather active size_t variable
 * @details Values are gathered to the master node
 *****************************************************************************/
void
gather_size_t(size_t *svar,
              size_t *svar_local)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_map_global_array_offsets;
    extern int          *mpi_map_local_array_sizes;
    extern size_t       *mpi_map_mapping_array;
    int                  status;
    size_t              *svar_gathered = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        svar_gathered =
            malloc(global_domain.ncells_active * sizeof(*svar_gathered));
        check_alloc_status(svar_gathered, "Memory allocation error.");
    }

    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(svar_local, local_domain.ncells_active, MPI_AINT,
                         svar_gathered, mpi_map_local_array_sizes,
                         mpi_map_global_array_offsets, MPI_AINT,
                         VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof(size_t), global_domain.ncells_active, NULL,
            mpi_map_mapping_array, svar_gathered, svar);

        // cleanup
        free(svar_gathered);
    }
}

/******************************************************************************
 * @brief   Gather size_t variable recursive
 * @details Values are gathered to the master node
 *****************************************************************************/
void
gather_size_t_2d(size_t **svar,
                 size_t **var_local,
                 int      depth)
{
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;

    size_t              *tmp_global = NULL;
    size_t              *tmp_local = NULL;

    size_t               i;
    size_t               j;

    if (mpi_rank == VIC_MPI_ROOT) {
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");

    for (i = 0; i < (size_t) depth; i++) {
        for (j = 0; j < local_domain.ncells_active; j++) {
            tmp_local[j] = var_local[j][i];
        }

        gather_size_t(tmp_global, tmp_local);


        if (mpi_rank == VIC_MPI_ROOT) {
            for (j = 0; j < global_domain.ncells_active; j++) {
                svar[j][i] = tmp_global[j];
            }
        }
    }

    if (mpi_rank == VIC_MPI_ROOT) {
        free(tmp_global);
    }
    free(tmp_local);
}

/******************************************************************************
 * @brief   Scatter double precision variable
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void
scatter_double(double *dvar,
               double *var_local)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_map_global_array_offsets;
    extern int          *mpi_map_local_array_sizes;
    extern size_t       *mpi_map_mapping_array;
    int                  status;
    double              *dvar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        dvar_mapped =
            malloc(global_domain.ncells_active * sizeof(*dvar_mapped));
        check_alloc_status(dvar_mapped, "Memory allocation error.");

        // map to prepare for MPI_Scatterv
        map(sizeof(double), global_domain.ncells_active, mpi_map_mapping_array,
            NULL, dvar, dvar_mapped);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(dvar_mapped, mpi_map_local_array_sizes,
                          mpi_map_global_array_offsets, MPI_DOUBLE,
                          var_local, local_domain.ncells_active, MPI_DOUBLE,
                          VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(dvar_mapped);
    }
}

/******************************************************************************
 * @brief   Scatter double precision variable recursive
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void
scatter_double_2d(double **dvar,
                  double **var_local,
                  int      depth)
{
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;

    double              *tmp_global = NULL;
    double              *tmp_local = NULL;

    size_t               i;
    size_t               j;

    if (mpi_rank == VIC_MPI_ROOT) {
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");

    for (i = 0; i < (size_t) depth; i++) {
        if (mpi_rank == VIC_MPI_ROOT) {
            for (j = 0; j < global_domain.ncells_active; j++) {
                tmp_global[j] = dvar[j][i];
            }
        }

        scatter_double(tmp_global, tmp_local);

        for (j = 0; j < local_domain.ncells_active; j++) {
            var_local[j][i] = tmp_local[j];
        }
    }

    if (mpi_rank == VIC_MPI_ROOT) {
        free(tmp_global);
    }
    free(tmp_local);
}

/******************************************************************************
 * @brief   Scatter integer variable
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void
scatter_int(int *ivar,
            int *var_local)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_map_global_array_offsets;
    extern int          *mpi_map_local_array_sizes;
    extern size_t       *mpi_map_mapping_array;
    int                  status;
    int                 *ivar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        ivar_mapped =
            malloc(global_domain.ncells_active * sizeof(*ivar_mapped));
        check_alloc_status(ivar_mapped, "Memory allocation error.");

        // map to prepare for MPI_Scatterv
        map(sizeof(int), global_domain.ncells_active, mpi_map_mapping_array,
            NULL, ivar, ivar_mapped);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(ivar_mapped, mpi_map_local_array_sizes,
                          mpi_map_global_array_offsets, MPI_INT,
                          var_local, local_domain.ncells_active, MPI_INT,
                          VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(ivar_mapped);
    }
}

/******************************************************************************
 * @brief   Scatter integer variable recursive
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void
scatter_int_2d(int **ivar,
               int **var_local,
               int   depth)
{
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;

    int                 *tmp_global = NULL;
    int                 *tmp_local = NULL;

    size_t               i;
    size_t               j;

    if (mpi_rank == VIC_MPI_ROOT) {
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");

    for (i = 0; i < (size_t) depth; i++) {
        if (mpi_rank == VIC_MPI_ROOT) {
            for (j = 0; j < global_domain.ncells_active; j++) {
                tmp_global[j] = ivar[j][i];
            }
        }

        scatter_int(tmp_global, tmp_local);

        for (j = 0; j < local_domain.ncells_active; j++) {
            var_local[j][i] = tmp_local[j];
        }
    }

    if (mpi_rank == VIC_MPI_ROOT) {
        free(tmp_global);
    }
    free(tmp_local);
}

/******************************************************************************
 * @brief   Scatter size_t variable
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void
scatter_size_t(size_t *svar,
               size_t *var_local)
{
    extern MPI_Comm      MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;
    extern int          *mpi_map_global_array_offsets;
    extern int          *mpi_map_local_array_sizes;
    extern size_t       *mpi_map_mapping_array;
    int                  status;
    size_t              *svar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        svar_mapped =
            malloc(global_domain.ncells_active * sizeof(*svar_mapped));
        check_alloc_status(svar_mapped, "Memory allocation error.");

        // map to prepare for MPI_Scatterv
        map(sizeof(size_t), global_domain.ncells_active, mpi_map_mapping_array,
            NULL, svar, svar_mapped);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(svar_mapped, mpi_map_local_array_sizes,
                          mpi_map_global_array_offsets, MPI_AINT,
                          var_local, local_domain.ncells_active, MPI_AINT,
                          VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(svar_mapped);
    }
}

/******************************************************************************
 * @brief   Scatter size_t variable recursive
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void
scatter_size_t_2d(size_t **svar,
                  size_t **var_local,
                  int      depth)
{
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int           mpi_rank;

    size_t              *tmp_global = NULL;
    size_t              *tmp_local = NULL;

    size_t               i;
    size_t               j;

    if (mpi_rank == VIC_MPI_ROOT) {
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");

    for (i = 0; i < (size_t) depth; i++) {
        if (mpi_rank == VIC_MPI_ROOT) {
            for (j = 0; j < global_domain.ncells_active; j++) {
                tmp_global[j] = svar[j][i];
            }
        }

        scatter_size_t(tmp_global, tmp_local);

        for (j = 0; j < local_domain.ncells_active; j++) {
            var_local[j][i] = tmp_local[j];
        }
    }

    if (mpi_rank == VIC_MPI_ROOT) {
        free(tmp_global);
    }
    free(tmp_local);
}

/******************************************************************************
 * @brief    Read active double precision netCDF field from file.
 *****************************************************************************/
int
get_active_nc_field_double(nameid_struct *nc_nameid,
                           char          *var_name,
                           size_t        *start,
                           size_t        *count,
                           double        *var)
{
    extern domain_struct global_domain;
    extern size_t       *filter_active_cells;

    int                  status;
    double              *dvar = NULL;

    dvar =
        malloc(global_domain.ncells_total * sizeof(*dvar));
    check_alloc_status(dvar, "Memory allocation error.");

    status = get_nc_field_double(nc_nameid, var_name, start, count, dvar);
    check_nc_status(status, "Error getting values %s for %s",
                    var_name, nc_nameid->nc_filename);

    // filter the active cells only
    map(sizeof(double), global_domain.ncells_active, filter_active_cells, NULL,
        dvar, var);

    free(dvar);

    return status;
}

/******************************************************************************
 * @brief    Read active integer netCDF field from file.
 *****************************************************************************/
int
get_active_nc_field_int(nameid_struct *nc_nameid,
                        char          *var_name,
                        size_t        *start,
                        size_t        *count,
                        int           *var)
{
    extern domain_struct global_domain;
    extern size_t       *filter_active_cells;

    int                  status;
    int                 *ivar = NULL;

    ivar =
        malloc(global_domain.ncells_total * sizeof(*ivar));
    check_alloc_status(ivar, "Memory allocation error.");

    status = get_nc_field_int(nc_nameid, var_name, start, count, ivar);
    check_nc_status(status, "Error getting values %s for %s",
                    var_name, nc_nameid->nc_filename);

    // filter the active cells only
    map(sizeof(int), global_domain.ncells_active, filter_active_cells,
        NULL,
        ivar, var);

    free(ivar);

    return status;
}
