#include <vic_driver_image.h>
#include <plugin.h>
#include <dams.h>

/******************************************
* @brief   Set dam state metadata
******************************************/
void
dam_set_state_meta_data_info(void)
{
    extern metadata_struct state_metadata[];

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_ACTIVE].varname, MAXSTRING, "STATE_DAM_ACTIVE");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_ACTIVE].long_name, MAXSTRING, "dam_active");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_ACTIVE].standard_name, MAXSTRING, "dam_active");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_ACTIVE].units, MAXSTRING, "-");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_ACTIVE].description, MAXSTRING, "dam active flag");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].varname, MAXSTRING, "STATE_DAM_OP_MONTH");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].long_name, MAXSTRING, "dam_op_month");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].standard_name, MAXSTRING, "dam_op_month");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].units, MAXSTRING, "-");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].description, MAXSTRING, "dam operational month");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].varname, MAXSTRING, "STATE_DAM_STORAGE");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].long_name, MAXSTRING, "dam_storage");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].standard_name, MAXSTRING, "dam_storage");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].description, MAXSTRING, "dam storage");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW].varname, MAXSTRING, "STATE_DAM_TOTAL_INFLOW");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW].long_name, MAXSTRING, "dam_total_inflow");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW].standard_name, MAXSTRING, "dam_total_inflow");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW].description, MAXSTRING, "dam accumulated inflow");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND].varname, MAXSTRING, "STATE_DAM_TOTAL_DEMAND");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND].long_name, MAXSTRING, "dam_total_demand");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND].standard_name, MAXSTRING, "dam_total_demand");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND].description, MAXSTRING, "dam accumulated demand");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_EFR].varname, MAXSTRING, "STATE_DAM_TOTAL_EFR");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_EFR].long_name, MAXSTRING, "dam_total_efr");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_EFR].standard_name, MAXSTRING, "dam_total_efr");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_EFR].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_EFR].description, MAXSTRING, "dam accumulated environmental requirments");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_REGISTER_STEPS].varname, MAXSTRING, "STATE_DAM_REGISTER_STEPS");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_REGISTER_STEPS].long_name, MAXSTRING, "dam_register_steps");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_REGISTER_STEPS].standard_name, MAXSTRING, "dam_register_steps");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_REGISTER_STEPS].units, MAXSTRING, "-");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_REGISTER_STEPS].description, MAXSTRING, "dam number of accumulation steps");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING].varname, MAXSTRING, "STATE_DAM_MONTHS_RUNNING");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING].long_name, MAXSTRING, "dam_months_running");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING].standard_name, MAXSTRING, "dam_months_running");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING].units, MAXSTRING, "-");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING].description, MAXSTRING, "dam number of months history is registered");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].varname, MAXSTRING, "STATE_DAM_HISTORY_INFLOW");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].long_name, MAXSTRING, "dam_history_inflow");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].standard_name, MAXSTRING, "dam_history_inflow");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].description, MAXSTRING, "dam historical inflow array");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].varname, MAXSTRING, "STATE_DAM_HISTORY_DEMAND");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].long_name, MAXSTRING, "dam_history_demand");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].standard_name, MAXSTRING, "dam_history_demand");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].description, MAXSTRING, "dam historical demand array");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].varname, MAXSTRING, "STATE_DAM_HISTORY_EFR");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].long_name, MAXSTRING, "dam_history_efr");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].standard_name, MAXSTRING, "dam_history_efr");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].description, MAXSTRING, "dam historical environmental requirment array");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].varname, MAXSTRING, "STATE_DAM_OP_RELEASE");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].long_name, MAXSTRING, "dam_op_release");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].standard_name, MAXSTRING, "dam_op_release");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].description, MAXSTRING, "dam calculated release array");

    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].varname, MAXSTRING, "STATE_DAM_OP_STORAGE");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].long_name, MAXSTRING, "dam_op_storage");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].standard_name, MAXSTRING, "dam_op_storage");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].units, MAXSTRING, "hm3");
    snprintf(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].description, MAXSTRING, "dam calculated storage array");
}

/******************************************
* @brief   Add dam dimensions to state file
*
* Compact layout: dimensions are dam_class, dam_hist, dam_month only.
* No lat/lon -- state is stored per-dam, not per-grid-cell.
* This reduces dam state from [NDAMTYPES x nj x ni] -> [NDAMTYPES],
* saving ~36000x for a 180x204 domain.
******************************************/
void
dam_add_state_dim(char *filename, nc_file_struct *nc_state_file)
{
    extern plugin_option_struct plugin_options;
    int status;

    nc_state_file->dam_size       = plugin_options.NDAMTYPES;
    nc_state_file->dam_hist_size  = MONTHS_PER_YEAR * DAM_HIST_YEARS;
    nc_state_file->dam_month_size = MONTHS_PER_YEAR;

    status = nc_def_dim(nc_state_file->nc_id, "dam_class",
                        nc_state_file->dam_size,
                        &(nc_state_file->dam_dimid));
    check_nc_status(status, "Error defining dam_class dim in %s", filename);

    status = nc_def_dim(nc_state_file->nc_id, "dam_hist",
                        nc_state_file->dam_hist_size,
                        &(nc_state_file->dam_hist_dimid));
    check_nc_status(status, "Error defining dam_hist dim in %s", filename);

    status = nc_def_dim(nc_state_file->nc_id, "dam_month",
                        nc_state_file->dam_month_size,
                        &(nc_state_file->dam_month_dimid));
    check_nc_status(status, "Error defining dam_month dim in %s", filename);
}

/******************************************
* @brief   Set state file dimension size
******************************************/
void
dam_set_nc_state_file_info(nc_file_struct *nc_state_file)
{
    extern plugin_option_struct plugin_options;

    nc_state_file->dam_size       = plugin_options.NDAMTYPES;
    nc_state_file->dam_hist_size  = MONTHS_PER_YEAR * DAM_HIST_YEARS;
    nc_state_file->dam_month_size = MONTHS_PER_YEAR;
}

/******************************************
* @brief   Set state variable NC dimensions for dam variables
*
* Compact layout:
*   Scalar vars  -> [dam_class]
*   History vars -> [dam_class x dam_hist]
*   Month vars   -> [dam_class x dam_month]
******************************************/
void
dam_set_nc_state_var_info(nc_file_struct *nc, size_t varid)
{
    switch(varid) {
        case N_STATE_VARS + STATE_DAM_ACTIVE:
        case N_STATE_VARS + STATE_DAM_OP_MONTH:
        case N_STATE_VARS + STATE_DAM_STORAGE:
        case N_STATE_VARS + STATE_DAM_TOTAL_INFLOW:
        case N_STATE_VARS + STATE_DAM_TOTAL_DEMAND:
        case N_STATE_VARS + STATE_DAM_TOTAL_EFR:
        case N_STATE_VARS + STATE_DAM_REGISTER_STEPS:
        case N_STATE_VARS + STATE_DAM_MONTHS_RUNNING:
            /* 1D: [dam_class] */
            nc->nc_vars[varid].nc_dims      = 1;
            nc->nc_vars[varid].nc_dimids[0] = nc->dam_dimid;
            nc->nc_vars[varid].nc_dimids[1] = -1;
            nc->nc_vars[varid].nc_counts[0] = nc->dam_size;
            nc->nc_vars[varid].nc_counts[1] = 0;
            break;

        case N_STATE_VARS + STATE_DAM_HISTORY_INFLOW:
        case N_STATE_VARS + STATE_DAM_HISTORY_DEMAND:
        case N_STATE_VARS + STATE_DAM_HISTORY_EFR:
            /* 2D: [dam_class x dam_hist] */
            nc->nc_vars[varid].nc_dims      = 2;
            nc->nc_vars[varid].nc_dimids[0] = nc->dam_dimid;
            nc->nc_vars[varid].nc_dimids[1] = nc->dam_hist_dimid;
            nc->nc_vars[varid].nc_counts[0] = nc->dam_size;
            nc->nc_vars[varid].nc_counts[1] = nc->dam_hist_size;
            break;

        case N_STATE_VARS + STATE_DAM_OP_RELEASE:
        case N_STATE_VARS + STATE_DAM_OP_STORAGE:
            /* 2D: [dam_class x dam_month] */
            nc->nc_vars[varid].nc_dims      = 2;
            nc->nc_vars[varid].nc_dimids[0] = nc->dam_dimid;
            nc->nc_vars[varid].nc_dimids[1] = nc->dam_month_dimid;
            nc->nc_vars[varid].nc_counts[0] = nc->dam_size;
            nc->nc_vars[varid].nc_counts[1] = nc->dam_month_size;
            break;

        default:
            break;
    }
}

/******************************************
* @brief   Store dam state
*
* Compact layout: one entry per dam_class (not per grid cell).
* MPI_Reduce(SUM) gathers values from whichever rank owns each dam.
* Root writes the 1D/2D compact arrays to the state file.
******************************************/
void
dam_store(nc_file_struct *state_file)
{
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;
    extern dam_con_map_struct  *dam_con_map;
    extern dam_var_struct     **dam_var;
    extern MPI_Comm             MPI_COMM_VIC;
    extern int                  mpi_rank;

    size_t  i, j, iDam, m;
    int     ncid           = state_file->nc_id;
    int     status;
    size_t  ND             = plugin_options.NDAMTYPES;
    size_t  dam_hist_size  = state_file->dam_hist_size;
    size_t  dam_month_size = state_file->dam_month_size;

    double *local_scalar   = calloc(ND, sizeof(double));
    double *global_scalar  = calloc(ND, sizeof(double));
    double *local_hist     = calloc(ND * dam_hist_size,  sizeof(double));
    double *global_hist    = calloc(ND * dam_hist_size,  sizeof(double));
    double *local_month    = calloc(ND * dam_month_size, sizeof(double));
    double *global_month   = calloc(ND * dam_month_size, sizeof(double));
    check_alloc_status(local_scalar,  "Memory allocation error");
    check_alloc_status(global_scalar, "Memory allocation error");
    check_alloc_status(local_hist,    "Memory allocation error");
    check_alloc_status(global_hist,   "Memory allocation error");
    check_alloc_status(local_month,   "Memory allocation error");
    check_alloc_status(global_month,  "Memory allocation error");

/* Helper: fill local_scalar for one field, reduce to root, write 1D */
#define DAM_STORE_SCALAR(VAR_ENUM, FIELD, CAST) \
    do { \
        memset(local_scalar,  0, ND * sizeof(double)); \
        memset(global_scalar, 0, ND * sizeof(double)); \
        for (i = 0; i < local_domain.ncells_active; i++) { \
            for (j = 0; j < ND; j++) { \
                if (dam_con_map[i].didx[j] != NODATA_DAM) { \
                    iDam = (size_t)dam_con_map[i].didx[j]; \
                    local_scalar[j] = (double)((CAST)dam_var[i][iDam].FIELD); \
                } \
            } \
        } \
        status = MPI_Reduce(local_scalar, global_scalar, (int)ND, \
                            MPI_DOUBLE, MPI_SUM, VIC_MPI_ROOT, MPI_COMM_VIC); \
        check_mpi_status(status, "MPI error in dam_store"); \
        if (mpi_rank == VIC_MPI_ROOT) { \
            int _vid = state_file->nc_vars[N_STATE_VARS + (VAR_ENUM)].nc_varid; \
            size_t _s[1] = {0}, _c[1] = {ND}; \
            status = nc_put_vara_double(ncid, _vid, _s, _c, global_scalar); \
            check_nc_status(status, "Error writing dam state scalar"); \
        } \
    } while (0)

    DAM_STORE_SCALAR(STATE_DAM_ACTIVE,         active,         double);
    DAM_STORE_SCALAR(STATE_DAM_OP_MONTH,       op_month,       double);
    DAM_STORE_SCALAR(STATE_DAM_STORAGE,        storage,        double);
    DAM_STORE_SCALAR(STATE_DAM_TOTAL_INFLOW,   total_inflow,   double);
    DAM_STORE_SCALAR(STATE_DAM_TOTAL_DEMAND,   total_demand,   double);
    DAM_STORE_SCALAR(STATE_DAM_TOTAL_EFR,      total_efr,      double);
    DAM_STORE_SCALAR(STATE_DAM_REGISTER_STEPS, register_steps, double);
    DAM_STORE_SCALAR(STATE_DAM_MONTHS_RUNNING, months_running, double);
#undef DAM_STORE_SCALAR

/* Helper: fill local_hist for one field, reduce to root, write 2D [ND x hist] */
#define DAM_STORE_HIST(VAR_ENUM, FIELD) \
    do { \
        memset(local_hist,  0, ND * dam_hist_size * sizeof(double)); \
        memset(global_hist, 0, ND * dam_hist_size * sizeof(double)); \
        for (i = 0; i < local_domain.ncells_active; i++) { \
            for (j = 0; j < ND; j++) { \
                if (dam_con_map[i].didx[j] != NODATA_DAM) { \
                    iDam = (size_t)dam_con_map[i].didx[j]; \
                    for (m = 0; m < dam_hist_size; m++) \
                        local_hist[j * dam_hist_size + m] = dam_var[i][iDam].FIELD[m]; \
                } \
            } \
        } \
        status = MPI_Reduce(local_hist, global_hist, (int)(ND * dam_hist_size), \
                            MPI_DOUBLE, MPI_SUM, VIC_MPI_ROOT, MPI_COMM_VIC); \
        check_mpi_status(status, "MPI error in dam_store hist"); \
        if (mpi_rank == VIC_MPI_ROOT) { \
            int _vid = state_file->nc_vars[N_STATE_VARS + (VAR_ENUM)].nc_varid; \
            size_t _s[2] = {0, 0}, _c[2] = {ND, dam_hist_size}; \
            status = nc_put_vara_double(ncid, _vid, _s, _c, global_hist); \
            check_nc_status(status, "Error writing dam state history"); \
        } \
    } while (0)

    DAM_STORE_HIST(STATE_DAM_HISTORY_INFLOW, history_inflow);
    DAM_STORE_HIST(STATE_DAM_HISTORY_DEMAND, history_demand);
    DAM_STORE_HIST(STATE_DAM_HISTORY_EFR,   history_efr);
#undef DAM_STORE_HIST

/* Helper: fill local_month for one field, reduce to root, write 2D [ND x month] */
#define DAM_STORE_MONTH(VAR_ENUM, FIELD) \
    do { \
        memset(local_month,  0, ND * dam_month_size * sizeof(double)); \
        memset(global_month, 0, ND * dam_month_size * sizeof(double)); \
        for (i = 0; i < local_domain.ncells_active; i++) { \
            for (j = 0; j < ND; j++) { \
                if (dam_con_map[i].didx[j] != NODATA_DAM) { \
                    iDam = (size_t)dam_con_map[i].didx[j]; \
                    for (m = 0; m < dam_month_size; m++) \
                        local_month[j * dam_month_size + m] = dam_var[i][iDam].FIELD[m]; \
                } \
            } \
        } \
        status = MPI_Reduce(local_month, global_month, (int)(ND * dam_month_size), \
                            MPI_DOUBLE, MPI_SUM, VIC_MPI_ROOT, MPI_COMM_VIC); \
        check_mpi_status(status, "MPI error in dam_store month"); \
        if (mpi_rank == VIC_MPI_ROOT) { \
            int _vid = state_file->nc_vars[N_STATE_VARS + (VAR_ENUM)].nc_varid; \
            size_t _s[2] = {0, 0}, _c[2] = {ND, dam_month_size}; \
            status = nc_put_vara_double(ncid, _vid, _s, _c, global_month); \
            check_nc_status(status, "Error writing dam state month"); \
        } \
    } while (0)

    DAM_STORE_MONTH(STATE_DAM_OP_RELEASE, op_release);
    DAM_STORE_MONTH(STATE_DAM_OP_STORAGE, op_storage);
#undef DAM_STORE_MONTH

    free(local_scalar);   free(global_scalar);
    free(local_hist);     free(global_hist);
    free(local_month);    free(global_month);
}

/******************************************
* @brief   Restore dam state
*
* Root reads compact 1D/2D arrays and broadcasts to all ranks.
* Each rank scatters values into its local dam_var entries.
******************************************/
void
dam_restore(void)
{
    extern domain_struct        local_domain;
    extern plugin_option_struct plugin_options;
    extern dam_con_map_struct  *dam_con_map;
    extern dam_var_struct     **dam_var;
    extern filenames_struct     filenames;
    extern metadata_struct      state_metadata[];
    extern MPI_Comm             MPI_COMM_VIC;
    extern int                  mpi_rank;

    size_t  i, j, iDam, m;
    int     status;
    int     ncid           = -1;
    size_t  ND             = plugin_options.NDAMTYPES;
    size_t  dam_hist_size  = MONTHS_PER_YEAR * DAM_HIST_YEARS;
    size_t  dam_month_size = MONTHS_PER_YEAR;

    double *scalar_buf = malloc(ND * sizeof(double));
    double *hist_buf   = malloc(ND * dam_hist_size  * sizeof(double));
    double *month_buf  = malloc(ND * dam_month_size * sizeof(double));
    check_alloc_status(scalar_buf, "Memory allocation error");
    check_alloc_status(hist_buf,   "Memory allocation error");
    check_alloc_status(month_buf,  "Memory allocation error");

    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_open(filenames.init_state.nc_filename, NC_NOWRITE, &ncid);
        check_nc_status(status, "Error opening state file %s",
                        filenames.init_state.nc_filename);
    }

/* Restore a 1D scalar field: root reads, all ranks receive via Bcast, scatter */
#define DAM_RESTORE_SCALAR(VARNAME, FIELD, CAST) \
    do { \
        if (mpi_rank == VIC_MPI_ROOT) { \
            int _vid; \
            size_t _s[1] = {0}, _c[1] = {ND}; \
            status = nc_inq_varid(ncid, (VARNAME), &_vid); \
            check_nc_status(status, "Error finding variable %s", (VARNAME)); \
            status = nc_get_vara_double(ncid, _vid, _s, _c, scalar_buf); \
            check_nc_status(status, "Error reading variable %s", (VARNAME)); \
        } \
        status = MPI_Bcast(scalar_buf, (int)ND, MPI_DOUBLE, \
                           VIC_MPI_ROOT, MPI_COMM_VIC); \
        check_mpi_status(status, "MPI error in dam_restore scalar"); \
        for (i = 0; i < local_domain.ncells_active; i++) { \
            for (j = 0; j < ND; j++) { \
                if (dam_con_map[i].didx[j] != NODATA_DAM) { \
                    iDam = (size_t)dam_con_map[i].didx[j]; \
                    dam_var[i][iDam].FIELD = (CAST)scalar_buf[j]; \
                } \
            } \
        } \
    } while (0)

    DAM_RESTORE_SCALAR(state_metadata[N_STATE_VARS + STATE_DAM_ACTIVE].varname,
                       active,         bool);
    DAM_RESTORE_SCALAR(state_metadata[N_STATE_VARS + STATE_DAM_OP_MONTH].varname,
                       op_month,       int);
    DAM_RESTORE_SCALAR(state_metadata[N_STATE_VARS + STATE_DAM_STORAGE].varname,
                       storage,        double);
    DAM_RESTORE_SCALAR(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_INFLOW].varname,
                       total_inflow,   double);
    DAM_RESTORE_SCALAR(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_DEMAND].varname,
                       total_demand,   double);
    DAM_RESTORE_SCALAR(state_metadata[N_STATE_VARS + STATE_DAM_TOTAL_EFR].varname,
                       total_efr,      double);
    DAM_RESTORE_SCALAR(state_metadata[N_STATE_VARS + STATE_DAM_REGISTER_STEPS].varname,
                       register_steps, size_t);
    DAM_RESTORE_SCALAR(state_metadata[N_STATE_VARS + STATE_DAM_MONTHS_RUNNING].varname,
                       months_running, size_t);
#undef DAM_RESTORE_SCALAR

/* Restore a 2D history field: root reads full [ND x hist], all receive via Bcast */
#define DAM_RESTORE_HIST(VARNAME, FIELD) \
    do { \
        if (mpi_rank == VIC_MPI_ROOT) { \
            int _vid; \
            size_t _s[2] = {0, 0}, _c[2] = {ND, dam_hist_size}; \
            status = nc_inq_varid(ncid, (VARNAME), &_vid); \
            check_nc_status(status, "Error finding variable %s", (VARNAME)); \
            status = nc_get_vara_double(ncid, _vid, _s, _c, hist_buf); \
            check_nc_status(status, "Error reading variable %s", (VARNAME)); \
        } \
        status = MPI_Bcast(hist_buf, (int)(ND * dam_hist_size), MPI_DOUBLE, \
                           VIC_MPI_ROOT, MPI_COMM_VIC); \
        check_mpi_status(status, "MPI error in dam_restore hist"); \
        for (i = 0; i < local_domain.ncells_active; i++) { \
            for (j = 0; j < ND; j++) { \
                if (dam_con_map[i].didx[j] != NODATA_DAM) { \
                    iDam = (size_t)dam_con_map[i].didx[j]; \
                    for (m = 0; m < dam_hist_size; m++) \
                        dam_var[i][iDam].FIELD[m] = hist_buf[j * dam_hist_size + m]; \
                } \
            } \
        } \
    } while (0)

    DAM_RESTORE_HIST(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_INFLOW].varname,
                     history_inflow);
    DAM_RESTORE_HIST(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_DEMAND].varname,
                     history_demand);
    DAM_RESTORE_HIST(state_metadata[N_STATE_VARS + STATE_DAM_HISTORY_EFR].varname,
                     history_efr);
#undef DAM_RESTORE_HIST

/* Restore a 2D month field: root reads full [ND x month], all receive via Bcast */
#define DAM_RESTORE_MONTH(VARNAME, FIELD) \
    do { \
        if (mpi_rank == VIC_MPI_ROOT) { \
            int _vid; \
            size_t _s[2] = {0, 0}, _c[2] = {ND, dam_month_size}; \
            status = nc_inq_varid(ncid, (VARNAME), &_vid); \
            check_nc_status(status, "Error finding variable %s", (VARNAME)); \
            status = nc_get_vara_double(ncid, _vid, _s, _c, month_buf); \
            check_nc_status(status, "Error reading variable %s", (VARNAME)); \
        } \
        status = MPI_Bcast(month_buf, (int)(ND * dam_month_size), MPI_DOUBLE, \
                           VIC_MPI_ROOT, MPI_COMM_VIC); \
        check_mpi_status(status, "MPI error in dam_restore month"); \
        for (i = 0; i < local_domain.ncells_active; i++) { \
            for (j = 0; j < ND; j++) { \
                if (dam_con_map[i].didx[j] != NODATA_DAM) { \
                    iDam = (size_t)dam_con_map[i].didx[j]; \
                    for (m = 0; m < dam_month_size; m++) \
                        dam_var[i][iDam].FIELD[m] = month_buf[j * dam_month_size + m]; \
                } \
            } \
        } \
    } while (0)

    DAM_RESTORE_MONTH(state_metadata[N_STATE_VARS + STATE_DAM_OP_RELEASE].varname,
                      op_release);
    DAM_RESTORE_MONTH(state_metadata[N_STATE_VARS + STATE_DAM_OP_STORAGE].varname,
                      op_storage);
#undef DAM_RESTORE_MONTH

    if (mpi_rank == VIC_MPI_ROOT) {
        status = nc_close(ncid);
        check_nc_status(status, "Error closing state file %s",
                        filenames.init_state.nc_filename);
    }

    free(scalar_buf);
    free(hist_buf);
    free(month_buf);
}
