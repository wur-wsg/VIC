#include <vic_driver_image.h>
#include <plugin.h>

/******************************************
 Setup broadcasting structures
******************************************/
void
plugin_create_MPI_global_struct_type(MPI_Datatype *mpi_type)
{
    extern MPI_Comm MPI_COMM_VIC;
    int             nitems; // number of elements in struct
    int             status;
    int            *blocklengths;
    size_t          i;
    MPI_Aint       *offsets;
    MPI_Datatype   *mpi_types;

    // nitems has to equal the number of elements in global_param_struct
    nitems = 2;
    blocklengths = malloc(nitems * sizeof(*blocklengths));
    check_alloc_status(blocklengths, "Memory allocation error.");
    offsets = malloc(nitems * sizeof(*offsets));
    check_alloc_status(offsets, "Memory allocation error.");
    mpi_types = malloc(nitems * sizeof(*mpi_types));
    check_alloc_status(mpi_types, "Memory allocation error.")

    // most of the elements in global_param_struct are not arrays. Use 1 as
    // the default block length and reset as needed
    for (i = 0; i < (size_t) nitems; i++) {
        blocklengths[i] = 1;
    }
    i = 0;

    // size_t rout_steps_per_day;
    offsets[i] = offsetof(plugin_global_param_struct, rout_steps_per_day);
    mpi_types[i++] = MPI_AINT;
    // double rout_dt;
    offsets[i] = offsetof(plugin_global_param_struct, rout_dt);
    mpi_types[i++] = MPI_DOUBLE;

    // make sure that the we have the right number of elements
    if (i != (size_t) nitems) {
        log_err("Miscount: %zd not equal to %d.", i, nitems);
    }
    status = MPI_Type_create_struct(nitems, blocklengths, offsets, mpi_types,
                                    mpi_type);
    check_mpi_status(status, "MPI error.");
    status = MPI_Type_commit(mpi_type);
    check_mpi_status(status, "MPI error.");

    // cleanup
    free(blocklengths);
    free(offsets);
    free(mpi_types);
}

void
plugin_create_MPI_filenames_struct_type(MPI_Datatype *mpi_type)
{
    extern MPI_Comm MPI_COMM_VIC;
    int             nitems; // number of elements in struct
    int             status;
    int            *blocklengths;
    size_t          i;
    MPI_Aint       *offsets;
    MPI_Datatype   *mpi_types;

    // nitems has to equal the number of elements in global_param_struct
    nitems = 1;
    blocklengths = malloc(nitems * sizeof(*blocklengths));
    check_alloc_status(blocklengths, "Memory allocation error.");
    offsets = malloc(nitems * sizeof(*offsets));
    check_alloc_status(offsets, "Memory allocation error.");
    mpi_types = malloc(nitems * sizeof(*mpi_types));
    check_alloc_status(mpi_types, "Memory allocation error.")

    // most of the elements in global_param_struct are not arrays. Use 1 as
    // the default block length and reset as needed
    for (i = 0; i < (size_t) nitems; i++) {
        blocklengths[i] = MAXSTRING;
    }
    i = 0;

    // char rf_path_pfx[MAXSTRING];
    offsets[i] = offsetof(plugin_filenames_struct, rf_path_pfx);
    mpi_types[i++] = MPI_CHAR;

    // make sure that the we have the right number of elements
    if (i != (size_t) nitems) {
        log_err("Miscount: %zd not equal to %d.", i, nitems);
    }
    status = MPI_Type_create_struct(nitems, blocklengths, offsets, mpi_types,
                                    mpi_type);
    check_mpi_status(status, "MPI error.");
    status = MPI_Type_commit(mpi_type);
    check_mpi_status(status, "MPI error.");

    // cleanup
    free(blocklengths);
    free(offsets);
    free(mpi_types);
}

void
plugin_create_MPI_option_struct_type(MPI_Datatype *mpi_type)
{
    extern MPI_Comm MPI_COMM_VIC;
    int             nitems; // number of elements in struct
    int             status;
    int            *blocklengths;
    size_t          i;
    MPI_Aint       *offsets;
    MPI_Datatype   *mpi_types;

    // nitems has to equal the number of elements in global_param_struct
    nitems = 4;
    blocklengths = malloc(nitems * sizeof(*blocklengths));
    check_alloc_status(blocklengths, "Memory allocation error.");
    offsets = malloc(nitems * sizeof(*offsets));
    check_alloc_status(offsets, "Memory allocation error.");
    mpi_types = malloc(nitems * sizeof(*mpi_types));
    check_alloc_status(mpi_types, "Memory allocation error.")

    // most of the elements in global_param_struct are not arrays. Use 1 as
    // the default block length and reset as needed
    for (i = 0; i < (size_t) nitems; i++) {
        blocklengths[i] = 1;
    }
    i = 0;

    // short unsigned int DECOMPOSITION;
    offsets[i] = offsetof(plugin_option_struct, DECOMPOSITION);
    mpi_types[i++] = MPI_INT;
    // bool ROUTING;
    offsets[i] = offsetof(plugin_option_struct, ROUTING);
    mpi_types[i++] = MPI_C_BOOL;
    // short unsigned int UH_LENGTH;
    offsets[i] = offsetof(plugin_option_struct, UH_LENGTH);
    mpi_types[i++] = MPI_UNSIGNED_SHORT;
    // bool FORCE_ROUTING;
    offsets[i] = offsetof(plugin_option_struct, FORCE_ROUTING);
    mpi_types[i++] = MPI_C_BOOL;

    // make sure that the we have the right number of elements
    if (i != (size_t) nitems) {
        log_err("Miscount: %zd not equal to %d.", i, nitems);
    }
    status = MPI_Type_create_struct(nitems, blocklengths, offsets, mpi_types,
                                    mpi_type);
    check_mpi_status(status, "MPI error.");
    status = MPI_Type_commit(mpi_type);
    check_mpi_status(status, "MPI error.");

    // cleanup
    free(blocklengths);
    free(offsets);
    free(mpi_types);
}

void
plugin_create_MPI_param_struct_type(MPI_Datatype *mpi_type)
{
    extern MPI_Comm MPI_COMM_VIC;
    int             nitems; // number of elements in struct
    int             status;
    int            *blocklengths;
    size_t          i;
    MPI_Aint       *offsets;
    MPI_Datatype   *mpi_types;

    // nitems has to equal the number of elements in global_param_struct
    nitems = 0;
    blocklengths = malloc(nitems * sizeof(*blocklengths));
    check_alloc_status(blocklengths, "Memory allocation error.");
    offsets = malloc(nitems * sizeof(*offsets));
    check_alloc_status(offsets, "Memory allocation error.");
    mpi_types = malloc(nitems * sizeof(*mpi_types));
    check_alloc_status(mpi_types, "Memory allocation error.")

    // most of the elements in global_param_struct are not arrays. Use 1 as
    // the default block length and reset as needed
    for (i = 0; i < (size_t) nitems; i++) {
        blocklengths[i] = 1;
    }
    i = 0;

    // make sure that the we have the right number of elements
    if (i != (size_t) nitems) {
        log_err("Miscount: %zd not equal to %d.", i, nitems);
    }
    status = MPI_Type_create_struct(nitems, blocklengths, offsets, mpi_types,
                                    mpi_type);
    check_mpi_status(status, "MPI error.");
    status = MPI_Type_commit(mpi_type);
    check_mpi_status(status, "MPI error.");

    // cleanup
    free(blocklengths);
    free(offsets);
    free(mpi_types);
}

void
plugin_initialize_mpi(void)
{
    extern MPI_Datatype plugin_mpi_global_struct_type;
    extern MPI_Datatype plugin_mpi_filenames_struct_type;
    extern MPI_Datatype plugin_mpi_option_struct_type;
    extern MPI_Datatype plugin_mpi_param_struct_type;
    
    // initialize MPI data structures
    plugin_create_MPI_global_struct_type(&plugin_mpi_global_struct_type);
    plugin_create_MPI_filenames_struct_type(&plugin_mpi_filenames_struct_type);
    plugin_create_MPI_option_struct_type(&plugin_mpi_option_struct_type);
    plugin_create_MPI_param_struct_type(&plugin_mpi_param_struct_type);
}

/******************************************
 Broadcast global structures
******************************************/
void
plugin_broadcast_filenames(void)
{
    extern plugin_filenames_struct plugin_filenames;
    extern MPI_Datatype plugin_mpi_filenames_struct_type;
    extern MPI_Comm            MPI_COMM_VIC;
    
    int status;
    
    status = MPI_Bcast(&plugin_filenames, 1, plugin_mpi_filenames_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
}

void
plugin_broadcast_global_params(void)
{
    extern plugin_global_param_struct plugin_global_param;
    extern MPI_Datatype plugin_mpi_global_struct_type;
    extern MPI_Comm            MPI_COMM_VIC;
    
    int status;
    
    status = MPI_Bcast(&plugin_global_param, 1, plugin_mpi_global_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
}

void
plugin_broadcast_options(void)
{
    extern plugin_option_struct plugin_options;
    extern MPI_Datatype plugin_mpi_option_struct_type;
    extern MPI_Comm            MPI_COMM_VIC;
    
    int status;
    
    status = MPI_Bcast(&plugin_options, 1, plugin_mpi_option_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
}

void
plugin_broadcast_params(void)
{
    extern plugin_parameters_struct plugin_param;
    extern MPI_Datatype plugin_mpi_param_struct_type;
    extern MPI_Comm            MPI_COMM_VIC;
    
    int status;
    
    status = MPI_Bcast(&plugin_param, 1, plugin_mpi_param_struct_type,
                       VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
}
