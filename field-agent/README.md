# Readme for Field Agent
## Paolo Takagi-Atilano, May 2017
### Compiling:
- `make all` (default): builds the Field Agent app
- `make install-emulator`: Installs the app on the basalt emulator
- `make kill`: Kills all running emulator istances
- `make clean`: Cleans everything
- `pebble install --phone X` (where x is the ip address of the phone that is paired via Bluetooth to the pebble): install onto the corresponding physical pebble. The pebble and computer must be on the same network in order for this to work.

### Output:
- `APP_LOG_LEVEL_INFO`: messages that are displayed onto the terminal to say what is going on
- Changes in the gui to handle corresponding inputs (includes manipulating window stack and displaying dialog boxes).

### Description:
The field agent is a pebble watch app that communicates with the Game Server (and Guide Agent through the Game Server).  It is a game, in which the player runs around with their pebble looking for krags, with the help of hints from the Game Server.  When they find a krag, they can input the code on the krag (slip of paper), to claim it.  Once they claim all the krags ahead of every other team, they win!

### Usage:
First, make sure tha the game server is up and running.  Then build and install the app if this has not already been done (see compiling above).  Select the app from the pebble.  At this point in time, you will be in the initial "join game menu", which will tell you to select a player and team.  After that, you may select "Join Game" (if you do this before selcting a player and a team, you will not be allowed to join the game).  You are then prompted with the in game screen where you can attempt to claim krags, look at hints, and see your status, until the game ends, in which case you will be notified, and the game will end.

### Input:
- Opcodes from Game Server
- User pebble click input

### Assumptions:
- The physical pebble is paired to a Smartphone that has Bluetooth turned on.
- The Game Server an the proxy are up and running before the "Join Game" option is selected from the initial menu.
- There are fewer than 10 krags for the game over screen (otherwise the winning team will not be determined properly).

### Limitations:
- If the physical pebble is not paried to a Smartphone that does not have Bluetooth, then the game can not be played in its entirety.  
- If the Game Server and proxy are not up and runing before the "Join Game" option is selected from the initial menu, then the game can not be played in its entirety.
