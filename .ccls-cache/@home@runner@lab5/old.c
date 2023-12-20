#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *calculateBinaryforOpOrFunct(char *line);
char *BinaryForRegisters(char *registerInput);
char *decToBinary(int n);

int MAX_LINES = 100;
int MAX_LINE_LENGTH = 100;

int main(void) {
  char *arrayOfLines[MAX_LINES]; // Array to store lines
  char line[MAX_LINE_LENGTH];    // Buffer to read each line
  FILE *file = fopen("lab5.asm", "r");

  if (file == NULL) {
    perror("Error opening file");
    return EXIT_FAILURE;
  }

  int i = 0;
  while (fgets(line, sizeof(line), file) && i < MAX_LINES) {
    arrayOfLines[i] =
        malloc(strlen(line) + 1); // Allocating memory and null terminator

    strcpy(arrayOfLines[i], line);

    printf("Line: %s", arrayOfLines[i]);
    i++;
  }

  printf("\n"); // Newlines for looks
  printf("\n");

  // removing commas
  int x, j, k;
  for (k = 0; k < i; k++) {
    int len = strlen(arrayOfLines[k]);
    char *current = arrayOfLines[k];

    for (x = j = 0; x < len; x++) {
      if (current[x] != ',') { // checking for commas in current token
        current[j++] = current[x];
      }
    }
    current[j] = '\0'; // Null-terminate the modified string
  }

  // Print the stored lines
  for (int j = 0; j < i; j++) {
    printf("Line: %s", arrayOfLines[j]);
  }

  fclose(file);

  int MAX_TOKENS = 50;
  char *token;
  char *tokens[MAX_TOKENS]; // Array of strings to store tokens
  int token_count = 0;

  // Get the first token
  token = strtok(line, " ");

  // Keep storing tokens while one exists and within the maximum limit
  while (token != NULL && token_count < MAX_TOKENS - 1) {
    tokens[token_count] = token;
    token_count++;
    token = strtok(NULL, " ");
  }
  tokens[token_count] =
      NULL; // Set the last element to NULL for indicating the end of the array

  printf("\n");
  printf("Tokens stored in the array:\n");
  for (int i = 0; i < token_count; i++) {
    printf("%s\n", tokens[i]);
  }

  calculateBinaryforOpOrFunct(tokens[0]);

  return 0;
}

char *calculateBinaryforOpOrFunct(char *tokens) {

  char *Rtype[] = {"add", "addu", "and", "jr",  "nor", "or",
                   "slt", "sltu", "sll", "srl", "sub", "subu"};

  char *Itype[] = {"addi",  "addiu", "andi", "beq", "bne", "lbu",
                   "lhu",   "ll",    "lui",  "lw",  "ori", "slti",
                   "sltiu", "sb",    "sc",   "sh"};

  char *RtypeBinary = {"100000", "100001", "100100", "001000",
                       "100111", "100101", "101010", "101011",
                       "000000", "000010", "100010", "100011"};

  char *ItypeBinary = {"001000", "001001", "001100", "000100",
                       "000101", "100100", "100101", "110000",
                       "001111", "100011", "001101", "001010",
                       "001011", "101000", "111000", "101001"};

  // Finding index of Rtype or Itype
  int foundFlag = 0;
  int isItype;
  int index;
  for (int i = 0; i < sizeof(Rtype); i++) {
    if (strcmp(Rtype[i], tokens) == 0) {
      index = i;
      foundFlag = 1;
      isItype = 0;
      break;
    }
  }
  if (foundFlag == 0) {
    for (int i = 0; i < sizeof(Itype); i++) {
      if (strcmp(Itype[i], tokens) == 0) {
        index = i;
        isItype = 1;
        break;
      }
    }
  }

  // Finding binary equivalent
  char *opOrFunctBinary;
  if (isItype == 1) {
    opOrFunctBinary = &ItypeBinary[index];
  } else {
    opOrFunctBinary = &RtypeBinary[index];
  }
  printf("%d\n", index);
  printf("%s\n", opOrFunctBinary);
  return opOrFunctBinary;
}

char *BinaryForRegisters(char *registerInput) {
  char *Registers[] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2",
                       "$a3",   "$t0", "$t1", "$t2", "$t3", "$t4", "$t5",
                       "$t6",   "$t7", "$s0", "$s1", "$s2", "$s3", "$s4",
                       "$s5",   "$s6", "$s7", "$t8", "$t9", "$k0", "$k1",
                       "$gp",   "$sp", "$fp", "$ra"};

  int index;
  for (int i = 0; i < sizeof(Registers); i++) {
    if (strcmp(Registers[i], registerInput) == 0) {
      index = i;
      break;
    }
  }
  printf("%s\n", decToBinary(index));
  return decToBinary(index);
}

char *decToBinary(int n) {
  // array to store binary number
  int binaryNum[32];

  // counter for binary array
  int i = 0;
  while (n > 0) {
    // storing remainder in binary array
    binaryNum[i] = n % 2;
    n = n / 2;
    i++;
  }

  // allocate memory for the output binary string
  char *output = malloc(sizeof(char) * (i + 1));

  // printing binary array in reverse order
  int j = 0;
  for (int j = i - 1; j >= 0; j--) {
    // convert binary to character and store in a string
    output[j] = binaryNum[j] + '0';
  }
  output[i] = '\0'; // add null-terminator to the output string
  return output;    // return the output binary string
}