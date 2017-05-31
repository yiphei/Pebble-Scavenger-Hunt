/*
 * words.h - header file for words.c
 * 
 * function prototypes for those in words.c that modify strings
 *
 * Michael Perezous, CS50, May 2017
 */


#ifdef NOPEBBLE
/* takes any string as input and "normalizes" it so that it is
   lower case and can be used uniformally and there are no repeats
   in the data structure storing it */
void NormalizeWord(char *word);

#endif
