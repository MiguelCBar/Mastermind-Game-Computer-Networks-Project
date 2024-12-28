# Mastermind Game (Computer Networks Project)

## Description  
The Mastermind Game is a game where players attempt to guess a randomly generated four-color code within eight tries and before the timer runs out.  

- **Black pegs**: Indicate correctly guessed colors in the correct positions.  
- **White pegs**: Indicate correctly guessed colors but in the wrong positions.  

## Technical Concepts  

1. **Communication Protocols**:  
   - **UDP**: Used for quick and short messages.  
   - **TCP**: Used for transmitting text files, ensuring no data loss.  

2. **Concurrency**:  
   - The server uses `fork` to create multiple processes for TCP commands, enabling simultaneous handling of multiple players without delays.  

## How to Compile and Run  

### Compile the Project  
Use the provided `makefile` to compile and set up the required directories:  
```bash
make
```  

### Start the Server  
```bash
./GS [-p GSport] [-v]
```  
- `-p GSport`: Specify the port for the game server (optional). If omitted, it assumes the value 58058.
- `-v`: Enable verbose mode (optional).  

### Start a Player  
```bash
./player [-n GSIP] [-p GSport]
```  
- `-n GSIP`: Specify the game server's IP address (optional). If this argument is omitted, the GS should be running on the same machine.  
- `-p GSport`: Specify the port for the game server (optional). If omitted, it assumes the value 58058.

### Clear Directories  
To clean the directories (`player_package/games`, `player_package/scoreboards`, `GAMES`and `SCORES`), run:  
```bash
make empty_dirs
```  

## Credits  
This project was developed by [Miguel Barbosa](https://github.com/MiguelCBar/) and [Diogo Almada](https://github.com/almada39/).

