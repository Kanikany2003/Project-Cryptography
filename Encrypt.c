#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MOD 26
#define MAX_SIZE 3

int determinant(int matrix[MAX_SIZE][MAX_SIZE], int size) {
    if (size == 2) {
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    } else if (size == 3) {
        return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1])
             - matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0])
             + matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
    }
    return 0;
}

// Function to find the modular inverse of a number
int modInverse(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    return -1; // No modular inverse exists
}

// Function to encrypt using Hill Cipher
void hillCipherEncrypt(char *plaintext, int key[MAX_SIZE][MAX_SIZE], int size) {
    int len = strlen(plaintext);
    int padded_len = len + (size - (len % size)) % size; // Ensure the plaintext length is a multiple of size

    // Convert plaintext characters to numerical values (A=0, B=1, ..., Z=25)
    int *numerical_values = malloc(padded_len * sizeof(int));
    for (int i = 0; i < padded_len; i++) {
        if (i < len) {
            if (plaintext[i] >= 'A' && plaintext[i] <= 'Z') {
                numerical_values[i] = plaintext[i] - 'A'; // Assign numerical value (0-25) for uppercase
            } else if (plaintext[i] >= 'a' && plaintext[i] <= 'z') {
                numerical_values[i] = plaintext[i] - 'a'; // Assign numerical value (0-25) for lowercase
            } else {
                printf("Error: Invalid character '%c'. Only letters allowed.\n", plaintext[i]);
                free(numerical_values);
                return; // Exit if invalid character encountered
            }
        } else {
            numerical_values[i] = 'X' - 'A'; // 'X' as padding character
        }
    }

    // Encryption
    FILE *ciphertext_file = fopen("ciphertext.txt", "w");
    if (ciphertext_file == NULL) {
        printf("Error: Unable to open file 'ciphertext.txt' for writing.\n");
        free(numerical_values);
        return;
    }

    for (int i = 0; i < padded_len; i += size) {
        for (int j = 0; j < size; j++) {
            int sum = 0;
            for (int k = 0; k < size; k++) {
                sum += key[j][k] * numerical_values[i + k];
            }
            fputc((sum % MOD) + 'A', ciphertext_file); // Write encrypted character to file
        }
    }

    fclose(ciphertext_file);
    free(numerical_values);
}

void encrypt() {
    int size;
    printf("Enter the dimension of the key matrix (2 or 3): ");
    scanf("%d", &size);
    if (size != 2 && size != 3) {
        printf("Error: Invalid matrix dimension. Only 2x2 and 3x3 matrices are supported.\n");
        return;
    }

    int key[MAX_SIZE][MAX_SIZE] = {0};
    printf("Enter the elements of the %dx%d key matrix:\n", size, size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("Enter element [%d][%d]: ", i, j);
            scanf("%d", &key[i][j]);
        }
    }

    char plaintext[100];
    FILE *plaintext_file = fopen("plaintext.txt", "r");
    if (plaintext_file == NULL) {
        printf("Error: Unable to open file 'plaintext.txt'. Make sure the file exists in the current directory.\n");
        return;
    }

    fgets(plaintext, sizeof(plaintext), plaintext_file);
    plaintext[strcspn(plaintext, "\n")] = '\0'; // Remove newline character from fgets input
    fclose(plaintext_file);

    hillCipherEncrypt(plaintext, key, size);
}

int main() {
    encrypt();
    return 0;
}
