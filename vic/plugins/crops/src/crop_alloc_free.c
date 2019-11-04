#include <vic_driver_image.h>
#include <plugin.h>

void
crop_alloc(void)
{
    extern domain_struct              local_domain;
    extern plugin_option_struct       plugin_options;
    extern crop_con_map_struct       *crop_con_map;
    extern crop_force_struct    *crop_force;

    size_t                            i;
    size_t                            j;

    crop_con_map = malloc(local_domain.ncells_active * sizeof(*crop_con_map));
    check_alloc_status(crop_con_map, "Memory allocation error");

    crop_force = malloc(local_domain.ncells_active * sizeof(*crop_force));
    check_alloc_status(crop_force, "Memory allocation error");
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        crop_con_map[i].nc_types = plugin_options.NCROPTYPES;
    
        crop_con_map[i].cidx = malloc(crop_con_map[i].nc_types * sizeof(*crop_con_map[i].cidx));
        check_alloc_status(crop_con_map[i].cidx, "Memory allocation error");

        crop_con_map[i].veg_class = malloc(crop_con_map[i].nc_types * sizeof(*crop_con_map[i].veg_class));
        check_alloc_status(crop_con_map[i].veg_class, "Memory allocation error");

        crop_con_map[i].Cc = malloc(crop_con_map[i].nc_types * sizeof(*crop_con_map[i].Cc));
        check_alloc_status(crop_con_map[i].Cc, "Memory allocation error");
            
        for(j = 0; j < crop_con_map[i].nc_types; j++){
            crop_con_map[i].Cc[j] = malloc(MONTHS_PER_YEAR * sizeof(*crop_con_map[i].Cc[j]));
            check_alloc_status(crop_con_map[i].Cc[j], "Memory allocation error");
        }

        crop_con_map[i].nc_active = (size_t) local_domain.locations[i].ncrop;
    }
    
    wofost_alloc();

    wofost_initialize_local_structures();
}

void
crop_finalize(void)
{
    extern domain_struct        local_domain;
    extern crop_con_map_struct       *crop_con_map;

    size_t                      i;
    size_t                      j;

    wofost_finalize();
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < crop_con_map[i].nc_types; j++){
            free(crop_con_map[i].Cc[j]);
        }

        free(crop_con_map[i].cidx);
        free(crop_con_map[i].veg_class);
        free(crop_con_map[i].Cc);
    }
    free(crop_con_map);
    free(crop_force);
}
