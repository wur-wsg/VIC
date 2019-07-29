#include <vic_driver_shared_image.h>
#include <plugin.h>

/******************************************************************************
 * @brief    This routine determines the current forcing file data type and
 *           stores its location in the description of the current forcing file.
 *****************************************************************************/
void
plugin_set_force_type(char *cmdstr)
{
    extern plugin_filenames_struct plugin_filenames;

    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];
    char                    ncvarname[MAXSTRING];
    char                    ncfilename[MAXSTRING];
    int                     type = SKIP;

    strcpy(ncvarname, "MISSING");
    strcpy(ncfilename, "MISSING");

    /** Initialize flgstr **/
    strcpy(flgstr, "NULL");

    sscanf(cmdstr, "%*s %s %s %s", optstr, ncvarname, ncfilename);

    /***************************************
       Get meteorological data forcing info
    ***************************************/

    if (strcasecmp("DISCHARGE", optstr) == 0) {
        type = FORCING_DISCHARGE;
    }
    /** Undefined variable type **/
    else {
        log_err("Undefined forcing variable type %s in forcing file.",
                optstr);
    }

    if (strcasecmp("MISSING", ncvarname) != 0) {
        strcpy(plugin_filenames.f_varname[type], ncvarname);
    }
    else {
        log_err(
            "Must supply netCDF variable name for %s forcing file",
            optstr);
    }

    if (strcasecmp("MISSING", ncfilename) != 0) {
        strcpy(plugin_filenames.f_path_pfx[type], ncfilename);
    }
    else {
        log_err(
            "Must supply netCDF file name for %s forcing file",
            optstr);
    }
}
