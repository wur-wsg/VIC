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
    double CO2;
} crop_force_struct;

crop_con_map_struct *crop_con_map;
crop_force_struct *crop_force;

bool crop_get_global_param(char *);
void crop_validate_global_param(void);
bool crop_get_parameters(char *);
void crop_validate_parameters(void);
void crop_start(void);
void crop_late_alloc(void);
void wofost_late_alloc(void);
void crop_initialize_local_structures(void);
void initialize_wofost_grid(SimUnit *);
void crop_init(void);
void wofost_set_data_text(char *);
void crop_finalize(void);
void wofost_finalize(void);

void crop_update_step_vars(size_t);
void crop_forcing(void);
void crop_run(size_t);
void wofost_run(SimUnit *);

#endif	// CROP_H

