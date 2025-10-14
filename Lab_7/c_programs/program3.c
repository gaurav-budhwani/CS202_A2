/**
 * @file student_grade_system.c
 * @brief A basic command-line system for managing student grades in C.
 *
 * This program allows a user to add new students with their scores, display
 * all student records, and calculate the average score of all students.
 * It demonstrates the use of structs, arrays, loops, and conditionals for
 * data management.
 *
 * Core Concepts Illustrated:
 * - struct: Defines a 'Student' data type to logically group related
 * information (ID, name, score).
 * - Arrays: An array of 'Student' structs is used to store the records
 * for multiple students.
 * - for loop: Used extensively to iterate through the array of students for
 * displaying data, adding new records, and calculating statistics.
 * - do-while loop: Manages the main menu, allowing the user to perform
 * multiple actions until they choose to exit.
 * - if/else statements: Used for input validation (e.g., checking if the
 * database is full) and for assigning letter grades based on scores.
 * - Variables and Reassignment: Variables like 'student_count' and array
 * elements are continuously updated as the user interacts with the system.
 * - Functions: The program is modularized, with functions for each major
 * feature (adding, displaying, etc.) to enhance readability and reuse.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STUDENTS 50
#define MAX_NAME_LENGTH 50

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    double score;
} Student;

Student student_database[MAX_STUDENTS];
int student_count = 0; // Keeps track of the number of students added

void display_menu();
int get_menu_choice();
void add_student();
void display_all_students();
void calculate_average_score();
char get_letter_grade(double score);
void clear_input_buffer();
void read_string(char *str, int max_len);

/**
 * @brief Main function to run the student management system.
 *
 * Contains the main application loop which displays a menu and calls
 * the appropriate function based on the user's choice. The loop
 * continues until the user decides to exit.
 */
int main() {
    int choice;

    do {
        display_menu();
        choice = get_menu_choice();

        switch (choice) {
            case 1:
                add_student();
                break;
            case 2:
                display_all_students();
                break;
            case 3:
                calculate_average_score();
                break;
            case 4:
                printf("Exiting the program. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please enter a number between 1 and 4.\n");
                break;
        }
        printf("\nPress Enter to continue...");
        clear_input_buffer();

    } while (choice != 4);

    return 0;
}

/**
 * @brief Displays the main menu options to the user.
 */
void display_menu() {
    system("clear || cls"); // Clears the console screen (works on Linux/Windows)
    printf("==========================================\n");
    printf("   Student Grade Management System\n");
    printf("==========================================\n");
    printf("1. Add a New Student\n");
    printf("2. Display All Students\n");
    printf("3. Calculate Average Score\n");
    printf("4. Exit\n");
    printf("------------------------------------------\n");
}

/**
 * @brief Prompts the user for a menu choice and returns it.
 *
 * @return The integer value of the user's choice.
 */
int get_menu_choice() {
    int choice = 0;
    printf("Enter your choice: ");
    
    // Loop until a valid integer is entered
    while (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a number: ");
        clear_input_buffer();
    }
    clear_input_buffer(); // Crucial for consuming the rest of the line
    
    return choice;
}

/**
 * @brief Adds a new student record to the database.
 *
 * Prompts the user for the student's ID, name, and score. It validates
 * the score and checks if the database has space.
 */
void add_student() {
    if (student_count >= MAX_STUDENTS) {
        printf("Error: Student database is full. Cannot add more students.\n");
        return;
    }

    printf("\n--- Add New Student ---\n");
    
    // get Student ID
    printf("Enter Student ID: ");
    int new_id;
    while (scanf("%d", &new_id) != 1) {
        printf("Invalid ID. Please enter a number: ");
        clear_input_buffer();
    }
    clear_input_buffer();
    
    // create a temporary student and assign the ID
    Student new_student;
    new_student.id = new_id;

    // get Student Name
    printf("Enter Student Name: ");
    read_string(new_student.name, MAX_NAME_LENGTH);

    // get Student Score
    double new_score = -1.0;
    printf("Enter Student Score (0-100): ");
    while (scanf("%lf", &new_score) != 1 || new_score < 0 || new_score > 100) {
        printf("Invalid score. Please enter a number between 0 and 100: ");
        clear_input_buffer();
    }
    clear_input_buffer();
    new_student.score = new_score;

    // add the new student to the database array
    student_database[student_count] = new_student;
    student_count++; // Increment the total count of students

    printf("\nStudent added successfully!\n");
}

/**
 * @brief Displays all student records currently in the database.
 *
 * If no students are present, it prints a message. Otherwise, it iterates
 * through the student array and prints each record in a formatted table.
 */
void display_all_students() {
    printf("\n--- List of All Students ---\n");

    if (student_count == 0) {
        printf("No students in the database.\n");
        return;
    }

    printf("----------------------------------------------------------\n");
    printf("| %-5s | %-25s | %-10s | %-5s |\n", "ID", "Name", "Score", "Grade");
    printf("----------------------------------------------------------\n");

    // Loop through all students and print their details
    for (int i = 0; i < student_count; i++) {
        char grade = get_letter_grade(student_database[i].score);
        printf("| %-5d | %-25s | %-10.2f | %-5c |\n",
               student_database[i].id,
               student_database[i].name,
               student_database[i].score,
               grade);
    }
    printf("----------------------------------------------------------\n");
}

/**
 * @brief Calculates and displays the average score of all students.
 */
void calculate_average_score() {
    printf("\n--- Average Score Calculation ---\n");

    if (student_count == 0) {
        printf("Cannot calculate average. No students in the database.\n");
        return;
    }

    double total_score = 0.0;
    // Loop to sum up all scores
    for (int i = 0; i < student_count; i++) {
        total_score += student_database[i].score;
    }

    double average = total_score / student_count;
    printf("The average score for %d student(s) is: %.2f\n", student_count, average);
}

/**
 * @brief Converts a numerical score to a letter grade.
 *
 * @param score The numerical score.
 * @return The corresponding letter grade (A, B, C, D, or F).
 */
char get_letter_grade(double score) {
    if (score >= 90.0) {
        return 'A';
    } else if (score >= 80.0) {
        return 'B';
    } else if (score >= 70.0) {
        return 'C';
    } else if (score >= 60.0) {
        return 'D';
    } else {
        return 'F';
    }
}

/**
 * @brief Safely reads a line of string input from the user.
 *
 * This function reads a string from stdin and removes the trailing newline
 * character, making it safer than using scanf("%s", ...).
 * @param str The buffer to store the string in.
 * @param max_len The maximum number of characters to read.
 */
void read_string(char *str, int max_len) {
    fgets(str, max_len, stdin);
    // Remove the trailing newline character, if it exists
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

/**
 * @brief Clears the standard input buffer.
 */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
