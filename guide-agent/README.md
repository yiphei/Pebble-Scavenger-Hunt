# Guide Agent

This module implements a Guide Agent for the KRAG game. The Guide Agent
is a component of a team, of which there is only 1, that has its own graphical
interface displaying the status of the game (current game statistics, current 
location and status of each player on the Guide's team, current secret string, 
and a list of known clues). This interface also allows for the Guide Agent to 
send custom textual hints to a chosen Field Agent on the team. The Guide Agent
must communicate with and throughthe Game Server via the game's protocol, 
listed in the requirement spec. Lastly, the Guide Agent logs its own activity
to a logfile for the duration of the game. 

### Compiling


Exit Status: 
* 0: success
* 1: invalid argument count
* 2: duplicate argument
* 3: missing necessary argument
* 4: misinformed argument
	* ex. non-hexidecimal gameId, team/player name too long, non-integer port


### Functions

##### main
Parses and validates all parameters and passes the parameters to the guide agent
if there are no errors