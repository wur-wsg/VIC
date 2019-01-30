#include <vic_driver_image.h>
#include <plugin.h>

void
set_basins_downstream(size_t *downstream_basin){
    extern domain_struct    global_domain;
    extern plugin_filenames_struct plugin_filenames;

    int                    *id;
    int                    *downstream;
    
    bool                    found;

    size_t                  i;
    size_t                  j;

    size_t                  d2count[2];
    size_t                  d2start[2];

    downstream = malloc(global_domain.ncells_active * sizeof(*downstream));
    check_alloc_status(downstream, "Memory allocation error.");
    id = malloc(global_domain.ncells_active * sizeof(*id));
    check_alloc_status(id, "Memory allocation error.");

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    get_active_nc_field_int(&(plugin_filenames.routing), "downstream_id", 
                             d2start, d2count, id);
    
    get_active_nc_field_int(&(plugin_filenames.routing), "downstream", 
                             d2start, d2count, downstream);
    
    for (i = 0; i < global_domain.ncells_active; i++) {
        found = false;
        
        for (j = 0; j < global_domain.ncells_active; j++) {
            if (downstream[i] == id[j]) {
                downstream_basin[i] = j;
                found = true;
            }
        }
        
        if(!found){
            log_warn("No downstream cell was found; "
                    "Probably the ID was outside of the mask or "
                    "the ID was not set;"
                    "Setting cell as outflow point");
            downstream_basin[i] = i;
        }
    }

    free(downstream);
    free(id);
}

void
get_basins_routing(basin_struct *basins)
{
    extern domain_struct    global_domain;

    size_t                 *downstream;
    size_t                 *river;
    size_t                  Nriver;

    size_t                  cur_cell;
    size_t                  next_cell;

    size_t                  i;
    size_t                  j;

    downstream = malloc(global_domain.ncells_active * sizeof(*downstream));
    check_alloc_status(downstream, "Memory allocation error.");
    river = malloc(global_domain.ncells_active * sizeof(*river));
    check_alloc_status(river, "Memory allocation error.");

    basins->basin_map =
        malloc(global_domain.ncells_active * sizeof(*basins->basin_map));
    check_alloc_status(basins->basin_map, "Memory allocation error.");
    
    for (i = 0; i < global_domain.ncells_active; i++) {
        basins->basin_map[i] = MISSING_USI;
    }
    
    set_basins_downstream(downstream);

    basins->Nbasin = 0;
    Nriver = 0;
    for (i = 0; i < global_domain.ncells_active; i++) {
        Nriver = 0;
        cur_cell = next_cell = i;

        while (true) {
            
            river[Nriver] = cur_cell;
            Nriver++;

            if (basins->basin_map[cur_cell] != MISSING_USI) {
                for (j = 0; j < Nriver; j++) {
                    basins->basin_map[river[j]] = basins->basin_map[cur_cell];
                }
                break;
            }

            next_cell = downstream[cur_cell];

            if (next_cell == cur_cell) {
                for (j = 0; j < Nriver; j++) {
                    basins->basin_map[river[j]] = basins->Nbasin;
                }
                basins->Nbasin++;
                break;
            }

            cur_cell = next_cell;
        }
    }

    basins->Ncells = malloc(basins->Nbasin * sizeof(*basins->Ncells));
    check_alloc_status(basins->Ncells, "Memory allocation error.");
    basins->sorted_basins =
        malloc(basins->Nbasin * sizeof(*basins->sorted_basins));
    check_alloc_status(basins->sorted_basins, "Memory allocation error.");
    for (i = 0; i < basins->Nbasin; i++) {
        basins->sorted_basins[i] = i;
        basins->Ncells[i] = 0;
    }

    for (i = 0; i < global_domain.ncells_active; i++) {
        if (basins->basin_map[i] == MISSING_USI) {
            log_err("Found active cell not in basin");
        }
        basins->Ncells[basins->basin_map[i]]++;
    }

    // Sort basins by size
    size_t_sort(basins->sorted_basins, basins->Ncells, basins->Nbasin, false);

    basins->catchment = malloc(basins->Nbasin * sizeof(*basins->catchment));
    check_alloc_status(basins->catchment, "Memory allocation error.");
    for (i = 0; i < basins->Nbasin; i++) {
        basins->catchment[i] =
            malloc(basins->Ncells[i] * sizeof(basins->catchment[i]));
        check_alloc_status(basins->catchment[i], "Memory allocation error.");
    }

    for (i = 0; i < basins->Nbasin; i++) {
        for (j = 0; j < basins->Ncells[i]; j++) {
            basins->catchment[i][j] = MISSING_USI;
        }
        basins->Ncells[i] = 0;
    }

    for (i = 0; i < global_domain.ncells_active; i++) {
        basins->catchment[basins->basin_map[i]][basins->Ncells[basins->
                                                               basin_map[i]]] =
            i;
        basins->Ncells[basins->basin_map[i]]++;
    }
    
    free(downstream);
    free(river);
}

void
get_basins_file(basin_struct *basins)
{
    extern domain_struct    global_domain;
    extern plugin_filenames_struct plugin_filenames;

    int                    *basin_list;
    bool                    duplicate;

    size_t                  d2count[2];
    size_t                  d2start[2];

    size_t                  i;
    size_t                  j;

    d2start[0] = 0;
    d2start[1] = 0;
    d2count[0] = global_domain.n_ny;
    d2count[1] = global_domain.n_nx;

    basins->basin_map =
        malloc(global_domain.ncells_active * sizeof(*basins->basin_map));
    check_alloc_status(basins->basin_map, "Memory allocation error.");
    basin_list = malloc(global_domain.ncells_active * sizeof(*basin_list));
    check_alloc_status(basin_list, "Memory allocation error.");

    get_active_nc_field_int(&plugin_filenames.decomposition, "basin", d2start, d2count,
                            basins->basin_map);

    basins->Nbasin = 0;
    for (i = 0; i < global_domain.ncells_active; i++) {
        duplicate = false;

        for (j = 0; j < basins->Nbasin; j++) {
            if (basin_list[j] == basins->basin_map[i]) {
                duplicate = true;
                break;
            }
        }

        if (duplicate) {
            continue;
        }

        basin_list[basins->Nbasin] = basins->basin_map[i];
        basins->Nbasin++;
    }

    basins->Ncells = malloc(basins->Nbasin * sizeof(*basins->Ncells));
    check_alloc_status(basins->Ncells, "Memory allocation error.");
    basins->sorted_basins =
        malloc(basins->Nbasin * sizeof(*basins->sorted_basins));
    check_alloc_status(basins->sorted_basins, "Memory allocation error.");

    for (i = 0; i < basins->Nbasin; i++) {
        basins->sorted_basins[i] = i;
        basins->Ncells[i] = 0;
    }

    for (i = 0; i < global_domain.ncells_active; i++) {
        for (j = 0; j < basins->Nbasin; j++) {
            if (basin_list[j] == basins->basin_map[i]) {
                basins->Ncells[j]++;
            }
        }
    }

    // Sort basins by size
    size_t_sort(basins->sorted_basins, basins->Ncells, basins->Nbasin, false);

    basins->catchment = malloc(basins->Nbasin * sizeof(*basins->catchment));
    check_alloc_status(basins->catchment, "Memory allocation error.");
    for (i = 0; i < basins->Nbasin; i++) {
        basins->catchment[i] =
            malloc(basins->Ncells[i] * sizeof(basins->catchment[i]));
        check_alloc_status(basins->catchment[i], "Memory allocation error.");
    }

    for (i = 0; i < basins->Nbasin; i++) {
        basins->Ncells[i] = 0;
    }

    for (i = 0; i < global_domain.ncells_active; i++) {
        for (j = 0; j < basins->Nbasin; j++) {
            if (basin_list[j] == basins->basin_map[i]) {
                basins->catchment[j][basins->Ncells[j]] = i;
                basins->Ncells[j]++;
            }
        }
    }

    free(basin_list);
}
