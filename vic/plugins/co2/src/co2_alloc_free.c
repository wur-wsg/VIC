#include <vic_driver_image.h>
#include <plugin.h>

void
co2_alloc(void)
{
    extern domain_struct local_domain;
    extern co2_force_struct    *co2_force;

    co2_force = malloc(local_domain.ncells_active * sizeof(*co2_force));
    check_alloc_status(co2_force, "Memory allocation error");
    
    co2_initialize_local_structures();
}

void
co2_finalize(void)
{
    extern co2_force_struct   *co2_force;
    
    free(co2_force);
}
