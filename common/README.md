# Common library

The common library will contain functions that will support all the three components of the game: field agents, guide agent, and game server. All three components will use this library in their code. The library will consists of three modules: a message module, a network module, a krag module, a team module, a word module, a log module, and a file module. The common library will also include the libcs50 library, which includes data structures like set, counters, and hashtable.

##Compiling
To compile, simply `make`.  To clean, simply `make clean`.

## Team module
The team module contains all the logic for creating teams, adding guide agents and field agents, and related functions. The module build a hashtable of teams where the key is the teamname and the item is a team struct. In the team struct itself, guide agent, and a set of field agents among other things are stored. To initialize the hashtable, call `initHash()`. Then, the hashtable is build by using `teamname` as the key and `team_t` structs as the iteams. In each team, there is a set of field agents, a set of krags, and a set of clues. In the field agents set, the key is the field agent `name` and the item is the `fieldAgent_t` struct. In the krags set, the key is the `kragID` and the item is the `krag_t` struct. In the clues set, the key is the `kragID` and the item is a string( which is the clue).

### Usage
The *team* module, defined in `team.h` and implemented in `team.c`, exports the following functions through `team.h`:

```c
/*
* This fucntion returns the revealed string of a team. 
*/
char * getRevealedString(char * teamname, hashtable_t * teamhash);

/*
* This function initializes a hashtable of teams and returns an empty hashtable.
* THis function should be called at the beginning of the game to initialize the hastable. 
* Caller is rensposible for freeing this pointer.
*/
hashtable_t * initHash(void);

/*
* This function adds a field agent to a team. If the field agent is being added to a non-existing team,
* than a team will be created first, and then the field agent will be added to the team. In a normal 
* situation, the function will return 0. If the user tries to add a field agent to a team that already
* has a field agent with the same name, then nothing happens and it returns 1.
*/
int addFieldAgent(char * name, char * pebbleID, char * teamname, char * gameID, connection_t * conn, hashtable_t * teamhash);

/*
* This function adds a guide agent to a team. If the guide agent is being added to a non-existing team,
* than a team will be created first, and then the guide agent will be added to the team. In a normal 
* situation, the function will return 0. If the user tries to add a guide agent to a team that already
* has a guide agent, then nothing happens and it returns 1.
*/
int addGuideAgent(char * guideID, char * teamname, char * name, char * gameID, connection_t * conn, hashtable_t * teamhash);

/*
* This function adds a krag to the set of krags found by a team. This function
* should be called when a team find a krag. If the krag added is a new krag, then
* the krag will be added to the sts of krags found by a team and return 0.
* If the krags added has already been found by the team, then
* nothing is added and func returns 1.
*/
int addKrag(char * teamname, char * kragID, hashtable_t * kraghash, hashtable_t * teamhash );

/*
* This function returns the guide agent of the team
*/
guideAgent_t * getGuideAgent(char * teamname, hashtable_t * teamhash);

/*
* This function returns the field agent of the team
*/
fieldAgent_t * getFieldAgent(char * name, char * teamname, hashtable_t * teamhash);

/*
* This function returns the set of all field agents in a team
*/
set_t * getAllFieldAgents(char * teamname, hashtable_t * teamhash);

/*
* This function returns the gameID of a guide agent of a team
*/
char * getGameIDGuidedA( char * teamname, hashtable_t * teamhash);

/*
* This function returns the gameID of a field agent of a team
*/
char * getGameIDFieldA(char * name, char * teamname, hashtable_t * teamhash);

/*
* This function returns the number of krags claimed by the team.
*/
int getKragsClaimed ( char * teamname, hashtable_t * teamhash);

/*
* This function returns the set of all the krags found by a team.
*/
set_t * getKrags(char * teamname, hashtable_t * teamhash);

/*
* This function returns the set of all clues that a team has
*/
set_t * getClues(char * teamname, hashtable_t * teamhash);

/*
* This function returns the most recent clue that a team has
*/
char * getClueOne(char * teamname, hashtable_t * teamhash);

/*
* This function returns the second-most recent clue that a team has
*/
char * getClueTwo(char * teamname, hashtable_t * teamhash);

/*
* This function updates the location of a field agent. If the team does not exists or the field agent
* does not exists, then nothing is done.
*/
void updateLocation(char * name, char * teamname, double longitude, double latitude, hashtable_t * teamhash);

/*
* This function increments time since guide agent last heard from a field agent
*/
void incrementTime(fieldAgent_t* fa);

/*
* Resets time to zero and resets timer
*/
void resetTime(char* name, char* teamname, hashtable_t * teamhash);

/*
* This fucntions returns the time since guide agent last heard from a field agent
*/
int getTime(char * name, char * teamname, hashtable_t * teamhash);

/*
* This function frees memory of the hashtable and everything in it.
*/
void deleteTeamHash(hashtable_t * teamhash);

/*
* This function frees memory of the hashtable and everything in it.
* This is used by the guide agent.
*/
void deleteTeamHashGA(hashtable_t * teamhash);

/*
* This function creates a new field agent.
*/
fieldAgent_t * newFieldAgent(char * gameID, char * pebbleID, connection_t * conn);

/*
* This function creates a new guide agent
*/
guideAgent_t * newGuideAgent(char * guideID, char * name, char * gameID, connection_t * conn);


/*
* This function creates a new team
*/
team_t * newTeam(void);

/*
* This function prints all the teams and all of its members
*/
void printTeams(hashtable_t * teamhash);

```
### Implementation
The `initHash` method initializes an empty hashtable.

The `addFieldAgent` method creates a new field agent and inserts it into the set of field agents with the name as the key and a team struct at the item. It first checks if the team the field agent is being inserted to exists in the first place. If not, then a team will be created and inserted in the hashtable with teamname as key and team struct as the item. Then, it will also checks if a field agents of the same name exists in the team. If yes, then the method returns 1. Otherwise, it normally returns 0.

The `addGuideAgent` method creates a new guide agent and inserts it into the team. It first checks if the team the guide agent is being inserted to exists in the first place. If not, then a team will be created and inserted in the hashtable with teamname as key and team struct as the item. Then, it will also checks if a guide agents already exists in the team. If yes, then the method returns 1. Otherwise, it normally returns 0.

The `incrementTime` method initializes a time struct, allocates memory, increments time, and assign the pointer to `lastContact` in the team struct.

 
### Assumptions
* Assume that 50 is a good size for the hashtable of teams

### Limitations
* NA

## Krag module

The krag module contains all the logic for saving krags to a hashtable, getting the secret string, revealing characters, and providing clues to krags. Given a filename, the module will read the file and store the krags in a hashtable where the key is the kragID and the item is a krag struct.

### Usage
The *krag* module, defined in `krag.h` and implemented in `krag.c`, exports the following functions through `krag.h`:

```c
/*
* This function creates a new krag and assigns coordinates to the krag. Caller
* must free the pointer.
*/
krag_t * kragNew(double longitude, double latitude);

/*
* This functions reads a kragfile and creates a hashtable where the kragID is the key and the item
* is a krag struct. Caller is rensposible for freeing the pointer
*/
hashtable_t * readKrag(char * filename);

/*
* This functions reads a secretfile and returns the secret string. 
* Caller is rensposible for freeing the pointer
*/
char * getSecretString(char * filename);

/*
* This functions returns the lenght of the secret string
*/
int getSecretStringLen(char * string);

/*
* This function reveal more characters to a team's current string. It takes the kragID for the krag found,
* teamname whose string is to be releaved, the secret string, the hashtable of the teams, and the hashtable of the krags.
* If a team calls the function for the first time, then this will set the current string of the team to be
* a series of '_' and set the number of claimed krags to zero. The function normally retuns 0. It
* only returns 1 when the team has claimed all krags. 
*/
int revealCharacters(char * kragID, char * teamname, char * secret, hashtable_t * teamhash, hashtable_t * kraghash);

/*
* This function computes the total number of krags. It takes the hashtable of krags as paramenter. 
* If kraghash is NULL, then 0 is returned.
*/
int totalKrags(hashtable_t * kraghash);

/*
* This funtion deletes the krag struct
*/
void deleteKragHash(hashtable_t * kraghash);

/*
* This function prints the kraghash with all its components.
*/
void printKrags(hashtable_t * kraghash);

/*
* This function stores a random clue for a krag that the team has not found yet. If the team has 
* collected all clues, then this function shall not be called because it will get
* stuck in an infinite loop.
*/
char * randomClue(char * teamname, hashtable_t * kraghash, hashtable_t * teamhash);
```

### Implementation

The `readKrag` method reads each line of the krag file and uses `strtok` to split the line into tokens. Then those tokens are accordingly casted and stored in a new krag struct. This krag struct will be inserted into a hashtable where the kragID is the key and the team struct is the item.

The `revealCharacters` method returns the revealed string to the caller. If a team calls the function for the first time, the string will be a series of underscores '_' of the lenght of the secret string. When a team claims a krag, then more characters will be revealed according to the krag claimed. The method for revealing characters is the following:

* Let k be the number of krags in the game, and assume the krags are numbered 0 <= i < k, according to their order in the krag file. (Note these numbers have nothing to do with the kragId.) Assume 1 < k <= n. When a team claims krag number i, reveal characters at all positions j where j % k == i.


The `randomClue` method stores a random clue in the team struct and returns the kragID of that clue. The method calls a helper method to find the clue. This method first generates a random number n < total krags in the game. Then, it will loop over the array of kragID to find a clue of a krag that has been found yet. If the loop did not find a clue, then the helper function will be called again until a valid clue has been found and the kragID of that clue is returned.


### Assumptions
* assume that 20 is a good size for the hashtable of krags
* assume there is less or equal than 20 krags
* assume the kragfile format follows the format of the requirements specs
* assume the secretfile is at most 140 characters
* the length of this string (n) may be assumed to be greater than the number of krags (k), as needed for the reveal 
* assume that the `randomClue` method wont be called for a team who has found all krags because the method will get stuck in an infinite loop.

### Limitations
* works only with less or equal than 20 krags


## Network module

The network module opens, closes, and otherwise handles sockets.
### Usage
The *network* module, defined in `network.h` and implemented in `network.c`, exports the following functions through `network.h`:

```c

/*
* Function to start server
* Returns connection
* Returns NULL if failure
*/
connection_t* startServer(int port);

/*
* Opens socket to server at host
* Returns connection type
* Returns NULL if failure
*/
connection_t* openSocket(int port, char* host);


/*
* Listens for a message
* Returns the received message, or NULL if no message received
* Messages must be freed by the user
* Takes connection type and updates address to last received address
*
* Usage:
* 
* while(true){
*	// better to save last address than do this
*	struct sockaddr* myaddr; // could be replaced by saved address
*	myaddr->sin_family = AF_INET;
*	connection_t connection = newConnection(0, myaddr);
*
*	char* message = receiveMessage(connection);
*	if(message == NULL){
*		continue;
*	}
*   printf("%s\n",message);
*	struct sockaddr* lastAddr = connection->address; // save last address
*	deleteConnection(connection); // not necessary, could reuse connection
*   free(message);
* }
*/
char* receiveMessage(connection_t* connection);

/*
* Sends a message
* Returns true on success, false on failure
* Takes connection type and sends to the address
*/
bool sendMessage(char* message, connection_t* connection);

/*
* Creates a new connection
* User is responsible for freeing it
*
*/
connection_t* newConnection(int socket, struct sockaddr address);

/*
* Deletes a connection structure
*
*
*/
void deleteConnection(connection_t* connection);

/*
* Closes the socket
*
*
*/
void closeSocket(int comm);

```

### Implementation

### Assumptions

### Limitations


## Log module
The log module contains shared function to log activity to a logfile based on given format given a logfile pointer, message, "TO" or "FROM" and a connection

### Usage
The *log* module, defined in `log.h` and implemented in `log.c`, exports the following functions through `log.h`:

```c
/* this function logs a given message to a given file in the logfile format in
the requirements spec. It uses the direction parameter to print "TO" or "FROM"
in the log message, so that input should be just "TO" or "FROM"*/
void logMessage(char *filePath, char *message, char *direction, connection_t *connect);
```

### Implementation

### Assumptions

### Limitations

## Message module

The message module parses messages into arrays of strings, implementing a message struct.

### Usage
The *message* module, defined in `message.h` and implemented in `message.c`, exports the following functions through `message.h`:

```c
/*
Initializes a new message struct with all string fields set to null
for easier memory management later on
*/

/* 
Parses messages with the help of a helper function, that will check 
each message field for its type and assign the rest of the message field 
to a specific variable inside of the message struct to be returned 
*/
message_t * parseMessage(char *message);

/* 
Goes through every possible allocated space of a specific message,
specifically all message types, and frees those that are not NULL, 
then frees the message struct itself 
*/
void deleteMessage(message_t *message);

/*
Helper function to do the brute force work of parseMessage by 
comparing specific fields of the message to known field names and 
error checking based on the requirement spec
*/
int parseHelper(char *message, message_t *parsedMessage);
```


### Implementation

### Assumptions

### Limitations


