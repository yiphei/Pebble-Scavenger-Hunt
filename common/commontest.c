/* 
 * commontest.c - test program for krag and team module
 *
 * usage:
 *   read stock quotes from stdin to use as test data.
 * each line should be comma-separated values, with 
 *  "symbol",close,price,volume
 * where close and price are floats and volume is an int.
 * for example, pipe the output of curl into this program:
 *  curl 'http://download.finance.yahoo.com/d/quotes.csv?s=AAPL,GOOG,MSFT,FB&f=spl1v&e=.csv' | ./bagtest
 * Old documentation about that API:
 *  http://www.financialwisdomforum.org/gummy-stuff/Yahoo-data.htm
 *  http://www.marketindex.com.au/yahoo-finance-api
 *
 * Yifei Yan
 * Some parts of the code are from CS50 bagtest.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "krag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/set.h"
#include "../libcs50/file.h"
#include "team.h"
#include <arpa/inet.h>


static void 
kragprint(FILE *fp, const char *key, void *item);

static void 
clueprint(FILE *fp, const char *key, void *item);

int main(const int argc, const char *argv[]) {

	char * kragfile;
	kragfile = malloc(strlen(argv[1])+1);       
	strcpy(kragfile,argv[1]);

	char * secretfile;
	secretfile = malloc(strlen(argv[2])+1);       
	strcpy(secretfile,argv[2]);


	hashtable_t * teamhash = initHash();

	struct sockaddr addr;
	addr.sa_family = AF_INET;
	connection_t*  conn = newConnection(3, addr);

	 addGuideAgent("1", "team one", "GA1", "22", conn, teamhash);
	addFieldAgent("FA1", "12345678", "team one", "22", conn, teamhash);
	addFieldAgent("FA2", "12312435", "team one", "22", conn, teamhash);
	addFieldAgent("FA3", "12345636", "team one", "22", conn, teamhash);
	int x = addFieldAgent("FA3", "12345678", "team one", "22", conn, teamhash);
	int y = addGuideAgent("2", "team one", "GA2", "22", conn, teamhash);

	//testing for duplicate field angent and guide agent
	if (x == 1 && y == 1){
		printf("FA3 already exists\n");
	}

	addGuideAgent("2", "team two", "GA1", "22", conn, teamhash);
	addFieldAgent("FA1", "12345678", "team two", "22", conn, teamhash);
	addFieldAgent("FA2", "12345678", "team two", "22", conn, teamhash);
	addFieldAgent("FA3", "12345678", "team two", "22", conn, teamhash);

	//read the krag file and store in a krag hashtablee
    hashtable_t * kraghash = readKrag(kragfile);
	printKrags(kraghash);

	//get the secret string from the file
	 char * secret = getSecretString(secretfile);
	 printf("secret is: %s\n", secret);

	 //reveal characters
	 revealCharacters("86DA","team one", secret, teamhash, kraghash);
	 char * string = getRevealedString("team one", teamhash);
	 printf("curr string: %s\n", string);

	 revealCharacters("86DA","team one", secret, teamhash, kraghash);
	 string = getRevealedString("team one", teamhash);
	 printf("curr string: %s\n", string);

	revealCharacters("87DA","team one",secret, teamhash, kraghash);
	revealCharacters("99DA","team one", secret, teamhash, kraghash);
	revealCharacters("89DA","team one", secret, teamhash, kraghash);
	string = getRevealedString("team one", teamhash);
	 printf("curr string: %s\n", string);

	 //add krag to the team one
	addKrag("team one", "86DA", kraghash,teamhash);
	addKrag("team one", "89DA", kraghash,teamhash);

	team_t * team = hashtable_find(teamhash, "team one");

	printf("num claimed is %d\n", team->claimed);

	addKrag("team one", "99DA", kraghash,teamhash);


	printf("num claimed is %d\n", team->claimed);

	int z = addKrag("team one", "89DA", kraghash,teamhash);

	printf("num claimed is %d\n", team->claimed);

	//check for krag duplicates
	if (z == 1){
		printf("krag is a duplicate\n");
	}

	set_t * set =  getKrags("team one", teamhash);
	set_print(set, stdout, kragprint);



	 incrementTime(getFieldAgent("FA1", "team one",teamhash));


	int t = getTime("FA1", "team one", teamhash);

	printf("time t is %d\n", t);

	char * asd = randomClue("team one", kraghash, teamhash);

	set_t * setclue = getClues("team one", teamhash);

	printf("kragID from randomCLue is %s\n", asd);

	if ( asd == NULL){

		printf("is NULL\n");
	}
	set_print(setclue, stdout, clueprint);

	free(asd);


	deleteKragHash(kraghash); 
	deleteTeamHash(teamhash);
	free(secret);
	free(secretfile);
	free(kragfile);


	exit(0);

}


static void 
kragprint(FILE *fp, const char *key, void *item)
{
  printf("kragID %s\n", key);
}

static void 
clueprint(FILE *fp, const char *key, void *item)
{
  printf("clue: %s\n", (char *) item);
}


