#ifndef CO2_H
#define CO2_H

typedef struct {
    double CO2;
} co2_force_struct;

co2_force_struct *co2_force;

void co2_alloc(void);
void co2_initialize_local_structures(void);
void co2_finalize(void);

void co2_update_step_vars(size_t);
void co2_forcing(void);

#endif  // CO2_H
