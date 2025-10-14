/**
 * @file atm_simulator.c
 * @brief A simple, self-contained command-line ATM simulator in C.
 *
 * This program simulates the basic functionalities of an ATM, including
 * PIN validation, checking balance, withdrawing cash, and depositing funds.
 * It's designed to demonstrate fundamental C programming concepts.
 *
 * Core Concepts Illustrated:
 * - do-while loop: Used for the main menu, ensuring it runs at least once and
 * continues until the user chooses to exit.
 * - while loop: Used for PIN validation, prompting the user repeatedly
 * until the correct PIN is entered or attempts are exhausted.
 * - if/else if/else and switch statements: The switch statement handles menu
 * navigation, while if/else logic is used for validation checks (e.g.,
 * sufficient funds, valid deposit amount).
 * - Variables and Reassignment: Key variables like 'balance', 'pin_attempts',
 * and 'transaction_amount' are continuously updated based on user actions.
 * - Standard I/O: Uses printf() and scanf() for user interaction.
 * - Functions: The code is structured with functions to handle specific
 * tasks like displaying the menu or performing transactions, improving
 * clarity and maintainability.
 */

#include <stdio.h>
#include <stdlib.h>

#define CORRECT_PIN 1234
#define MAX_PIN_ATTEMPTS 3

double account_balance = 5000.75; // Starting account balance

void display_welcome_message();
int validate_pin();
void display_main_menu();
int get_user_choice();
void perform_transaction(int choice);
void check_balance();
void withdraw_cash();
void deposit_cash();
void clear_input_buffer();

/**
 * @brief The main function, which drives the ATM simulation.
 *
 * It starts by welcoming the user and validating their PIN. If the PIN is
 * correct, it enters a do-while loop to display the main menu and perform
 * transactions until the user chooses to exit.
 */
int main() {
    display_welcome_message();

    // authenticate the user first
    if (validate_pin()) {
        printf("\nPIN accepted. Access granted.\n");

        int user_choice;
        // main application loop
        do {
            display_main_menu();
            user_choice = get_user_choice();
            perform_transaction(user_choice);
        } while (user_choice != 4);
    } else {
        printf("\nToo many incorrect PIN attempts. Your card has been locked.\n");
        printf("Please contact your bank for assistance.\n");
    }

    printf("\nThank you for using the ATM. Goodbye!\n");

    return 0;
}

/**
 * @brief Displays the initial welcome message to the user.
 */
void display_welcome_message() {
    printf("=====================================\n");
    printf("      Welcome to the C-Bank ATM\n");
    printf("=====================================\n");
}

/**
 * @brief Prompts the user for a PIN and validates it.
 *
 * @return Returns 1 if the PIN is correct, 0 otherwise.
 * The user has a limited number of attempts.
 */
int validate_pin() {
    int entered_pin;
    int attempts = 0;

    while (attempts < MAX_PIN_ATTEMPTS) {
        printf("Please enter your 4-digit PIN: ");
        
        // check if input is a valid integer
        if (scanf("%d", &entered_pin) != 1) {
            printf("Invalid input. Please enter numbers only.\n");
            clear_input_buffer();
            attempts++;
            printf("You have %d attempt(s) remaining.\n\n", MAX_PIN_ATTEMPTS - attempts);
            continue; // Skip to the next iteration
        }
        clear_input_buffer();

        if (entered_pin == CORRECT_PIN) {
            return 1; // success
        } else {
            printf("Incorrect PIN.\n");
            attempts++;
            printf("You have %d attempt(s) remaining.\n\n", MAX_PIN_ATTEMPTS - attempts);
        }
    }

    return 0; // failure
}

/**
 * @brief Displays the main menu of ATM options.
 */
void display_main_menu() {
    printf("\n---------- ATM Main Menu ----------\n");
    printf("1. Check Account Balance\n");
    printf("2. Withdraw Cash\n");
    printf("3. Deposit Cash\n");
    printf("4. Exit\n");
    printf("-----------------------------------\n");
}

/**
 * @brief gets the user's menu choice and validates it.
 *
 * @return The integer choice from the user.
 */
int get_user_choice() {
    int choice = 0;
    printf("Please select an option: ");
    
    // Loop until a valid integer is entered
    while (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a number (1-4): ");
        clear_input_buffer();
    }
    clear_input_buffer();
    
    return choice;
}

/**
 * @brief Calls the appropriate function based on the user's choice.
 *
 * @param choice The menu option selected by the user.
 */
void perform_transaction(int choice) {
    switch (choice) {
        case 1:
            check_balance();
            break;
        case 2:
            withdraw_cash();
            break;
        case 3:
            deposit_cash();
            break;
        case 4:
            // the exit message is handled in main
            break;
        default:
            printf("Invalid option selected. Please try again.\n");
            break;
    }
}

/**
 * @brief Displays the current account balance.
 */
void check_balance() {
    printf("\n-> Your current account balance is: $%.2f\n", account_balance);
}

/**
 * @brief Handles the logic for cash withdrawal.
 *
 * Prompts the user for an amount, validates it, and updates the balance.
 */
void withdraw_cash() {
    double amount;
    printf("\n-> Enter the amount to withdraw: $");

    if (scanf("%lf", &amount) != 1) {
        printf("Invalid amount entered.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    if (amount <= 0) {
        printf("Withdrawal amount must be positive.\n");
    } else if (amount > account_balance) {
        printf("Insufficient funds. You cannot withdraw more than you have.\n");
    } else {
        // successful withdrawal
        account_balance = account_balance - amount; // Reassignment
        printf("Please take your cash: $%.2f\n", amount);
        printf("Your new balance is: $%.2f\n", account_balance);
    }
}

/**
 * @brief Handles the logic for depositing cash.
 *
 * Prompts the user for an amount, validates it, and updates the balance.
 */
void deposit_cash() {
    double amount;
    printf("\n-> Enter the amount to deposit: $");

    if (scanf("%lf", &amount) != 1) {
        printf("Invalid amount entered.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    if (amount <= 0) {
        printf("Deposit amount must be positive.\n");
    } else {
        // successful deposit
        account_balance = account_balance + amount; // Reassignment
        printf("Successfully deposited $%.2f\n", amount);
        printf("Your new balance is: $%.2f\n", account_balance);
    }
}

/**
 * @brief Clears the standard input buffer to prevent errors.
 *
 * This function is crucial for robustly handling user input with scanf.
 */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


