![Language](https://img.shields.io/badge/Language-C11-blue?style=flat)
![Platform](https://img.shields.io/badge/platform-Linux%2FUbuntu-orange?style=flat)
![Architecture](https://img.shields.io/badge/architecture-modular-purple?style=flat)
![Status](https://img.shields.io/badge/status-active%20development-4c1?style=flat)

---

## 🌀 Cosestrane
**Cosestrane** is a text-based adventure game written in C, inspired by the TV series *Stranger Things*. This project was made for the Procedural Programming exam (A.Y. 2025/2026).

---

## ⚙️ Architecture
The project was developed following C11 standards and it uses standard C libraries. The software's architecture is modular and it was divided in three different files:
*main.c*: Contains the *main()* function, managing the game menu as well as the user input.
*gamelib.c*: Contains the game engine, as well as the implementation of the core game functions.
*gamelib.h*: Contains function declarations and defines the custom data structures used in the game.

### Technical Features:
- **Dynamic Data Structures**: The game map is implemented as two mirrored doubly-linked lists of *Zona_mondoreale* and *Zona_soprasotto* structs, cross-linked to each other.
- **Memory Management**: Extensive use of dynamic memory allocation (*malloc* and *free*) to create and remove players (1 to 4) and map zones, with full cleanup on game reset.
- **Procedural Generation**: Randomly generates a 15-zone map. The algorithm assigns zone types, enemies (*Billi*, *Democane*, *Demotorzone*) and items (*Bicicletta*, *Bussola*, *Schitarrata Metallica*...) with tuned probabilities to ensure playability. Exactly one *Demotorzone* is always guaranteed in the Soprasotto.
- **Turn-Based Combat System**: Custom logic using a 20-sided die, factoring in *Attacco Psichico*, *Difesa Psichica* and *Fortuna*. Players can use inventory items mid-combat and have a 50% chance of permanently removing a defeated enemy from the zone.

---

## 🚀 Compilation & Execution
The game is designed to be compiled in a Linux environment (tested on Ubuntu 24.04.3 LTS).

To compile the project, run the following commands in your terminal:

```bash
gcc -c main.c -std=c11 -Wall
gcc -c gamelib.c -std=c11 -Wall
gcc -o gioco main.o gamelib.o
```

The `-std=c11` and `-Wall` flags ensure compliance with the C 2011 standard and help remove all warnings.

To start the game, run:

```bash
./gioco
```

---

Author: Filippo Castagnola - 376476
