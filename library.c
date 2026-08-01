#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "library_data.dat"

// Structure to store book information
struct Book {
    int id;
    char title[50];
    char author[50];
    int isIssued; // 0 = Available, 1 = Issued
};

// Function prototypes
void addBook();
void displayBooks();
void searchBook();
void issueBook();
void returnBook();
void deleteBook();

int main() {
    int choice;

    while (1) {
        printf("\n=========================================\n");
        printf("       LIBRARY MANAGEMENT SYSTEM         \n");
        printf("=========================================\n");
        printf("1. Add New Book\n");
        printf("2. Display All Books\n");
        printf("3. Search Book by ID or Title\n");
        printf("4. Issue Book\n");
        printf("5. Return Book\n");
        printf("6. Delete Book\n");
        printf("7. Exit\n");
        printf("-----------------------------------------\n");
        printf("Enter your choice (1-7): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear buffer
            continue;
        }

        switch (choice) {
            case 1:
                addBook();
                break;
            case 2:
                displayBooks();
                break;
            case 3:
                searchBook();
                break;
            case 4:
                issueBook();
                break;
            case 5:
                returnBook();
                break;
            case 6:
                deleteBook();
                break;
            case 7:
                printf("\nExiting program. Thank you!\n");
                exit(0);
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    }

    return 0;
}

// 1. Add a new book to the file
void addBook() {
    FILE *fp = fopen(FILE_NAME, "ab");
    if (fp == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }

    struct Book b;
    printf("\nEnter Book ID: ");
    scanf("%d", &b.id);

    // Check if ID already exists (optional safety measure)
    FILE *checkfp = fopen(FILE_NAME, "rb");
    if (checkfp != NULL) {
        struct Book temp;
        while (fread(&temp, sizeof(struct Book), 1, checkfp) == 1) {
            if (temp.id == b.id) {
                printf("Error: A book with ID %d already exists!\n", b.id);
                fclose(checkfp);
                fclose(fp);
                return;
            }
        }
        fclose(checkfp);
    }

    printf("Enter Book Title: ");
    getchar(); // Consume trailing newline
    fgets(b.title, sizeof(b.title), stdin);
    b.title[strcspn(b.title, "\n")] = 0; // Remove newline character

    printf("Enter Author Name: ");
    fgets(b.author, sizeof(b.author), stdin);
    b.author[strcspn(b.author, "\n")] = 0;

    b.isIssued = 0; // Default status is available

    fwrite(&b, sizeof(struct Book), 1, fp);
    fclose(fp);

    printf("\nSuccess: Book added successfully!\n");
}

// 2. Display all books
void displayBooks() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        printf("\nNo books found in the library yet.\n");
        return;
    }

    struct Book b;
    printf("\n-----------------------------------------------------------------\n");
    printf("%-5s | %-25s | %-20s | %-10s\n", "ID", "Title", "Author", "Status");
    printf("-----------------------------------------------------------------\n");

    while (fread(&b, sizeof(struct Book), 1, fp) == 1) {
        char *status = (b.isIssued == 1) ? "Issued" : "Available";
        printf("%-5d | %-25s | %-20s | %-10s\n", b.id, b.title, b.author, status);
    }
    printf("-----------------------------------------------------------------\n");
    fclose(fp);
}

// 3. Search for a book
void searchBook() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        printf("\nNo books found in the library to search.\n");
        return;
    }

    int choice, targetId, found = 0;
    char targetTitle[50];

    printf("\nSearch by:\n1. Book ID\n2. Book Title\nEnter choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("Enter Book ID to search: ");
        scanf("%d", &targetId);

        struct Book b;
        while (fread(&b, sizeof(struct Book), 1, fp) == 1) {
            if (b.id == targetId) {
                printf("\n--- Book Found ---\n");
                printf("ID     : %d\n", b.id);
                printf("Title  : %s\n", b.title);
                printf("Author : %s\n", b.author);
                printf("Status : %s\n", b.isIssued ? "Issued" : "Available");
                found = 1;
                break;
            }
        }
    } else if (choice == 2) {
        printf("Enter Book Title to search: ");
        getchar();
        fgets(targetTitle, sizeof(targetTitle), stdin);
        targetTitle[strcspn(targetTitle, "\n")] = 0;

        struct Book b;
        while (fread(&b, sizeof(struct Book), 1, fp) == 1) {
            // Case-insensitive-like search using strstr
            if (strcasecmp(b.title, targetTitle) == 0) {
                printf("\n--- Book Found ---\n");
                printf("ID     : %d\n", b.id);
                printf("Title  : %s\n", b.title);
                printf("Author : %s\n", b.author);
                printf("Status : %s\n", b.isIssued ? "Issued" : "Available");
                found = 1;
            }
        }
    } else {
        printf("Invalid choice!\n");
    }

    if (!found) {
        printf("\nBook not found in records.\n");
    }
    fclose(fp);
}

// 4. Issue a book
void issueBook() {
    FILE *fp = fopen(FILE_NAME, "rb+");
    if (fp == NULL) {
        printf("\nNo books available to issue.\n");
        return;
    }

    int targetId, found = 0;
    printf("\nEnter Book ID to issue: ");
    scanf("%d", &targetId);

    struct Book b;
    while (fread(&b, sizeof(struct Book), 1, fp) == 1) {
        if (b.id == targetId) {
            found = 1;
            if (b.isIssued == 1) {
                printf("\nSorry, this book is already issued!\n");
            } else {
                b.isIssued = 1;
                // Move file pointer back by one struct size to overwrite
                fseek(fp, -sizeof(struct Book), SEEK_CUR);
                fwrite(&b, sizeof(struct Book), 1, fp);
                printf("\nSuccess: Book issued successfully!\n");
            }
            break;
        }
    }

    if (!found) {
        printf("\nBook with ID %d not found.\n", targetId);
    }
    fclose(fp);
}

// 5. Return a book
void returnBook() {
    FILE *fp = fopen(FILE_NAME, "rb+");
    if (fp == NULL) {
        printf("\nNo records found.\n");
        return;
    }

    int targetId, found = 0;
    printf("\nEnter Book ID to return: ");
    scanf("%d", &targetId);

    struct Book b;
    while (fread(&b, sizeof(struct Book), 1, fp) == 1) {
        if (b.id == targetId) {
            found = 1;
            if (b.isIssued == 0) {
                printf("\nThis book was not marked as issued.\n");
            } else {
                b.isIssued = 0;
                fseek(fp, -sizeof(struct Book), SEEK_CUR);
                fwrite(&b, sizeof(struct Book), 1, fp);
                printf("\nSuccess: Book returned successfully!\n");
            }
            break;
        }
    }

    if (!found) {
        printf("\nBook with ID %d not found.\n", targetId);
    }
    fclose(fp);
}

// 6. Delete a book record
void deleteBook() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        printf("\nNo books to delete.\n");
        return;
    }

    int targetId, found = 0;
    printf("\nEnter Book ID to delete: ");
    scanf("%d", &targetId);

    FILE *tempFp = fopen("temp.dat", "wb");
    if (tempFp == NULL) {
        printf("Error creating temporary file!\n");
        fclose(fp);
        return;
    }

    struct Book b;
    while (fread(&b, sizeof(struct Book), 1, fp) == 1) {
        if (b.id == targetId) {
            found = 1; // Skip writing this book to the temp file
        } else {
            fwrite(&b, sizeof(struct Book), 1, tempFp);
        }
    }

    fclose(fp);
    fclose(tempFp);

    if (found) {
        remove(FILE_NAME);
        rename("temp.dat", FILE_NAME);
        printf("\nSuccess: Book record deleted successfully!\n");
    } else {
        remove("temp.dat");
        printf("\nBook with ID %d not found.\n", targetId);
    }
}