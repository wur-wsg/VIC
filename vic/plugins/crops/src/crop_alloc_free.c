#include <vic_driver_image.h>
#include <plugin.h>

void
crop_alloc(void)
{
    extern domain_struct              local_domain;
    extern plugin_option_struct       plugin_options;
    extern crop_con_map_struct       *crop_con_map;
    extern crop_con_struct       **crop_con;
    extern crop_force_struct    *crop_force;

    size_t                            i;
    size_t                            j;

    crop_con = malloc(local_domain.ncells_active * sizeof(*crop_con));
    check_alloc_status(crop_con, "Memory allocation error")

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
        
        crop_con[i] = malloc(crop_con_map[i].nc_active * sizeof(*crop_con[i]));
        check_alloc_status(crop_con[i], "Memory allocation error");
        
        crop_force[i].DVS_point = malloc(crop_con_map[i].nc_active * sizeof(*crop_force[i].DVS_point));
        check_alloc_status(crop_force[i].DVS_point, "Memory allocation error");
        
        crop_force[i].N_amount = malloc(crop_con_map[i].nc_active * sizeof(*crop_force[i].N_amount));
        check_alloc_status(crop_force[i].N_amount, "Memory allocation error");
        
        crop_force[i].P_amount = malloc(crop_con_map[i].nc_active * sizeof(*crop_force[i].P_amount));
        check_alloc_status(crop_force[i].P_amount, "Memory allocation error");
        
        crop_force[i].K_amount = malloc(crop_con_map[i].nc_active * sizeof(*crop_force[i].K_amount));
        check_alloc_status(crop_force[i].K_amount, "Memory allocation error");
        
        for (j = 0; j < crop_con_map[i].nc_active; j++){
            crop_con[i][j].DVS_point = malloc(plugin_options.NFERTTIMES * sizeof(*crop_con[i][j].DVS_point));
            check_alloc_status(crop_con[i][j].DVS_point, "Memory allocation error");
            
            crop_con[i][j].N_amount = malloc(plugin_options.NFERTTIMES * sizeof(*crop_con[i][j].N_amount));
            check_alloc_status(crop_con[i][j].N_amount, "Memory allocation error");
            
            crop_con[i][j].P_amount = malloc(plugin_options.NFERTTIMES * sizeof(*crop_con[i][j].P_amount));
            check_alloc_status(crop_con[i][j].P_amount, "Memory allocation error");
            
            crop_con[i][j].K_amount = malloc(plugin_options.NFERTTIMES * sizeof(*crop_con[i][j].K_amount));
            check_alloc_status(crop_con[i][j].K_amount, "Memory allocation error");
        
            crop_force[i].DVS_point[j] = malloc(plugin_options.NFERTTIMES * sizeof(*crop_force[i].DVS_point[j]));
            check_alloc_status(crop_force[i].DVS_point[j], "Memory allocation error");

            crop_force[i].N_amount[j] = malloc(plugin_options.NFERTTIMES * sizeof(*crop_force[i].N_amount[j]));
            check_alloc_status(crop_force[i].N_amount[j], "Memory allocation error");

            crop_force[i].P_amount[j] = malloc(plugin_options.NFERTTIMES * sizeof(*crop_force[i].P_amount[j]));
            check_alloc_status(crop_force[i].P_amount[j], "Memory allocation error");

            crop_force[i].K_amount[j] = malloc(plugin_options.NFERTTIMES * sizeof(*crop_force[i].K_amount[j]));
            check_alloc_status(crop_force[i].K_amount[j], "Memory allocation error");
        }
    }
    
    wofost_alloc();

    crop_initialize_local_structures();
    wofost_initialize_local_structures();
}

void
crop_finalize(void)
{
    extern domain_struct        local_domain;
    extern crop_con_map_struct       *crop_con_map;
    extern crop_con_struct       **crop_con;
    extern crop_force_struct    *crop_force;

    size_t                      i;
    size_t                      j;

    wofost_finalize();
    
    for (i = 0; i < local_domain.ncells_active; i++) {
        for(j = 0; j < crop_con_map[i].nc_types; j++){
            free(crop_con_map[i].Cc[j]);
        }
        for(j = 0; j < crop_con_map[i].nc_active; j++){
            free(crop_con[i][j].DVS_point);
            free(crop_con[i][j].N_amount);
            free(crop_con[i][j].P_amount);
            free(crop_con[i][j].K_amount);
            free(crop_force[i].DVS_point[j]);
            free(crop_force[i].N_amount[j]);
            free(crop_force[i].P_amount[j]);
            free(crop_force[i].K_amount[j]);
        }

        free(crop_con_map[i].cidx);
        free(crop_con_map[i].veg_class);
        free(crop_con_map[i].Cc);
        free(crop_con[i]);
        free(crop_force[i].DVS_point);
        free(crop_force[i].N_amount);
        free(crop_force[i].P_amount);
        free(crop_force[i].K_amount);
    }
    free(crop_con_map);
    free(crop_con);
    free(crop_force);
}
