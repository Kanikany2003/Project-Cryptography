#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MOD 26
#define MAX_SIZE 3

// Function declarations
int determinant(int matrix[MAX_SIZE][MAX_SIZE], int n);
int modInverse(int a, int m);
void matrixInverse(int keyMatrix[MAX_SIZE][MAX_SIZE], int inverseMatrix[MAX_SIZE][MAX_SIZE], int n);
void matrixMultiplication(int matrix1[MAX_SIZE][MAX_SIZE], int matrix2[MAX_SIZE][1], int result[MAX_SIZE][1], int n);
void decryptHillCipher(char *ciphertext, int keyMatrix[MAX_SIZE][MAX_SIZE], int size, FILE *outputFile);

int main() {
    int size;
    printf("Enter the dimension of the key matrix (2 or 3): ");
    scanf("%d", &size);
    if (size != 2 && size != 3) {
        printf("Error: Invalid matrix dimension. Only 2x2 and 3x3 matrices are supported.\n");
        return 1;
    }

    int keyMatrix[MAX_SIZE][MAX_SIZE] = {0};
    printf("Enter the elements of the %dx%d key matrix:\n", size, size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("Enter element [%d][%d]: ", i, j);
            scanf("%d", &keyMatrix[i][j]);
        }
    }

    char ciphertext[1024] = {0}; // Initialize to zero to ensure it is null-terminated
    FILE *inputFile, *outputFile;

    inputFile = fopen("ciphertext.txt", "r");
    if (!inputFile) {
        perror("Error");
        return 1;
    }

    // Read the input file content
    size_t bytesRead = fread(ciphertext, sizeof(char), sizeof(ciphertext) - 1, inputFile);
    if (bytesRead == 0 && ferror(inputFile)) {
        perror("Error reading file");
        fclose(inputFile);
        return 1;
    }

    fclose(inputFile);

    int ciphertextLength = strlen(ciphertext); // Store the original length of the ciphertext

    outputFile = fopen("plaintext.txt", "w");
    if (!outputFile) {
        perror("Error opening output file");
        return 1;
    }

    decryptHillCipher(ciphertext, keyMatrix, size, outputFile);

    fclose(outputFile);
    printf("Operation completed. Check plaintext.txt for the result.\n");

    return 0;
}

void decryptHillCipher(char *ciphertext, int keyMatrix[MAX_SIZE][MAX_SIZE], int size, FILE *outputFile) {
    int inverseKeyMatrix[MAX_SIZE][MAX_SIZE];
    int n = strlen(ciphertext);
    char decryptedMessage[n + 1]; // Decrypted message will be same length as ciphertext
    int i, j;

    matrixInverse(keyMatrix, inverseKeyMatrix, size);

    for (i = 0; i < n; i += size) { // Iterate over the ciphertext in blocks of size
        int cipherBlock[MAX_SIZE][1] = {0};
        int decryptedBlock[MAX_SIZE][1] = {0};

        for (j = 0; j < size; j++) {
            cipherBlock[j][0] = ciphertext[i + j] - 'A';
        }

        matrixMultiplication(inverseKeyMatrix, cipherBlock, decryptedBlock, size);

        for (j = 0; j < size; j++) {
            decryptedMessage[i + j] = (decryptedBlock[j][0] % MOD + MOD) % MOD + 'A';
        }
    }
    decryptedMessage[n] = '\0';

    fprintf(outputFile, "%s", decryptedMessage);
}

int determinant(int matrix[MAX_SIZE][MAX_SIZE], int n) {
    if (n == 2) {
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    } else if (n == 3) {
        return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1])
             - matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0])
             + matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
    }
    return 0;
}

int modInverse(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    return -1; // No modular inverse exists
}

void matrixInverse(int keyMatrix[MAX_SIZE][MAX_SIZE], int inverseMatrix[MAX_SIZE][MAX_SIZE], int n) {
    int det = determinant(keyMatrix, n);
    int invDet = modInverse(det, MOD);

    if (n == 2) {
        // Calculate adjoint matrix for 2x2
        inverseMatrix[0][0] = keyMatrix[1][1];
        inverseMatrix[0][1] = -keyMatrix[0][1];
        inverseMatrix[1][0] = -keyMatrix[1][0];
        inverseMatrix[1][1] = keyMatrix[0][0];
    } else if (n == 3) {
        // Calculate adjoint matrix for 3x3
        inverseMatrix[0][0] = (keyMatrix[1][1] * keyMatrix[2][2] - keyMatrix[1][2] * keyMatrix[2][1]);
        inverseMatrix[0][1] = -(keyMatrix[0][1] * keyMatrix[2][2] - keyMatrix[0][2] * keyMatrix[2][1]);
        inverseMatrix[0][2] = (keyMatrix[0][1] * keyMatrix[1][2] - keyMatrix[0][2] * keyMatrix[1][1]);
        inverseMatrix[1][0] = -(keyMatrix[1][0] * keyMatrix[2][2] - keyMatrix[1][2] * keyMatrix[2][0]);
        inverseMatrix[1][1] = (keyMatrix[0][0] * keyMatrix[2][2] - keyMatrix[0][2] * keyMatrix[2][0]);
        inverseMatrix[1][2] = -(keyMatrix[0][0] * keyMatrix[1][2] - keyMatrix[0][2] * keyMatrix[1][0]);
        inverseMatrix[2][0] = (keyMatrix[1][0] * keyMatrix[2][1] - keyMatrix[1][1] * keyMatrix[2][0]);
        inverseMatrix[2][1] = -(keyMatrix[0][0] * keyMatrix[2][1] - keyMatrix[0][1] * keyMatrix[2][0]);
        inverseMatrix[2][2] = (keyMatrix[0][0] * keyMatrix[1][1] - keyMatrix[0][1] * keyMatrix[1][0]);
    }

    // Apply modulo
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inverseMatrix[i][j] = (inverseMatrix[i][j] * invDet) % MOD;
            if (inverseMatrix[i][j] < 0)
                inverseMatrix[i][j] += MOD;
        }
    }
}

void matrixMultiplication(int matrix1[MAX_SIZE][MAX_SIZE], int matrix2[MAX_SIZE][1], int result[MAX_SIZE][1], int n) {
    for (int i = 0; i < n; i++) {
        result[i][0] = 0;
        for (int j = 0; j < n; j++) {
            result[i][0] = (result[i][0] + matrix1[i][j] * matrix2[j][0]) % MOD;
        }
    }
}
