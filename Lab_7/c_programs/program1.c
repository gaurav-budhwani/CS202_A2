/**
 * @file text_adventure_game.c
 * @brief A simple, self-contained text-based adventure game in C.
 *
 * This program demonstrates the use of loops, conditionals (if/else),
 * and variable reassignment to create an interactive story. The player
 * navigates through different rooms, makes choices, and tries to find
 * the treasure without running out of health.
 *
 * Core Concepts Illustrated:
 * - while loop: Manages the main game loop, running until a game-over
 * condition is met.
 * - if/else if/else statements: Control the game's logic based on the
 * player's current location and input.
 * - Variables and Reassignment: Player health, score, current location,
 * and inventory flags are stored in variables that change throughout the game.
 * - Standard I/O: Uses printf() for displaying text and scanf() for
 * reading player input.
 * - Functions: The code is modularized into functions for better
 * readability and organization.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int player_health = 100;
int player_score = 0;
int current_room = 0; // 0: Start, 1: Armory, 2: Dark Forest, 3: Treasure Room, 4: Trap Room
int has_sword = 0;    // A boolean flag (0 for false, 1 for true)
int has_key = 0;      // A boolean flag
int game_over = 0;    // A boolean flag to control the main game loop

void display_introduction();
void display_status();
void handle_room_start();
void handle_room_armory();
void handle_room_dark_forest();
void handle_room_treasure();
void handle_room_trap();
int get_player_choice();
void clear_input_buffer();

/**
 * @brief The main function, entry point of the program.
 *
 * Initializes the game and runs the main game loop. The loop continues
 * as long as the 'game_over' flag is not set. Inside the loop, it displays
 * the player's status and handles the logic for the current room.
 */
int main() {
    display_introduction();

    // Main game loop
    while (game_over == 0) {
        display_status();

        if (current_room == 0) {
            handle_room_start();
        } else if (current_room == 1) {
            handle_room_armory();
        } else if (current_room == 2) {
            handle_room_dark_forest();
        } else if (current_room == 3) {
            handle_room_treasure();
        } else if (current_room == 4) {
            handle_room_trap();
        } else {
            printf("An unknown error occurred. Invalid room state.\n");
            game_over = 1;
        }

        // check for game over condition (player health)
        if (player_health <= 0) {
            printf("\nYour health has dropped to zero. You have perished.\n");
            printf("GAME OVER!\n");
            game_over = 1;
        }
    }

    printf("\nFinal Score: %d\n", player_score);
    printf("Thank you for playing!\n");

    return 0;
}

/**
 * @brief Displays the introductory text for the game.
 */
void display_introduction() {
    printf("======================================\n");
    printf(" Welcome to the C Adventure Game!\n");
    printf("======================================\n");
    printf("Your goal is to find the hidden treasure.\n");
    printf("Navigate through the rooms and make wise choices.\n");
    printf("Good luck!\n");
}

/**
 * @brief Displays the player's current health, score, and inventory.
 */
void display_status() {
    printf("\n--------------------------------------\n");
    printf("Health: %d | Score: %d | ", player_health, player_score);
    if (has_sword) {
        printf("Inventory: Sword ");
    }
    if (has_key) {
        printf("Key ");
    }
    printf("\n--------------------------------------\n");
}

/**
 * @brief Handles the logic for the starting room (Room 0).
 */
void handle_room_start() {
    printf("You are in a dimly lit starting chamber. The air is cold.\n");
    printf("There are two doors in front of you.\n");
    printf("1. Go to the door on the LEFT.\n");
    printf("2. Go to the door on the RIGHT.\n");
    printf("Choose your path (1 or 2): ");

    int choice = get_player_choice();

    if (choice == 1) {
        printf("\nYou chose the left door and enter an old armory.\n");
        current_room = 1; // Move to the Armory
    } else if (choice == 2) {
        printf("\nYou chose the right door and step into a dark forest.\n");
        current_room = 2; // Move to the Dark Forest
    } else {
        printf("Invalid choice. You hesitate and waste time.\n");
        player_health -= 5; // Penalty for invalid choice
    }
}

/**
 * @brief Handles the logic for the armory (Room 1).
 */
void handle_room_armory() {
    printf("You are in an armory. Rusted weapons line the walls.\n");
    if (has_sword == 0) {
        printf("You see a sturdy SWORD lying on a table.\n");
        printf("1. Take the SWORD.\n");
        printf("2. Leave the armory and go back to the start.\n");
        printf("Choose your action (1 or 2): ");

        int choice = get_player_choice();
        if (choice == 1) {
            printf("\nYou pick up the sword. It feels heavy but reliable.\n");
            has_sword = 1;
            player_score += 20;
        } else if (choice == 2) {
            printf("\nYou decide to leave the armory.\n");
            current_room = 0; // Go back
        } else {
            printf("Invalid choice. You stumble and lose some health.\n");
            player_health -= 5;
        }
    } else {
        printf("There is nothing else of interest here.\n");
        printf("1. Go back to the starting chamber.\n");
        printf("Choose your action (1): ");
        get_player_choice(); // Wait for user input
        current_room = 0; // Go back
    }
}

/**
 * @brief Handles the logic for the dark forest (Room 2).
 */
void handle_room_dark_forest() {
    printf("You are in a dark forest. You hear strange noises.\n");
    printf("A goblin jumps out from behind a tree!\n");

    if (has_sword == 1) {
        printf("You have a sword to defend yourself!\n");
        printf("1. Fight the goblin.\n");
        printf("2. Try to flee.\n");
        printf("Choose your action (1 or 2): ");
        int choice = get_player_choice();
        if (choice == 1) {
            printf("\nYou fight bravely and defeat the goblin!\n");
            printf("Behind the goblin, you find a hidden door and a key.\n");
            player_score += 50;
            has_key = 1;
            current_room = 3; // go to treasure room
        } else {
            printf("\nYou try to flee but the goblin strikes you as you run.\n");
            player_health -= 30;
            current_room = 0; // flee back to start
        }
    } else {
        printf("You are unarmed! The goblin attacks you.\n");
        printf("You take a serious blow before managing to escape.\n");
        player_health -= 50;
        current_room = 0; // forced to flee
    }
}

/**
 * @brief Handles the logic for the treasure room (Room 3).
 */
void handle_room_treasure() {
    printf("You are in a magnificent room filled with gold!\n");
    if (has_key == 1) {
        printf("Your key fits the lock on a large treasure chest.\n");
        printf("You open it and find the legendary treasure!\n");
        player_score += 100;
        printf("\nCONGRATULATIONS! YOU HAVE WON!\n");
        game_over = 1;
    } else {
        printf("You see a large treasure chest, but it is locked.\n");
        printf("You need a key to open it.\n");
        printf("1. Look for another way out.\n");
        printf("Choose your action (1): ");
        get_player_choice();
        printf("You find a hidden passage that leads to a trap!\n");
        current_room = 4; // Move to trap room
    }
}

/**
 * @brief Handles the logic for the trap room (Room 4).
 */
void handle_room_trap() {
    printf("You've fallen into a pit trap! It was a mistake to come here.\n");
    player_health -= 40;
    printf("You manage to climb out, but you are badly injured.\n");
    printf("You find yourself back in the starting chamber.\n");
    current_room = 0; // Go back to start
}

/**
 * @brief Gets and validates player's integer choice.
 * @return The integer choice from the user.
 */
int get_player_choice() {
    int choice = 0;
    // Loop until a valid integer is entered
    while (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a number: ");
        clear_input_buffer();
    }
    clear_input_buffer(); // Clear any trailing characters
    return choice;
}

/**
 * @brief Clears the standard input buffer.
 *
 * This is a utility function to prevent scanf from misbehaving when
 * the user enters non-numeric or extra characters. It reads characters
 * until a newline or EOF is found.
 */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
