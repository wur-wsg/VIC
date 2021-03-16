#include <vic_driver_image.h>
#include <plugin.h>

void
wofost_alloc(void)
{
    extern option_struct        options;
    extern domain_struct        local_domain;
    extern SimUnit           ***Grid;
    extern crop_con_map_struct *crop_con_map;

    SimUnit                    *cgrid;

    size_t                      i;
    size_t                      j;
    size_t                      k;

    Grid = malloc(local_domain.ncells_active * sizeof(*Grid));
    check_alloc_status(Grid, "Memory allocation error");

    for (i = 0; i < local_domain.ncells_active; i++) {
        Grid[i] = malloc(options.SNOW_BAND * sizeof(*Grid[i]));
        check_alloc_status(Grid[i], "Memory allocation error");

        for (j = 0; j < options.SNOW_BAND; j++) {
            Grid[i][j] = cgrid = NULL;

            for (k = 0; k < crop_con_map[i].nc_active; k++) {
                if (cgrid == NULL) {
                    Grid[i][j] = malloc(sizeof(*Grid[i][j]));
                    check_alloc_status(Grid[i][j], "Memory allocation error");
                    cgrid = Grid[i][j];
                }
                else {
                    cgrid->next = malloc(sizeof(*cgrid->next));
                    check_alloc_status(cgrid->next, "Memory allocation error");
                    cgrid = cgrid->next;
                }

                cgrid->crp = malloc(sizeof(*(cgrid->crp)));
                check_alloc_status(cgrid->crp, "Memory allocation error");
                cgrid->ste = malloc(sizeof(*(cgrid->ste)));
                check_alloc_status(cgrid->ste, "Memory allocation error");
                cgrid->mng = malloc(sizeof(*(cgrid->mng)));
                check_alloc_status(cgrid->mng, "Memory allocation error");
                cgrid->soil = malloc(sizeof(*(cgrid->soil)));
                check_alloc_status(cgrid->soil, "Memory allocation error");
                cgrid->met = malloc(sizeof(*(cgrid->met)));
                check_alloc_status(cgrid->met, "Memory allocation error");
                cgrid->next = NULL;
            }
        }
    }
}

void
wofost_finalize(void)
{
    extern option_struct options;
    extern domain_struct local_domain;
    extern SimUnit    ***Grid;

    size_t               i;
    size_t               j;

    for (i = 0; i < local_domain.ncells_active; i++) {
        for (j = 0; j < options.SNOW_BAND; j++) {
            Clean(Grid[i][j]);
        }
        free(Grid[i]);
    }
    free(Grid);
}
