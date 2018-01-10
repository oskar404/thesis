/*
	Getopt headerfile
*/

extern int	opterr;		/* if error message should be printed */
extern int	optind;		/* index into parent argv vector */
extern int	optopt;			/* character checked for validity */
extern char	*optarg;		/* argument associated with option */

int getopt(int,char **,char *);


