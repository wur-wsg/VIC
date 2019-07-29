#include <vic_driver_shared_image.h>
#include <plugin.h>

/******************************************************************************
 * @brief    This routine determines the current forcing file data type and
 *           stores its location in the description of the current forcing file.
 *****************************************************************************/
void
plugin_set_force_type(char *cmdstr)
{
    extern plugin_global_param_struct plugin_global_param;
    extern plugin_filenames_struct plugin_filenames;

    char                    nctype[MAXSTRING];
    char                    ncfreq[MAXSTRING];
    char                    ncvarname[MAXSTRING];
    char                    ncfilename[MAXSTRING];
    int                     type = SKIP;
    int                     freq = FORCE_STEP;

    strcpy(nctype, MISSING_S);
    strcpy(ncfreq, MISSING_S);
    strcpy(ncvarname, MISSING_S);
    strcpy(ncfilename, MISSING_S);

    sscanf(cmdstr, "%*s %s %s %s %s", nctype, ncvarname, ncfreq, ncfilename);

    /***************************************
       Get meteorological data forcing info
    ***************************************/

    if (strcasecmp("DISCHARGE", nctype) == 0) {
        type = FORCING_DISCHARGE;
    }
    /** Undefined variable type **/
    else {
        log_err("Undefined forcing variable type %s in forcing file.",
                nctype);
    }

    if (strcasecmp("STEP", ncfreq) == 0) {
        freq = FORCE_STEP;
    }
    else if (strcasecmp("DAY", ncfreq) == 0) {
        freq = FORCE_DAY;
    }
    else if (strcasecmp("MONTH", ncfreq) == 0) {
        freq = FORCE_MONTH;
    }
    /** Undefined variable frequency **/
    else {
        log_err("Undefined forcing variable frequency %s in forcing file.",
                ncfreq);
    }
    plugin_global_param.forcefreq[type] = freq;

    if (strcasecmp(MISSING_S, ncvarname) != 0) {
        strcpy(plugin_filenames.f_varname[type], ncvarname);
    }
    else {
        log_err(
            "Must supply netCDF variable name for %s forcing file",
            nctype);
    }

    if (strcasecmp(MISSING_S, ncfilename) != 0) {
        strcpy(plugin_filenames.f_path_pfx[type], ncfilename);
    }
    else {
        log_err(
            "Must supply netCDF file name for %s forcing file",
            nctype);
    }
}
