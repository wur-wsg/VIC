#ifndef SUPPORT_H
#define SUPPORT_H

// plugin
bool plugin_force_get_global_param(char *);
void plugin_force_validate_global_param(void);
void plugin_force_init(void);
void plugin_get_forcing_file_info(short unsigned int);
void plugin_get_forcing_file_skip(short unsigned int);
void plugin_force_start(void);
void plugin_force_end(void);

// general
void cshift(double *, int, int, int, int);
void size_t_sort(size_t *, size_t *, size_t, bool);
void size_t_sort2(size_t *, int *, size_t, bool);
void double_flip(double *, size_t);
void size_t_swap(size_t, size_t, size_t *);
void int_swap(size_t, size_t, int *);
void double_swap(size_t, size_t, double *);

// mpi
void gather_double(double *, double *);
void gather_double_2d(double **, double **, int);
void gather_size_t(size_t *, size_t *);
void gather_size_t_2d(size_t **, size_t **, int);
void gather_int(int *, int *);
void gather_int_2d(int **, int **, int);
void scatter_double(double *, double *);
void scatter_double_2d(double **, double **, int);
void scatter_size_t(size_t *, size_t *);
void scatter_size_t_2d(size_t **, size_t **, int);
void scatter_int(int *, int *);
void scatter_int_2d(int **, int **, int);

// decomposition
int get_active_nc_field_double(nameid_struct *, char *, size_t *, size_t *,
                               double *);
int get_active_nc_field_int(nameid_struct *, char *, size_t *, size_t *, int *);

#endif /* SUPPORT_H */
