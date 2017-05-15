# KRAG Design
The KRAG game will be broken up into three components: Field Agent, Guide Agent, and Game Server, as well as a common library containing code used by all three.

## Field Agent
The Field Agent is a Pebble app that communicates with the Game Server and interacts with the player wearing the Pebble.

### User Interface

### Inputs and Outputs

### Functional Decomposition

### Data Structures

### Pseudo Code

### Testing Plan

## Guide Agent
The Guide Agent is a Unix program that communicates with the Game Server and interacts with the player who acts as a guide.

### User Interface

### Inputs and Outputs

### Functional Decomposition

### Data Structures

### Pseudo Code

### Testing Plan

## Game Server
The Game Server is a server that communicates with all players, from all teams, to record the game’s progress and provide information to each of the players.

### User Interface

### Inputs and Outputs

### Functional Decomposition

### Data Structures

### Pseudo Code

### Testing Plan

## Libraries

### Common
The common library will contain functions and structures to parse and handle messages. All three components will use this library in their code.