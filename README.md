# CUNO
```
      ___           ___           ___           ___     
     /\  \         /\__\         /\__\         /\  \    
    /::\  \       /:/  /        /::|  |       /::\  \   
   /:/\:\  \     /:/  /        /:|:|  |      /:/\:\  \  
  /:/  \:\  \   /:/  /  ___   /:/|:|  |__   /:/  \:\  \ 
 /:/__/ \:\__\ /:/__/  /\__\ /:/ |:| /\__\ /:/__/ \:\__\
 \:\  \  \/__/ \:\  \ /:/  / \/__|:|/:/  / \:\  \ /:/  /
  \:\  \        \:\  /:/  /      |:/:/  /   \:\  /:/  / 
   \:\  \        \:\/:/  /       |::/  /     \:\/:/  /  
    \:\__\        \::/  /        /:/  /       \::/  /   
     \/__/         \/__/         \/__/         \/__/    
```

## Getting Started

Clone the repository and set up the output directory.

```bash
git clone https://github.com/Olament/UNO.git
mkdir bin
make
```

### Server

First, start the CUNO server with match size as parameter.

```bash
# Usage: ./bin/server <match_size>
```

### Client

Connect to the CUNO server with server name, port, and your username.

```bash
# Usage: ./bin/client <server name> <port> <username>
```

### Control

Use ```WASD``` to navigate your selection, ```enter``` to select and ```space``` to escape from current selection.

## Example

Here is an example of what you should see in the client terminal. 
+ The scoreboard in the upper left corner will list the players and card count.
+ There will be a left arrow by the player with the current turn and a star by the next player.
+ The big card in the top right corner will represent the card on top of the shared deck.
+ The message box will give you important notifications, including the turn of the current player.
+ You should also see the cards in your current deck as well as the draw button.

```
  ┌─SCOREBOARD───────────┐
  │         NAME  COUNT  │
  │                      │
  │                      │
  │                      │                          ┌──────┐
  │                      │                          │1     │
  │>*     Zixuan      9  │                          │      │
  │                      │                          │     1│
  │                      │                          └──────┘
  │                      │
  │                      │
  │                      │
  └──────────────────────┘
  ┌─MESSAGE──────────────────────────────────────────────────────────────────────┐
  │ 16:34:32 Your turn!                                                          │
  │ 17:42:53 Your turn!                                                          │
  │ 17:42:55 Your turn!                                                          │
  │                                                                              │
  └──────────────────────────────────────────────────────────────────────────────┘
  ┌─CARD SELECTION────────────────────────────────────────────1/2────┐  ┌────────┐
  │                                                                  │  │        │
  │ ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐       │  │        │
  │ │SKIP  │  │REV   │  │3     │  │8     │  │7     │  │9     │       │  │        │
  │ │      │  │      │  │      │  │      │  │      │  │      │       │  │        │
  │ │      │  │      │  │     3│  │     8│  │     7│  │     9│       │  │ DRAW   │
  │ └──────┘  └──────┘  └──────┘  └──────┘  └──────┘  └──────┘       │  │        │
  │                                                                  │  │        │
  └──────────────────────────────────────────────────────────────────┘  └────────┘
```
Follow standard UNO rules to select matching cards. If there are no matching cards, draw.
For example, suppose both the 1 (in the top right) and the 7 (in your deck) are colored blue. 
Then you could use the A, D, and Enter keys to select the 7. Afterwards, you would
see something like the following.

```
  ┌─SCOREBOARD───────────┐
  │         NAME  COUNT  │
  │                      │
  │                      │
  │                      │                          ┌──────┐
  │                      │                          │7     │
  │>*     Zixuan      8  │                          │      │
  │                      │                          │     7│
  │                      │                          └──────┘
  │                      │
  │                      │
  │                      │
  └──────────────────────┘
  ┌─MESSAGE──────────────────────────────────────────────────────────────────────┐
  │ 16:34:32 Your turn!                                                          │
  │ 17:42:53 Your turn!                                                          │
  │ 17:42:55 Your turn!                                                          │
  │ 17:43:06 Your turn!                                                          │
  └──────────────────────────────────────────────────────────────────────────────┘
  ┌─CARD SELECTION────────────────────────────────────────────1/2────┐  ┌────────┐
  │                                                                  │  │        │
  │ ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐       │  │        │
  │ │SKIP  │  │REV   │  │3     │  │8     │  │9     │  │WD    │       │  │        │
  │ │      │  │      │  │      │  │      │  │      │  │      │       │  │        │
  │ │      │  │      │  │     3│  │     8│  │     9│  │    WD│       │  │ DRAW   │
  │ └──────┘  └──────┘  └──────┘  └──────┘  └──────┘  └──────┘       │  │        │
  │                                                                  │  │        │
  └──────────────────────────────────────────────────────────────────┘  └────────┘
```
Continue until somebody wins!
