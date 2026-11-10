# ProjectModernCpp – Multiplayer Card Game

A multiplayer card game developed in C++ using a client-server architecture. The application provides a graphical client for player interaction and a server responsible for handling game logic, user management, database operations and communication between clients.

## Features

- **User Authentication:** Registration and login with input validation using regular expressions.
- **Multiplayer Gameplay:** Client-server communication supporting multiple players.
- **Game Logic:** Server-side implementation of game mechanics and player interactions.
- **Power-up System:** Power-ups with individual effects, integrated into the game mechanics.
- **User Management:** Persistent user data and account-related information.
- **Database Integration:** SQLite database used for storing and managing application data, including user-specific power-ups.
- **REST API:** Communication between client and server through REST endpoints.

## Architecture

The application is divided into two main components:

### Client
The client application provides the graphical user interface and allows players to interact with the game, authenticate and access game functionality.

### Server
The server handles backend operations, including:
- Game logic and player interactions.
- User registration and authentication.
- REST API endpoints and request processing.
- Database operations and data persistence.
- Power-up management.

## Technologies

- **Language:** C++20
- **GUI:** Qt
- **Backend:** Crow
- **Database:** SQLite
- **Communication:** REST API
- **Build Environment:** Visual Studio

## Team Project

Developed collaboratively as part of an academic project.

The development involved collaborative work on the client application, server-side functionality, game mechanics and database integration.

## Demo

[Watch the project demonstration](https://www.youtube.com/watch?v=3hBUJcm7ad8)

