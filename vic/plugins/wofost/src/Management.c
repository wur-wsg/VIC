#include <vic_driver_shared_image.h>
#include "plugin.h"

void
GetManagement(Management *MNG,
              char       *management)
{
    extern char *ManageParam[];
    extern char *ManageParam2[];

    TABLE_D     *Table[NR_TABLES_MANAGEMENT], *start;

    char         line[MAXSTRING];
    int          i, c, count;
    float        Variable[100], YValue;
    char         x[2], word[100];
    char         dateString[7];
    FILE        *fq;

    if ((fq = fopen(management, "r")) == NULL) {
        log_err("Cannot open input file %s", management);
    }

    i = 0;
    count = 0;
    while (strcmp(ManageParam[i], "NULL")) {
        while ((c = fscanf(fq, "%s", word)) != EOF) {
            if (strlen(word) > 98) {
                log_err(
                    "Check the management input file: very long strings (%zu)", strlen(
                        word));
            }
            if (!strcmp(word, ManageParam[i])) {
                while ((c = fgetc(fq)) != '=') {
                    ;
                }
                fscanf(fq, "%f", &Variable[i]);
                count++;
                break;
            }
        }
        rewind(fq);
        i++;
    }

    if (count != NR_VARIABLES_MANAGEMENT) {
        log_err("Something wrong with the Site variables");
    }

    rewind(fq);

    FillManageVariables(MNG, Variable);


    i = 0;
    count = 0;
    while (strcmp(ManageParam2[i], "NULL")) {
        while (fgets(line, MAXSTRING, fq)) {
            if (line[0] == '*' || line[0] == ' ' || line[0] == '\n') {
                continue;
            }

            sscanf(line, "%s", word);
            if (!strcmp(word, ManageParam2[i])) {
                memset(dateString, '\0', 7);
                sscanf(line, "%s %s %s %f", word, x, dateString, &YValue);
                if (strlen(dateString) >= 6) {
                    exit(0);
                }

                Table[i] = start = malloc(sizeof(TABLE_D));
                Table[i]->next = NULL;
                sscanf(dateString, "%d-%d", &Table[i]->month, &Table[i]->day);
                Table[i]->amount = YValue;

                while (fgets(line, MAXSTRING, fq)) {
                    memset(dateString, '\0', 7);
                    if (sscanf(line, "%s  %f", dateString, &YValue) != 2) {
                        break;
                    }
                    if (strlen(dateString) >= 6) {
                        exit(0);
                    }

                    Table[i]->next = malloc(sizeof(TABLE_D));
                    Table[i] = Table[i]->next;
                    Table[i]->next = NULL;
                    sscanf(dateString, "%d-%d", &Table[i]->month,
                           &Table[i]->day);
                    Table[i]->amount = YValue;
                }
                /* Go back to beginning of the table */
                Table[i] = start;
                count++;
                break;
            }
        }
        rewind(fq);
        i++;
    }

    fclose(fq);

    if (i != NR_TABLES_MANAGEMENT) {
        log_err("Something wrong with the Management tables");
    }

    MNG->N_Fert_table = Table[0];
    MNG->P_Fert_table = Table[1];
    MNG->K_Fert_table = Table[2];
    MNG->Irrigation = Table[3];
}
