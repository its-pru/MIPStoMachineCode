// Tommy Prusak
// CMPE220-02
// Lab 5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int number;
  char register_name[4]; // Assuming the register name won't exceed 3 characters
                         // ($t2)
} ParsedDataLwSw;

char *calculateBinaryforOpOrFunct(char *line);
char *BinaryForRegisters(char *registerInput);
char *decToBinary(int n);
char *binaryToHex(char *binary_str);
char *rTypeConcatenate(char *funct, char *rs, char *rt, char *rd);
void remove_spaces(char *s);
void remove_newline(char *str);
char *iTypeConcatenate(char *op, char *rs, char *rt, char *constOrAddress);
char *decimalToBinaryForConstant(int input);
ParsedDataLwSw parseExpression(const char *expression);
char *calculateCurrentAddress();
char* decimalToBinaryString(int decimalNum);

int MAX_LINES = 100;
int MAX_LINE_LENGTH = 100;
int numOfLines = 0;
int isItype;
int isLoadStore;
int isArithmetic;
int isLogical;
int isJump;
int isBranch;
int hexIterator = 0;

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
    // printf("Line: %s", arrayOfLines[i]);
    i++;
    numOfLines++;
  }

  fclose(file);

  //printf("\n\n"); // Newlines for looks

  // removing commas
  for (int k = 0; k < i; k++) {
    int len = strlen(arrayOfLines[k]);
    char *current = arrayOfLines[k];
    char *modifiedString = malloc(len + 1);

    int j = 0;
    for (int x = 0; x < len; x++) {
      if (current[x] != ',') { // checking for commas in current token
        modifiedString[j++] = current[x];
      }
    }
    modifiedString[j] = '\0'; // Null-terminate the modified string

    free(arrayOfLines[k]); // freeing old string
    arrayOfLines[k] = modifiedString;
  }

  // Print the stored lines after removing commas
  // for (int j = 0; j < i; j++) {
  //   printf("Line: %s", arrayOfLines[j]);
  // }
  int startingLine = 1;
  int k = 0;
  while (strncmp(arrayOfLines[k], ".text\n", 20) !=
         0) { // Ignore lines until after we see the .text directive.
    startingLine++;
    k++;
  }

  for (int i = startingLine; i < numOfLines;
       i++) { // iterating through all lines after .text

    int MAX_TOKENS = 50;
    char *token;
    char *tokens[MAX_TOKENS];
    int token_count = 0;

    isItype = 0;
    isLoadStore = 0;
    isArithmetic = 0;
    isLogical = 0;
    isJump = 0;
    isBranch = 0;

    // Get the first token
    token = strtok(arrayOfLines[i], " ");

    while (token != NULL &&
           token_count <
               MAX_TOKENS - 1) { // Kept storing tokens while one exists and
                                 // within the maximum limit
      tokens[token_count] = token;
      token_count++;
      token = strtok(NULL, " ");
    }
    tokens[token_count] = NULL; // Setting the last element to NULL for
                                // indicating the end of the array

    for (int i = 0; i < token_count;
         i++) { // remove newlines from tokens so strncmp doesnt mess up later
      remove_newline(tokens[i]);
    }
    char *temp = calculateBinaryforOpOrFunct(
        tokens[0]); // Calling this function to see whether Itype or Rtype and
                    // what type of instruction it is
    if ((isItype == 1 && isArithmetic == 1) ||
        (isItype == 1 && isLogical == 1)) {
      char *remaining;
      char *binaryOutput = iTypeConcatenate(
          calculateBinaryforOpOrFunct(tokens[0]), BinaryForRegisters(tokens[1]),
          BinaryForRegisters(tokens[2]),
          decimalToBinaryForConstant(strtol(tokens[3], &remaining, 10)));
      char *hexVal = binaryToHex(binaryOutput);

      printf("I-format: %s\n", binaryOutput);
      printf("%s: %s\n", calculateCurrentAddress(), hexVal); //print out hex value with memory address in front

      printf("\n\n");
    } else if ((isItype == 0 && isArithmetic == 1) ||
               (isItype == 0 && isLogical == 1)) {
      char *binaryOutput = rTypeConcatenate(
          calculateBinaryforOpOrFunct(tokens[0]), BinaryForRegisters(tokens[1]),
          BinaryForRegisters(tokens[2]), BinaryForRegisters(tokens[3]));
      char *hexVal = binaryToHex(binaryOutput);

      printf("R-format: %s\n", binaryOutput);
      printf("%s: %s\n", calculateCurrentAddress(), hexVal); //print out hex value with memory address in front

      printf("\n\n");
    } else if (isItype == 1 && isBranch == 1) {
      char *remaining;
      char *binaryOutput = iTypeConcatenate(
          calculateBinaryforOpOrFunct(tokens[0]), BinaryForRegisters(tokens[2]),
          BinaryForRegisters(tokens[1]),
          decimalToBinaryForConstant(strtol(tokens[3], &remaining, 10)));
      char *hexVal = binaryToHex(binaryOutput);

      printf("I-format: %s\n", binaryOutput);
      printf("%s: %s\n", calculateCurrentAddress(), hexVal); //print out hex value with memory address in front

      printf("\n\n");
    }

    else if (isItype == 1 && isLoadStore == 1) {
      ParsedDataLwSw data = parseExpression((tokens[2]));
      char *remaining;
      char *binaryOutput = iTypeConcatenate(
          calculateBinaryforOpOrFunct(tokens[0]), BinaryForRegisters(tokens[1]),
          BinaryForRegisters(data.register_name),
          decimalToBinaryForConstant(data.number));
      char *hexVal = binaryToHex(binaryOutput);

      printf("I-format: %s\n", binaryOutput);
      printf("%s: %s\n", calculateCurrentAddress(), hexVal); //print out hex value with memory address in front

      printf("\n\n");

    } else if (isItype == 0 && isJump == 1) {
      int totalLength = 33;
      char *output = (char *)malloc(totalLength * sizeof(char));
      output[0] = '\0';

      strncat(output, "000000", 33);
      strncat(output, BinaryForRegisters(tokens[1]), 33); // rs
      strncat(output, BinaryForRegisters("$zero"), 33);   // rt
      strncat(output, decimalToBinaryForConstant(8), 33); // rd

      char *hexVal = binaryToHex(output);

      printf("R-format: %s\n", output);
      printf("%s: %s\n", calculateCurrentAddress(), hexVal); //print out hex value with memory address in front

      printf("\n\n");
    }
  }

  // printf("\n");
  // printf("Tokens stored in the array:\n");
  // for (int i = 0; i < token_count; i++) {
  //   printf("%s\n", tokens[i]);
  // }

  // NEED TO ADD RIGHT SIDE OP CODES

  // char *temps = calculateBinaryforOpOrFunct(tokens[0]);
  // BinaryForRegisters(
  //     tokens[1]); // Gotta go back and fix these to not hard code 0 in
  //     front
  // BinaryForRegisters(tokens[2]);
  // char *remaining;
  // char *temp = decToBinary(strtol(tokens[3], &remaining, 10));
  // printf("%s\n", temp);
  // printf("\n");
  // long tempss = binaryToHex(temp);
  // printf("%lx\n", tempss);

  // remove_newline(tokens[3]);

  // char *remaining;
  // int integerValue = strtol(tokens[3], &remaining, 10);

  // char *tempss = iTypeConcatenate(
  //     calculateBinaryforOpOrFunct(tokens[0]),
  //     BinaryForRegisters(tokens[1]), BinaryForRegisters(tokens[2]),
  //     decimalToBinaryForConstant(strtol(tokens[3], &remaining, 10)));
  // printf("%s\n", tempss);
  return 0;
}

char *calculateBinaryforOpOrFunct(char *tokens) {

  isItype = 0;
  isLoadStore = 0;
  isArithmetic = 0;
  isLogical = 0;
  isJump = 0;
  isBranch = 0;

  // Divided up instructions by I and R type and what type of instruction they
  // are

  char *ItypeAndLoadStore[] = {"lbu", "lhu", "ll", "lui", "lw",
                               "sb",  "sc",  "sh", "sw"};

  char *ItypeAndArithmetic[] = {"addi", "addiu", "slti", "sltiu"};

  char *ItypeAndLogical[] = {"andi", "ori"};

  char *ItypeAndBranch[] = {"beq", "bne"};

  char *RtypeAndArithmetic[] = {"add", "addu", "sub", "subu", "slt", "sltu"};

  char *RtypeAndLogical[] = {"and", "or", "nor", "sll", "srl"};

  char *RtypeAndJump[] = {"jr"};

  // I have equivalent binary values for each instruction in the same index as
  // the arrays above

  char *ItypeBinaryLoadStore[] = {"100100", "100101", "110000",
                                  "001111", "100011", "101000",
                                  "111000", "101001", "101011"};

  char *ItypeBinaryArithmetic[] = {"001000", "001001", "001010", "001011"};

  char *ItypeBinaryLogical[] = {"001100", "001101"};

  char *ItypeBinaryBranch[] = {"000100", "000101"};

  char *RtypeBinaryArithmetic[] = {"100000", "100001", "100100",
                                   "100011", "101010", "101011"};

  char *RtypeBinaryLogical[] = {"100100", "100101", "100111", "000000",
                                "000010"};

  char *RtypeBinaryJump[] = {"001000"};

  // Finding index of Rtype or Itype
  int foundFlag = 0;
  int index;
  char *opOrFunctBinary;

  for (int i = 0; i < sizeof(RtypeAndArithmetic) / 8;
       i++) { // iterating throgh whole RtypeAndArithmetic array
    if (strcmp(RtypeAndArithmetic[i], tokens) ==
        0) { // comparing current string to the token from input
      index = i;
      foundFlag =
          1; // if we get here we found the instruction we were looking for
      isItype = 0;
      isArithmetic = 1;
      opOrFunctBinary = RtypeBinaryArithmetic[index]; // output
      break;
    }
  }

  if (foundFlag == 0) {
    for (int i = 0; i < sizeof(RtypeAndLogical) / 8; i++) {
      if (strcmp(RtypeAndLogical[i], tokens) == 0) {
        index = i;
        foundFlag = 1;
        isItype = 0;
        isLogical = 1;
        opOrFunctBinary = RtypeBinaryLogical[index];
        break;
      }
    }
  }

  if (foundFlag == 0) {
    for (int i = 0; i < sizeof(RtypeAndJump) / 8; i++) {
      if (strcmp(RtypeAndJump[i], tokens) == 0) {
        index = i;
        foundFlag = 1;
        isItype = 0;
        isJump = 1;
        opOrFunctBinary = RtypeBinaryJump[index];
        break;
      }
    }
  }

  if (foundFlag == 0) {
    for (int i = 0; i < sizeof(ItypeAndLoadStore) / 8; i++) {
      if (strcmp(ItypeAndLoadStore[i], tokens) == 0) {
        index = i;
        foundFlag = 1;
        isItype = 1;
        isLoadStore = 1;
        opOrFunctBinary = ItypeBinaryLoadStore[index];
        break;
      }
    }
  }

  if (foundFlag == 0) {
    for (int i = 0; i < sizeof(ItypeAndArithmetic) / 8; i++) {
      if (strcmp(ItypeAndArithmetic[i], tokens) == 0) {
        index = i;
        foundFlag = 1;
        isItype = 1;
        isArithmetic = 1;
        opOrFunctBinary = ItypeBinaryArithmetic[index];
        break;
      }
    }
  }

  if (foundFlag == 0) {
    for (int i = 0; i < sizeof(ItypeAndLogical) / 8; i++) {
      if (strcmp(ItypeAndLogical[i], tokens) == 0) {
        index = i;
        foundFlag = 1;
        isItype = 1;
        isLogical = 1;
        opOrFunctBinary = ItypeBinaryLogical[index];
        break;
      }
    }
  }

  if (foundFlag == 0) {
    for (int i = 0; i < sizeof(ItypeAndBranch) / 8; i++) {
      if (strcmp(ItypeAndBranch[i], tokens) == 0) {
        index = i;
        foundFlag = 1;
        isItype = 1;
        isBranch = 1;
        opOrFunctBinary = ItypeBinaryBranch[index];
        break;
      }
    }
  }

  // Finding binary equivalent
  // char *opOrFunctBinary;
  // if (isItype == 1 && isLoadStore == 1) {
  //   opOrFunctBinary = &ItypeAndLoadStore[index];
  // } else {
  //   opOrFunctBinary = &RtypeBinary[index];
  // }
  // printf("I type: %d\n", isItype);
  // printf("%d\n", index);
  // printf("%s\n", opOrFunctBinary);
  return opOrFunctBinary;
}

char *BinaryForRegisters(
    char *registerInput) { // calculates binary of inputted register by looking
                           // at the index of the register in the register array
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
  // printf("%s\n", registerInput);
  // printf("%s\n", decToBinary(index));
  // printf("%d\n", index);
  int temp = strlen(decToBinary(index));

  int totalLength = 6;
  char *output = (char *)malloc(totalLength * sizeof(char));
  output[0] = '\0';

  // printf("temp = %s\n", decToBinary(index));

  // making sure this binary value has 5 bits
  int difference = 5 - temp;
  while (difference > 0) {
    strncat(output, "0", 6);
    difference--;
  }
  strncat(output, decToBinary(index), 6);
  // if (temp == 4) {
  //   strncat(output, "0", 6);
  //   strncat(output, decToBinary(index), 6);
  // } else {
  //   printf("%s\n", decToBinary(index));
  //   printf("ERROR wih Binary of Registers\n");
  // }
  // printf("output = %s\n", output);
  return output;
}

char *
decToBinary(int n) { // converts decimal value to binary. Used for registers
  int temp = n;
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

  char *output = malloc(sizeof(char) * (i + 1)); // allocating memory
  int j = 0;
  for (int k = i - 1; k >= 0;
       k--) { // printing in reverse order and converting it to string
    output[j++] = binaryNum[k] + '0';
  }
  output[j] = '\0'; // null terminator
  if (temp == 0) {  // have to manually do this. The input of 0 does not work
    output = "0000\0";
  }

  return output; // Return the output binary string
}

char *
rTypeConcatenate(char *funct, char *first, char *second,
                 char *third) { // concatenates all needed stuff in R format
  int totalLength = 33;
  char *output = (char *)malloc(totalLength * sizeof(char));
  output[0] = '\0';

  strncat(output, "000000", 33);
  strncat(output, second, 33); // rs
  strncat(output, third, 33);  // rt
  strncat(output, first, 33);  // rd
  strncat(output, "00000", 33);
  strncat(output, funct, 33);
  return output;
}

char *iTypeConcatenate(char *op, char *rs, char *rt,
                       char *constOrAddress) { // concatenates in I format
  int totalLength = 33;
  char *output = (char *)malloc(totalLength * sizeof(char));
  output[0] = '\0';

  strncat(output, op, 33); // op code
  strncat(output, rt, 33);
  strncat(output, rs, 33);
  strncat(output, constOrAddress, 33); // constant or address
  return output;
}

void remove_newline(char *str) { // removes new lines from strings
  int length = strlen(str);
  if (length > 0 && str[length - 1] == '\n') {
    str[length - 1] = '\0'; // Replace newline character with null terminator
  }
}

char *
decimalToBinaryForConstant(int input) { // converts decimal to binary. Used for
                                        // I format with 16 bit values.
  int temp = strlen(decToBinary(input));

  int totalLength = 17;
  char *output = (char *)malloc(totalLength * sizeof(char));
  output[0] = '\0';
  int difference = 16 - temp;
  while (difference > 0) {
    strncat(output, "0", 17);
    difference--;
  }
  strncat(output, decToBinary(input), 17);
  // printf("output = %s\n", output);
  return output;
}

char *binaryToHex(char *binary_str) { // converts binary to hex
  int decimal_num = strtol(binary_str, NULL, 2); // Convert binary string to decimal number
  char *hex_str = (char *)malloc(11 * sizeof(char)); // Allocates memory for hexadecimal string of length 8


  if (hex_str == NULL) {
    printf("Memory allocation failed.\n");
    return NULL;
  }

  sprintf(hex_str, "0x%08X",
          decimal_num); // Convert decimal number to hexadecimal string and adds
                        // 0x in front

  return hex_str;
}

// parses token when we have a LW or SW instruction. Extracts the value and
// register
ParsedDataLwSw parseExpression(const char *expression) {
  ParsedDataLwSw data;
  data.number = 0;
  data.register_name[0] = '\0';

  sscanf(expression, "%d($%9[^)])", &data.number,
         data.register_name); // pulls out register without anything else.

  char temp[10];
  snprintf(temp, sizeof(temp), "$%s",
           data.register_name); // concatenate $ back manually.
  strncpy(data.register_name, temp, sizeof(data.register_name));

  return data;
}

char *calculateCurrentAddress() {
  char *temp = decimalToBinaryString(hexIterator);
  hexIterator = hexIterator + 4;
  return binaryToHex(temp);
}

char* decimalToBinaryString(int decimalNum) {
    char* binaryString = (char*)malloc(33 * sizeof(char)); // Allocate memory for string to hold the binary representation of 32 bits + '\0'

    if (binaryString == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    unsigned int mask = 1 << 31;

    int index = 0;

    for (int i = 0; i < 32; i++) { //iterating through each bit
        int bit = (decimalNum & mask) ? 1 : 0;
        binaryString[index++] = bit + '0'; // Converted int to char ('0' or '1')

        mask = mask >> 1;
    }
    binaryString[32] = '\0'; // Null-terminated the string
    return binaryString;
}
