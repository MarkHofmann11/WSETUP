/* Program to generate registration key codes for use with the BP?.LIB
 * registration key decoding algorithm. When compiled, this program should
 * be linked with the BP?.LIB file, by adding the name of this program and
 * the BP?.LIB file to a project / make file.
 */


#include "bp.h"

#include <stdio.h>
#include <string.h>

main()
   {
   char temp_string[80];
   unsigned int security_code;
   unsigned int security_verification;
   char registration_string[201];
   char registration_verification[201];

   printf("MAKEKEY 2.10 - (C) Copyright 1992, Brian Pirie. All Rights Reserved.\n\n");

   printf(" Please Enter Program's Unique Security Code : ");
   gets(temp_string);
   security_code=atoi(temp_string);
   printf("                      Again For Verification : ");
   gets(temp_string);
   security_verification=atoi(temp_string);
   if(security_code!=security_verification)
      {
      printf("\nCodes do not match!\n");
      return(1);
      }

   printf("           Registration String (User's Name) : ");
   gets(registration_string);
   printf("                      Again For Verification : ");
   gets(registration_verification);
   if(strcmp(registration_string,registration_verification)!=0)
      {
      printf("\nStrings do not match!\n");
      return(1);
      }

   printf("\n---------------------------------------------------------------------------\n");
   printf("GENERATED REGISTRATION KEY :\n\n");
   printf("            Registration String : [%s]\n",registration_string);
   printf("               Registration Key : [%lu]\n",bp(registration_string,security_code));
   printf("---------------------------------------------------------------------------\n");

   return(0);
   }
