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

Clone and Repository and set up the output directory

```bash
git clone https://github.com/Olament/UNO.git
mkdir bin
make
```

### Server

Start the CUNO server with match size as parameter

```bash
# Usage: ./bin/server <match_size>
```

### Client

Connect to CUNO server with server name, port, and your username

```bash
# Usage: ./bin/client <server name> <port> <username>
```

### Control

Use ```WASD``` to navigate your selection, ```enter``` to select and ```space``` to escape from current selection.
