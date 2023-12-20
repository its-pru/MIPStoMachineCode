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

  fclose(file);

  printf("\n"); // Newlines for looks
  printf("\n");

  // removing commas
  for (int k = 0; k < i; k++) {
    int len = strlen(arrayOfLines[k]);
    char *current = arrayOfLines[k];
    char *modifiedString =
        malloc(len + 1); // Allocate memory for modified string

    int j = 0;
    for (int x = 0; x < len; x++) {
      if (current[x] != ',') { // checking for commas in current token
        modifiedString[j++] = current[x];
      }
    }
    modifiedString[j] = '\0'; // Null-terminate the modified string

    // Free the old string (if required) and point arrayOfLines to the modified
    // string
    free(arrayOfLines[k]);
    arrayOfLines[k] = modifiedString;
  }

  // Print the stored lines after removing commas
  for (int j = 0; j < i; j++) {
    printf("Line: %s", arrayOfLines[j]);
  }

  int MAX_TOKENS = 50;
  char *token;
  char *tokens[MAX_TOKENS]; // Array of strings to store tokens
  int token_count = 0;

  // Get the first token
  token = strtok(arrayOfLines[6], " "); // Currently hard coded for line 6

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

  // NEED TO ADD RIGHT SIDE OP CODES

  calculateBinaryforOpOrFunct(tokens[0]);
  BinaryForRegisters(
      tokens[1]); // Gotta go back and fix these to not hard code 0 in front
  BinaryForRegisters(tokens[2]);
  char *remaining;
  char *temp = decToBinary(strtol(tokens[3], &remaining, 10));
  printf("%s\n", temp);

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
  for (int i = 0; i < sizeof(Rtype) / 8; i++) {
    if (strcmp(Rtype[i], tokens) == 0) {
      index = i;
      foundFlag = 1;
      isItype = 0;
      break;
    }
  }
  if (foundFlag == 0) {
    for (int i = 0; i < sizeof(Itype) / 8; i++) {
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
  printf("%d\n", isItype);
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
  for (int i = 0; i < sizeof(Registers) / 8; i++) {
    if (strcmp(Registers[i], registerInput) == 0) {
      index = i;
      break;
    }
  }
  printf("%s\n", registerInput);
  printf("%s\n", decToBinary(index));
  printf("%d\n", index);
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

  output[0] = '0'; // HARD CODED with 0 at beginning of register binary array

  // Printing binary array in reverse order, starting from index 1
  int j = 0;
  for (int k = i - 1; k >= 0; k--) {
    // Convert binary to character and store in a string
    output[j++] = binaryNum[k] + '0';
  }
  output[j] = '\0'; // Add null-terminator to the output string
  return output;    // Return the output binary string
}