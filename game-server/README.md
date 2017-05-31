# Game Server

This module implements a Game Server for the KRAG game. The Game Server is the center piece of the game that keeps track of all data among the players and teams and enforces the game rules. This server communicates with Guide Agents over UDP sockets and with Field Agents via a proxy server. The proxy server must also be running in order for field agents to connect.

## Compiling
The game server can be compiled using `make` in the `game-server` directory. `make test` allows the server to be run on the assigned port with a test krag file and secret file (not included in repo).

## Usage
The game server is run by calling `gameserver` and assigning values for `gameId`, which is a unique hexidecimal identifier for the game session, `kff`, which is a filepath to a krag file, `sf`, which is a file path to a secret file, and `port`, which is the port to run the server on. The Game Server prints sent and received messages to `stdin` for ease of use, all messages are also logged.

Example usage:
```
gameserver gameId=FEED kff=testKragFile sf=testSecretFile port=36587
```

### Exit Codes

* 0 - Success
* 1 - Wrong number of arguments
* 2 - Duplicate argument(s)
* 3 - Missing argument(s)
* 4 - Port is not an integer
* 5 - Failure to start server
* 6 - Failure to load krag file
* 7 - Failure to load secret file


## Assumtions

* `kff` file is in the correct format as described in the [Requirement Spec](http://www.cs.dartmouth.edu/~cs50/Labs/Project/REQUIREMENTS.html)
* `sf` file is in the correct format as described in the [Requirement Spec](http://www.cs.dartmouth.edu/~cs50/Labs/Project/REQUIREMENTS.html)
* At least one Guide Agent and one Field Agent joins every team
* No two Field Agents will join with the same Pebble ID

## Limitations

* It is impossible for a Field Agent to win the game without a Guide Agent on their team
* The game cannot end until a team wins
* If the Guide Agent quits the game and tries to rejoin with gameId 0, the server will treat this as invalid because the Guide Agent is already part of the game