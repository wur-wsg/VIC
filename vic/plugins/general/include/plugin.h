#ifndef PLUGIN_GENERAL_H
#define PLUGIN_GENERAL_H

#include <vic_driver_shared_image.h>

enum {
    STATE_DISCHARGE_DT,
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_STATE_VARS                   /**< used as a loop counter*/
};
   
enum {
    OUT_STREAM_MOIST,
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    PLUGIN_N_OUTVAR_TYPES                /**< used as a loop counter*/
};

typedef struct {
    // simulation options
    bool MATRIC;
    bool ROUTING;
    int DECOMPOSITION;
    
    // module options
    short unsigned int UH_LENGTH;
    bool FORCE_ROUTING;
    
} plugin_option_struct;

plugin_option_struct plugin_options;

// Setup
bool plugin_get_global_param(char optstr[MAXSTRING]);
void plugin_validate_global_param(void);
bool plugin_get_parameters(char optstr[MAXSTRING]);
void plugin_validate_parameters(void);
void plugin_compare_ncdomain_with_global_domain(void);
void plugin_get_forcing_file_info(void);
void plugin_start(void);
void plugin_alloc(void);
void plugin_init(void);
void plugin_generate_default_state(void);
void plugin_compute_derived_state_vars(void);
void plugin_check_init_state_file(void);
void plugin_restore(void);
void plugin_print_global_param(global_param_struct *gp);
void plugin_print_options(option_struct *option);
void plugin_print_parameters(parameters_struct *param);
void plugin_finalize(void);

// Output
void plugin_set_output_met_data_info(void);
void plugin_initialize_nc_file(nc_file_struct  *nc_file);
void plugin_add_history_dimensions(nc_file_struct *nc,
                              stream_struct  *stream);
void plugin_set_nc_var_info(unsigned int       varid,
                       unsigned short int dtype,
                       nc_file_struct    *nc_hist_file,
                       nc_var_struct     *nc_var);
void plugin_set_nc_var_dimids(unsigned int    varid,
                         nc_file_struct *nc_hist_file,
                         nc_var_struct  *nc_var);
void plugin_get_default_outvar_aggtype(unsigned int varid);
void plugin_set_state_meta_data_info(void);
void plugin_set_nc_state_file_info(nc_file_struct  *nc_state_file);
void plugin_initialize_state_file(char           *filename,
                                     nc_file_struct *nc_state_file);
void plugin_set_nc_state_var_info(nc_file_struct *nc);

// Simulation
void plugin_force(void);
void plugin_run(void);
void plugin_put_data(void);

#endif /* PLUGIN_GENERAL_H */

