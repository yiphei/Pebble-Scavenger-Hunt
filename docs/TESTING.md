# Testing Spec

## Guide Agent 

###Unit Testing

To test the Guide Agent by itself, I edited the chatserver2.c client-server
communication so that the server could send messages back to the client, also
displaying any input gotten from the Guide Agent itself. This program is 
called server.c in the testing subdirectory.

I have attached a test run with the output of both the server and the Guide
Agent display from this server output.

##### Test runs

Before the end of the game, but with most of the game progress made:

```
|                                           //                                            GAME STATS────────────────────────┐
|yifei                                     //                                             │                                 │
|tony                                     //                         Kotz Tower      //       │Total krags in game: 4           │
|                                        //                          X          //        │Total krags claimed: 4           │
|                                       //                        Sudikoff     //         └─────────────────────────────────┘
|=======================================||                                    //          CURRENT STRING────────────────────┐
|                       ||              ||          X                            //           │                                 │
|                       ||              ||    Kemeny                        //            │computer science 50 rocks!       │
|                       ||              ||                                 //             │                                 │
|                       ||              ||                                //              │                                 │
|                       ||              ||                               //               └─────────────────────────────────┘
|                       ||              ||                              ||         X      CLUES─────────────────────────────┐
|     Fahey     Russell ||          X     ||                              ||     Fairchild  │                                 │
|     X          X    ||    Silsby    ||                              ||                │                                 │
|     McLane     Sage   ||              ||                              ||                │                                 │
|                       ||              ||             Baker            ||                │                                 │
|=======================================||               X              ||                │                                 │
|                       ||              ||             Berry            ||                │                                 │
|                       ||              ||            Library           ||                │                                 │
|             Gold Coast      ||              ||                              ||        X         │                                 │
| X    X    X        X    ||           X  ||    X                          krag               │                                 │
|                       ||              ||                              ||                │                                 │
|                       ||              ||                              ||            X   │                                 │
|                       ||==============||                      Rauner  ||                │                                 │
|                       ||              ||                        X     ||    X         │                                 │
|                       ||              ||                              ||    Rkragns     │                                 │
|                  X    ||              ||==============================||    Chapel        │                                 │
|                       ||            X   ||\\                          //||                │                                 │
|                Mass   ||              || \\                        // ||                │                                 │
|                  X    ||              ||  \\                      //  ||                │                                 │
|                 Row   ||              ||   \\                    //   ||                │                                 │
|                       ||    McNutt    ||    \\                  //    ||                │                                 │
|                  X    ||         X          ||     \\                //     ||     X          │                                 │
|                       ||              ||      \\              //          ||                │                                 │
|                       ||              ||       \\            //     || Dartmouth      │                                 │
|=======================||==============||==============================||     X          │                                 │
|                       ||              ||           //            \\       ||   Hall         │                                 │
|         X             ||     X        ||      //              \\      ||                │                                 │
|        Foco           ||              ||     //                \\     ||     X          │                                 │
|                       ||              ||    //                  \\    ||                │                                 │
|                       ||              ||   //                    \\   ||                │                                 │
|                       ||     X    ||  //                      \\ *||                │                                 │
|                       ||    Collis  || //                        \\ ||                │                                 │
|                       ||              ||//                          \\||                │                                 │
==========================================================================================│                                 │
                                                                                          └─────────────────────────────────┘
INPUT───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                                                           │
│                                                                                                                           │
│                                                                                                                           │
└───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

The messages and activity from the test server side:

```
[mzp@moose ~/cs50/grep/testing]$ server
Ready at port 36587
[129.170.213.207@46895]: opCode=GA_STATUS|gameId=0|guideId=9876|team=One|player=Michael|statusReq=1
: opCode=GAME_STATUS|gameId=9999|guideId=9876|numClaimed=2|numKrags=4
: opCode=GS_AGENT|gameId=9999|pebbleId=12345679|team=grep|player=tony|latitude=43.703|longitude=-72.287|lastContact=0
: opCode=GS_CLUE|gameId=9999|guideId=9876|kragId=0456|clue=Check Dartmouth Hall!
: [129.170.213.207@46895]: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=0
: opCode=GS_CLUE|gameId=9999|guideId=9876|kragId=0346|clue=Rollins Chapel? Maybe!
: opCode=GS_AGENT|gameId=9999|pebbleId=12345678|team=grep|player=yifei|latitude=43.712|longitude=-72.2864|lastContact=0
: [129.170.213.207@46895]: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=0
: [129.170.213.207@46895]: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=1
: opCode=GS_CLAIMED|gameId=9999|guideId=9876|pebbleId=12345679|kragId=0346|latitude=43.705|longitude=-72.287
: opCode=GS_CLAIMED|gameId=9999|guideId=9876|pebbleId=12345678|kragId=0456|latitude=43.7045|longitude=-72.2864
: [129.170.213.207@46895]: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=0
: opCode=GS_SECRET|gameId=9999|guideId=9876|secret=compu___ _cie____50 ______
: opCode=TEAM_RECORD|gameId=9999|team=grep|numClaimed=4|numPlayers=4
: [129.170.213.207@46895]: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=0
: opCode=TEAM_RECORD|gameId=9999|team=aqua|numClaimed=2|numPlayers=3
: [129.170.213.207@46895]: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=1
: [129.170.213.207@46895]: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=0
: opCode=GS_SECRET|gameId=9999|guideId=9876|secret=computer science 50 rocks!
: [129.170.213.207@46895]: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=0
: [129.170.213.207@46895]: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=1
: opCode=GAME_OVER|gameId=9999|secret=computer science 50 rocks!
: 
```

The guide agent's game over display:

```


                              GAME OVER

                              Team: aqua
                              Players: 3
                              Krasgs claimed: 2

                              Team: grep
                              Players: 4
                              Krasgs claimed: 4


                              Press any key to exit


```

The log file after the game:

```
(Tue May 30 13:27:59 2017) TO 129.170.213.207@3: opCode=GA_STATUS|gameId=0|guideId=9876|team=One|player=Michael|statusReq=1 
(Tue May 30 13:28:04 2017) FROM 129.170.213.207@3: opCode=GAME_STATUS|gameId=9999|guideId=9876|numClaimed=2|numKrags=4 
(Tue May 30 13:28:08 2017) FROM 129.170.213.207@3: opCode=GS_AGENT|gameId=9999|pebbleId=12345679|team=grep|player=tony|latitude=43.703|longitude=-72.287|lastContact=0 
(Tue May 30 13:28:12 2017) FROM 129.170.213.207@3: opCode=GS_CLUE|gameId=9999|guideId=9876|kragId=0456|clue=Check Dartmouth Hall! 
(Tue May 30 13:28:12 2017) TO 129.170.213.207@3: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=0 
(Tue May 30 13:28:17 2017) FROM 129.170.213.207@3: opCode=GS_CLUE|gameId=9999|guideId=9876|kragId=0346|clue=Rollins Chapel? Maybe! 
(Tue May 30 13:28:22 2017) FROM 129.170.213.207@3: opCode=GS_AGENT|gameId=9999|pebbleId=12345678|team=grep|player=yifei|latitude=43.712|longitude=-72.2864|lastContact=0 
(Tue May 30 13:28:22 2017) TO 129.170.213.207@3: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=0 
(Tue May 30 13:28:27 2017) TO 129.170.213.207@3: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=1 
(Tue May 30 13:28:36 2017) TO 129.170.213.207@3: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=0 
(Tue May 30 13:28:41 2017) FROM 129.170.213.207@3: opCode=GS_SECRET|gameId=9999|guideId=9876|secret=compu___ _cie____50 ______ 
(Tue May 30 13:28:46 2017) FROM 129.170.213.207@3: opCode=TEAM_RECORD|gameId=9999|team=grep|numClaimed=4|numPlayers=4 
(Tue May 30 13:28:46 2017) TO 129.170.213.207@3: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=0 
(Tue May 30 13:28:50 2017) FROM 129.170.213.207@3: opCode=TEAM_RECORD|gameId=9999|team=aqua|numClaimed=2|numPlayers=3 
(Tue May 30 13:28:50 2017) TO 129.170.213.207@3: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=1 
(Tue May 30 13:29:05 2017) TO 129.170.213.207@3: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=0 
(Tue May 30 13:29:09 2017) FROM 129.170.213.207@3: opCode=GS_SECRET|gameId=9999|guideId=9876|secret=computer science 50 rocks! 
(Tue May 30 13:29:19 2017) TO 129.170.213.207@3: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=0 
(Tue May 30 13:29:24 2017) TO 129.170.213.207@3: opCode=GA_STATUS|gameId=9999|guideId=9876|team=One|player=Michael|statusReq=1 
(Tue May 30 13:29:56 2017) FROM 129.170.213.207@3: opCode=GAME_OVER|gameId=9999|secret=computer science 50 rocks!
```

This unit testing shows the functionality of the guide agent on its own, from
validating and parsing messages to handling the display of the game status 
upon receipt of messages that affect it. Also, it shows the intermittent
delivery of the GA_STATUS message to the server, with and without a statusReq.

Below I have attached a sample run of running the program with various
incorrect arguments from the command line:

```
[mzp@moose ~/cs50/grep/guide-agent]$ guideagent guideId=9876 player=Michael team=One host=moose.cs.dartmouth.edu port=abc
port is not an integer

[mzp@moose ~/cs50/grep/guide-agent]$ guideagent guideId=abdsjjjhadf player=Michael team=One host=moose.cs.dartmouth.edu port=36587
guideId should be 1-8 characters

[mzp@moose ~/cs50/grep/guide-agent]$ guideagent guideId=9876 player=Michaelisalongname team=One host=moose.cs.dartmouth.edu port=abc
max player name length is 10 characters

[mzp@moose ~/cs50/grep/guide-agent]$ guideagent guideId=9876 player=Michaelisalongname team=One host=moose.cs.dartmouth.edu
missing port=... argument

[mzp@moose ~/cs50/grep/guide-agent]$ guideagent guideId=9876 player=Michael guideId= 9877 team=One host=moose.cs.dartmouth.edu port=36587
duplicate guideId argument

### still runs but gives a warning and ignores the extra argument
[mzp@moose ~/cs50/grep/guide-agent]$ guideagent guideId=9876 player=Michael team=One host=moose.cs.dartmouth.edu port=36587 argument=extra
warning: unrecognized argument argument=extra

```

##### Valgrind test

The usage of ncurses for the display causes still reachable memory loss while
running the Guide Agent. However, there are no bytes directly/indirectly lost
from the display or the agent itself. I ran the same exact run as above in 
`valgrind --show-leak-kinds=definite guideagent...` and directed the stderr to
a separate text file to show this:

```
==20368== Memcheck, a memory error detector
==20368== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
==20368== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
==20368== Command: guideagent guideId=9876 player=Michael team=One host=moose.cs.dartmouth.edu port=36587
==20368== 
==20368== 
==20368== HEAP SUMMARY:
==20368==     in use at exit: 155,827 bytes in 221 blocks
==20368==   total heap usage: 724 allocs, 503 frees, 21,241,400 bytes allocated
==20368== 
==20368== LEAK SUMMARY:
==20368==    definitely lost: 0 bytes in 0 blocks
==20368==    indirectly lost: 0 bytes in 0 blocks
==20368==      possibly lost: 0 bytes in 0 blocks
==20368==    still reachable: 155,827 bytes in 221 blocks
==20368==         suppressed: 0 bytes in 0 blocks
==20368== Rerun with --leak-check=full to see details of leaked memory
==20368== 
==20368== For counts of detected and suppressed errors, rerun with: -v
==20368== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
## Guide Agent - display.c
To test the display module, I wrote a `displaytest.c` file that tests the all the basic functionalities of the module.

### Unit Testing
In the `displaytest.c`, all the function of the two modules were individually tested.

#### Usage
To test, simply type `make test`

#### Test Cases
* creating the five windows: map, input, clue, stirng, and game stats
* creating the map with the field agent players and krags on the map
* display the clues to the clue window. Also testing that clues wont go out the clues window
* display the revealed string
* display the game stats
* display the game over screen
* listening from user input and returning the input to the caller


## Game Server

### Unit Testing

To apply unit testing to the Game Server, a chat client modeled from the class chat client was used to send test messages to the Game Server in a similar way to the Guide Agent Testing. This multiple agents could easily be created and information such as location could be spoofed to test all aspects of the game server and krag validation.

#### Usage
To compile the unit testing chat bot, use the command in the `testing` directory:
```
mygcc fakepebble.c ../common/file.c -o pebble
```

The `pebble` program can then be called by using the command `pebble host port`.


#### Test Cases

* Incorrect argument parameters
      * Incorrect `kff` and `sf`
      * `port` that is already in use
* Incorrect message syntax
      * Invalid fields
      * Duplicate fields
      * Invalid types
* Incorrect data in message
      * Incorrect `gameId`
      * Incorrect player identifiers such as `gameId`, `guideId`, `player`, and `pebbleId`
      * Incorrect `opCode`, both non-existent and codes unused by the Game Server


#### Integration Testing

To test the integrated system, the Guide Agents and Field Agents were added to the game via both the `fakepebble.c` program and the actual Field and Guide Agents. Several different game scenarios were played out including with different player types, numbers of players, and numbers of teams. This ensured that all three components work together seemlessly.

##Field Agent

### Unit Testing
To apply unit testing to the Field Agent, a chat client modeled from the class chat client was used to send test messages to the Field Agent in a similar way to the Guide Agent and Game Server Testing.  With this, multiple agents could easily be created, and informatoin could be spoofed to test all aspects of the field agent and krag validification.  It was also easy to check certain events that may not necessarily happen during gameplay but could, such as getting messages that the Field Agent should not get, or just invalid messages.

#### Usage
To compile the unit testing chat bot, use the command in the `testing` directory:
```
mygcc fakepebble.c ../common/file.c -o pebble
```

The `pebble` program can then be called by using the command `pebble host port`.

### Test Cases
* Duplicate player names
* Invalid game id
* Invalid player id
* Incorrect message syntax
      * Invalid fields
      * Duplicate fields
      * Invalid types
* Incorrect or erroneous data in message
      * incorrect `gameId`
      * incorrect `opCode`, both ones that exist and ones that do not.

###Integration testing
To test the integrated system, the Guide Agents and Field Agents were added to the game via both teh `fakepebble.c` program and the actual FIELD and GUIDE Agents. Severl different game scenarios were played out including with different player types, numbers of players, and numbers of teams. This ensured that all three components work together seemlessly.  Furthermore, different problems were tested, to see what happened when the same player name was picked twice, or if a krag was already calimed, etc.



## Common library - Team and Krag module

To test the team and krag module, I wrote a `commontest.c` file that tests the all the basic functionalities of the two modules.

### Unit Testing
In the `commontest.c`, all the function of the two modules were individually tested.

#### Usage

To test, simply type `make test`

#### Test Cases
* adding two field agents of the same name
* adding a guide agent to a team that already has a guide agent
* revealing more characters when a krag is claimed
* adding a krag that has already been found to the set of krags found by the team
* increment the time of the last contact with field agent
* get a random clue
* delete the krag and team hashtables