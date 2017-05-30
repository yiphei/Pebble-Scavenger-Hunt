# KRAG

The KRAG game consists of three major components: the Game Server, the Guide 
Agent, and the Field Agent. The three work together to operate the 
Pebble smartwatch-based "KRAG" game around Dartmouth campus, with teams
that search for hidden krags based on clues given to them by the Game Server.

### Game Server

The Game Server acts mostly as an intermediary and a database for the game.
It receives messages from Guide Agents and Field Agents from all teams 
playing the game, responding as called for in the protocol and the requirement
spec based on the message type it receives and what is stored in that message.
The Game Server is also in charge of the logic for ending the game, sending out
a specific string of output messages to all Agents to signify the end of the
game, but allowing the agents to handle this accordingly.

### Guide Agent

The Guide Agent is a terminal based program in the KRAG game that allows one
player per team to act as a guide for their team. It delivers an ncurses-based
graphical interface to the user, displaying many different game statistics
in different windows of the interface and a window for the user to input
custom textual hints to send to all, or one, of its field agents. Most of the
Guide Agent's job is to communicate with the Game Server based on the current
status of the game, updating the Game Server with its status and staying up to
date with the game's current status.

### Field Agent

The Field Agent is the Pebble-based program that allows players to move around
campus searching for krags. The provided interface allows the user to join
a list of teams and pick from a list of player names, then giving its player
different options in terms of gameplay. The main options are to send its
location to the Game Server, to update the Server and its Guide Agent of its
whereabouts, and to claim a krag found somewhere on campus. These actions are
performed by messages following the protocol in the requirement spec and the
Field Agent, because it is on the Pebble, is more of an event-driven program 
than either of the other two major components.