# Computer Networks Project (MASTERMIND GAME)

## Description

Game where a player has to guess the random four colors code the server generates in a maximum of eight tries and before time runs out.

**Number of blacks**
Represents the number of colors correctly guessed and positioned. 

**Number of whites**
Represents the number of colors correctly guessed, but incorrectly positioned.


## Technical concepts used

Player communicates with server using **UDP** for quick and short messages and **TCP** for text files, so that potencial data losses are prevented.

Since TCP usually takes more time to complete the task, the use of **fork** to create multiple processes is needed, therefore when multiple players are trying to play the game, each of them do not have to wait for one another to use this communicaction method.

## How to compile and run

To compile and create the executables, use the makefile included.
The makefile also creates the necessary directories for the project to run. (*player_package/games player_package/scoreboards GAMES SERVER*)

```bash
make
```

To initiate the **server**, use:

```bash
./GS [-p GSport] [-v]
```

To initiate a **player**, use:

```bash
./player [-n GSIP] [-p GSport]
```

To empty the test directories refered above use:

```bash
make empty_dirs
```

## Credits

This project was developed with [Diogo Almada](https://github.com/almada39/).