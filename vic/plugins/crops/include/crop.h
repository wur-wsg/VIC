#ifndef CROP_H
#define CROP_H

#include <wofost.h>

typedef struct {
    size_t nc_types;
    size_t nc_active;
    
    int *veg_class;
    int *cidx;
    double **Cc;
} crop_con_map_struct;

typedef struct {
    double *DVS_point;
    double *N_amount;
    double *P_amount;
    double *K_amount;
} crop_con_struct;

typedef struct {
    double CO2;
    double **DVS_point;
    double **N_amount;
    double **P_amount;
    double **K_amount;
} crop_force_struct;

crop_con_map_struct *crop_con_map;
crop_con_struct **crop_con;
crop_force_struct *crop_force;

bool crop_get_global_param(char *);
void crop_validate_global_param(void);
bool crop_get_parameters(char *);
void crop_validate_parameters(void);
void crop_start(void);
void crop_alloc(void);
void crop_initialize_local_structures(void);
void wofost_alloc(void);
void wofost_initialize_local_structures(void);
void initialize_wofost_grid(SimUnit *);
void copy_wofost_grid(SimUnit *, SimUnit *);
void crop_init(void);
void wofost_set_data_text(char *);
void wofost_set_data(void);
void wofost_check_data(void);
void crop_finalize(void);
void wofost_finalize(void);

void crop_set_output_met_data_info(void);
void crop_initialize_nc_file(nc_file_struct *);
void crop_add_hist_dim(nc_file_struct *, stream_struct *);
void crop_set_nc_var_info(unsigned int, nc_file_struct *, nc_var_struct *);
void crop_set_nc_var_dimids(unsigned int, nc_file_struct *, nc_var_struct *);
void crop_history(int varid, unsigned int *);

void crop_update_step_vars(size_t);
void wofost_update_step_vars(size_t);
void crop_forcing(void);
bool crop_run_flag(void);
bool crop_reset_flag(void);
void crop_run(size_t);
void wofost_run(SimUnit *);
void crop_put_data(size_t);

#endif	// CROP_H

