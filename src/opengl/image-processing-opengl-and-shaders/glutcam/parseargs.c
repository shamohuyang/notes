/* ************************************************************************* 
* NAME: parseargs.c
*
* DESCRIPTION:
*
* parse the command line args into a structure for the rest of the program.
* code here expects to parse:
*
*      [-d devicefile] [-w width] [-h height]
*      [-e  LUMA |  YUV420 |  YUV422 | RGB ]
* all args are optional:
*
* if devicefile is not supplied, the source is assumed to be testpattern
* if width or height are not supplied, 320x240 are assumed
* if encoding not supplied, LUMA assumed
*
* PROCESS:
*
* GLOBALS: optarg
*
* REFERENCES:
*
* LIMITATIONS:
*
* REVISION HISTORY:
*
*   STR                Description                          Author
*
*   31-Dec-06          initial coding                        gpk
*
* TARGET: unix C
*
* This software is in the public domain. if it breaks you get to keep
* both pieces.
*
* ************************************************************************* */

#include <stdio.h>
#include <unistd.h> /* getopt  */
#include <string.h> /* memset, strncpy */
#include <stdlib.h> /* atoi  */

#include "glutcam.h"
#include "parseargs.h"


extern char *optarg; /* declared in the C library for getopt  */





/* ************************************************************************* 


   NAME:  parse_command_line


   USAGE: 

   int some_int;
   int argc;
   char * argv;
   Cmdargs_t argstruct;

   some_int =  parse_command_line(argc, argv, &argstruct);

   if (0 == some_int)
   -- success
   else
   -- handle an error
   
   returns: int

   DESCRIPTION:

     take argc, argv[] and parse the results into the argstruct

     expecting to get some of:

     [-d devicefile] [-w width] [-h height]
     [-e  LUMA |  YUV420 |  YUV422 | RGB ]
     
     return 0 on success, -1 on error

   REFERENCES:

   LIMITATIONS:

   glutInit (called later) will also accept:
             -display
	     -geometry
	     -iconic
	     -indirect
	     -direct
	     -gldebug
	     -sync

   i'm not sure why, but the switch statement in this function
   generates a bunch of warnings about lines that will never
   be executed. i don't know why...
   
   GLOBAL VARIABLES:

      accessed: optarg

      modified: none

   FUNCTIONS CALLED:

   REVISION HISTORY:

        STR                  Description of Revision                 Author

     31-Dec-06               initial coding                           gpk
     20-Jan-08  remove '-o' option since it's been replaced by a menu gpk
                option.
		
 ************************************************************************* */

int parse_command_line (int argc, char * argv[], Cmdargs_t *args)
{
  int opt, unexpected, retval;

  args->source = TESTPATTERN;
  /*   args->output = GREYSCALE; */
  args->devicename[0] = 0;
  args->image_width = 320;
  args->image_height = 240;
  args->encoding = LUMA;

  unexpected = 0;
  retval = 0;
  
  opt = getopt(argc, argv, "d:o:w:h:e:");

  while ((-1 != opt) && (0 == unexpected))
    {
      switch (opt) {
      case 'd':
	strncpy(args->devicename, optarg, MAX_DEVICENAME);
	args->source = LIVESOURCE;
	break;

      case 'w':
	args->image_width = atoi(optarg);
	break;
	
      case 'h':
	args->image_height = atoi(optarg);
	break;
	
      case 'e':
	/* i don't know why, but the compiler warns that the  */
	/* following "if" statement won't be executed. since  */
	/* it works, i'll say the warning can be ignored.     */
	if (0 == strcmp("LUMA", optarg))
	  {
	    args->encoding = LUMA;
	  }
	else if (0 == strcmp("YUV420", optarg))
	  {
	    args->encoding = YUV420;
	  }
	else if (0 == strcmp("YUV422", optarg))
	  {
	    args->encoding = YUV422;
	  }
#if 0
	else if (0 == strcmp("RGB_BAYER", optarg))
	  { 
	    args->encoding = RGB_BAYER;

	  }
#endif /* 0  */
	else if (0 == strcmp("RGB", optarg))
	  {
	    args->encoding = RGB;
	  }
	else
	  {
	    fprintf(stderr, "image encoding (-e) option '%s' not recognized\n",
		    optarg);
	    fprintf(stderr,
		    "must be LUMA, YUV420, YUV422 or RGB\n");
	    unexpected = 1;
	  }
	break;
      default:
	fprintf(stderr, "Error parsing command line argument -%c\n",
		opt);
	fprintf(stderr, "Usage: %s %s %s\n", argv[0],
		"[-d devicefile][-w width][-h height]",
		"[-e  LUMA |  YUV420 |  YUV422 | RGB ]"
		);
	unexpected = 1;
	retval = -1;
	break;
      }
      opt = getopt(argc, argv, "d:o:w:h:e:");
    }

  if (1 == unexpected)
    {
      retval = -1;
    }
  return(retval);
}
