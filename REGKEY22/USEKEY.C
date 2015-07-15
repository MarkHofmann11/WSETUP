/* Sample program which uses the registration key system. This program's
 * security code is 24805 - I would suggest that you change this value
 * use in any programs you have written. When compiled, this program must
 * be linked with the BP?.LIB file. For more information on compiling
 * programs using the registration key system, please see the accompanying
 * documentation, in the file REGKEY.DOC. This program reads the
 * registration information from the file REGISTER.KEY. The program expects
 * to find the registered user's name on the first line of this file, and
 * the user's registration key on the second line of this file.
 */


#include "bp.h"           /* Include the registration key system header file */

#include <stdio.h>                                   /* Other C header files */
#include <string.h>

char is_reged=0;                                /* 1 if registered, 0 if not */
char registered_name[201];                        /* Name of registered user */

main()                                              /* Main program function */
   {
   FILE *fp;                           /* File pointer for REGISTER.KEY file */
   unsigned long supplied_key;                       /* Key supplied by user */
   unsigned long correct_key;                    /* Correct registration key */

   if((fp=fopen("REGISTER.KEY","r"))!=NULL)              /* Try to open file */
      {                                                     /* If successful */
      fgets(registered_name,200,fp);                  /* read name from file */
      if(registered_name[strlen(registered_name)-1]=='\n')
         registered_name[strlen(registered_name)-1]='\0';

      fscanf(fp,"%lu",&supplied_key);                  /* read key from file */

      fclose(fp);                                              /* Close file */

      correct_key=bp(registered_name,24805);        /* Calculate correct key */

      if(correct_key==supplied_key)       /* Compare correct & supplied keys */
         {                                          /* If they are identical */
         is_reged=1;             /* Then switch program into registered mode */
         }
      }

   if(is_reged==1)                                     /* If registered mode */
      {                                  /* Display registration information */
      printf("This program is registered to: %s\n",registered_name);
      }
   else if(is_reged==0)                         /* If not in registered mode */
      {                                  /* Display unregistered information */
      printf("This program is UNREGISTERED!!!\n");
      }
   }


