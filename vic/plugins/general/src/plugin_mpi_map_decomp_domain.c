/******************************************************************************
 * @section DESCRIPTION
 *
 * This routine decomposes the domain based on the routing basins
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#include <vic_driver_image.h>
#include <plugin.h>

/******************************************************************************
 * @brief    This routine decomposes the domain based on the routing basins
 *****************************************************************************/
void
plugin_mpi_map_decomp_domain(size_t   ncells,
                             size_t   mpi_size,
                             int    **mpi_map_local_array_sizes,
                             int    **mpi_map_global_array_offsets,
                             size_t **mpi_map_mapping_array)
{
    extern plugin_option_struct plugin_options;

    if (plugin_options.ROUTING &&
        plugin_options.DECOMPOSITION != RANDOM_DECOMPOSITION) {
        rout_mpi_map_decomp_domain(ncells, mpi_size, mpi_map_local_array_sizes,
                                   mpi_map_global_array_offsets,
                                   mpi_map_mapping_array);
    }
}
