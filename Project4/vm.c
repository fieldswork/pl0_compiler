// COP3402 - Fall 2021
// Group: Henry Graves and Arthur Upfield
// HW1 VM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

// Using a struct here to more accurately reflect how IR's fields are described as being accessed by IR.__ in the document
struct IR
{
	int OP;
	int L;
	int M;
} IR;

#define MAX_PAS_LENGTH 500 // The Process Address Space (PAS) represent the process memory and it is represented by an array of length 500
#define MAX_TEXT_LENGTH 150 // You will never be given an input file with more than text length greater than 150 lines of code

// Function copied from Appendix D - Helpful to find a variable in a different Activation Record some L levels down
int base(int L, int BP, int *PAS)
{
	int ARB = BP;
	while (L > 0)
	{
		ARB = PAS[ARB - 1];
		L--;
	}
	return ARB;
}

void execute_program(instruction *code, int outputs)
{

	// Code was calloc'd then filled with valid opcodes. Get its size by iterating until the opcode == 0
	int sizeOfCode = 0;
	while(code[sizeOfCode + 1].opcode != 0)
		sizeOfCode++;

	// Declare array for PAS and initialize register values
	int PAS[MAX_PAS_LENGTH] = {0}; // Initialize all indexes to 0
	int SP = MAX_PAS_LENGTH;  // This stack grows downwards
	int IC = sizeOfCode * 3; // Instruction Counter - counts 3 instructions per line of input code
	int BP = IC; // IC = 3 * Lines counted during input file scanning
	int GP = BP;
	int DP = IC - 1; // DP is the data pointer; for accessing variables in MAIN
	int PC = 0; // PC is the Stack pointer; points to the stop of the stack
	int FREE = IC + 40; // Free points to heap
	struct IR IR; // Setting up our IR struct for later use
	int haltFlag = 1;
	char opCode[4];

	// Fill PAS data section with values from IFP
	for (int i = 0, j = 0; j < sizeOfCode; i += 3)
	{
		PAS[i] = code[j].opcode;
		PAS[i + 1] = code[j].l;
		PAS[i + 2] = code[j].m;
		j++;
	}

	// Print headers of output
	if (outputs)
	{
		printf("                PC   BP    SP    DP    data\n");
		printf("Initial values: %d  %4d    %3d  %3d    \n", PC, BP, SP, DP);	
	}

	while (haltFlag)
	{
		// Fetch Cycle:
		// Put fetched instruction's values in Instruction Register
		IR.OP = PAS[PC];
		IR.L = PAS[PC + 1];
		IR.M = PAS[PC + 2];

		// Increment PC register (by 3). If op is JMP, CAL, JPC, or OPR 0,0 PC will be updated again during Execute Cycle
		PC += 3;

		// Execute Cycle: carry out the instruction and print an output line (pg. 11 of instruction docs)
		switch(IR.OP)
		{
			case 1: // LIT 0,M - Pushing a constant M onto the stack (or DATA)
				if (outputs)
					printf("%2d ", (PC - 1) / 3); // Prints out the program count

				strcpy(opCode, "LIT");
				if (BP == GP)
				{
					DP++;
					PAS[DP] = IR.M;
				}
				else
				{
					SP--;
					PAS[SP] = IR.M;
				}
				break;
			case 2: // OPR 0,M - Operation to be performed on the data at the top of the stack (or in data segment)
				if (outputs)
					printf("%2d ", (PC - 1) / 3); // Prints out the program count

				switch(IR.M) {
					case 0: // RTN - Return from PAS using BP to find SP
						strcpy(opCode, "RTN");
						SP = BP + 1;
						BP = PAS[SP - 3];
						PC = PAS[SP - 4];
						break;
					case 1: // NEG - Calculates the negative value of a number in PAS
						strcpy(opCode, "NEG");
						if (BP == GP)
						{
							PAS[DP] = -1 * PAS[DP];
						}
						else
						{
							PAS[SP] = -1 * PAS[SP];
						}
						break;
					case 2: // ADD - Adding two values from PAS together
						strcpy(opCode, "ADD");
						if (BP == GP)
						{
							DP--;
							PAS[DP] = PAS[DP] + PAS[DP + 1];
						}
						else
						{
							SP++;
							PAS[SP] = PAS[SP] + PAS[SP - 1];
						}
						break;
					case 3: // SUB - Subtracting two values from PAS
						strcpy(opCode, "SUB");
						if (BP == GP)
						{
							DP--;
							PAS[DP] = PAS[DP] - PAS[DP + 1];
						}
						else
						{
							SP++;
							PAS[SP] = PAS[SP] - PAS[SP - 1];
						}
						break;
					case 4: // MUL - Multiplying two values from PAS
						strcpy(opCode, "MUL");
						if (BP == GP)
						{
							DP--;
							PAS[DP] = PAS[DP] * PAS[DP + 1];
						}
						else
						{
							SP++;
							PAS[SP] = PAS[SP] * PAS[SP - 1];
						}
						break;
					case 5: // DIV - Dividing two values from PAS
						strcpy(opCode, "DIV");
						if (BP == GP)
						{
							DP--;
							PAS[DP] = PAS[DP] / PAS[DP + 1];
						}
						else
						{
							SP++;
							PAS[SP] = PAS[SP] / PAS[SP - 1];
						}
						break;
					case 6: // ODD - Finding whether a value in PAS value is odd/even
						strcpy(opCode, "ODD");
						if (BP == GP)
						{
							PAS[DP] = PAS[DP] % 2;
						}
						else
						{
							PAS[SP] = PAS[SP] % 2;
						}
						break;
					case 7: // MOD - Finding modulo of a PAS value
						strcpy(opCode, "MOD");
						if (BP == GP)
						{
							DP--;
							PAS[DP] = PAS[DP] % PAS[DP + 1];
						}
						else
						{
							SP++;
							PAS[SP] = PAS[SP] % PAS[SP - 1];
						}
						break;
					case 8: // EQL - Finding whether two PAS values are equal or not
						strcpy(opCode, "EQL");
						if (BP == GP)
						{
							DP--;
							PAS[DP] = PAS[DP] == PAS[DP + 1];
						}
						else
						{
							SP++;
							PAS[SP] = PAS[SP] == PAS[SP - 1];
						}
						break;
					case 9: // NEQ - Finding whether two PAS values are not equal
						strcpy(opCode, "NEQ");
						if (BP == GP)
						{
							DP--;
							PAS[DP] = PAS[DP] != PAS[DP + 1];
						}
						else
						{
							SP++;
							PAS[SP] = PAS[SP] != PAS[SP-1];
						}
						break;
					case 10: // LSS - Finding whether one PAS value is less than another
						strcpy(opCode, "LSS");
						if (BP == GP)
						{
							DP--;
							if (PAS[DP] < PAS[DP + 1])
								PAS[DP] = 1;
							else
								PAS[DP] = 0;
						}
						else
						{
							SP++;
							if (PAS[SP] < PAS[SP - 1])
								PAS[SP] = 1;
							else
								PAS[SP] = 0;
						}
						break;
					case 11: // LEQ - Finding whether one PAS value is less than/equal to another
						strcpy(opCode, "LEQ");
						if (BP == GP)
						{
							DP--;
							if (PAS[DP] <= PAS[DP + 1])
								PAS[DP] = 1;
							else
								PAS[DP] = 0;
						}
						else
						{
							SP++;
							if (PAS[SP] <= PAS[SP - 1])
								PAS[SP] = 1;
							else
								PAS[SP] = 0;
						}
						break;
					case 12: // GTR - Finding whether one PAS value is greater than another
						strcpy(opCode, "GTR");
						if (BP == GP)
						{
							DP--;
							if (PAS[DP] > PAS[DP + 1])
								PAS[DP] = 1;
							else
								PAS[DP] = 0;
						}
						else
						{
							SP++;
							if (PAS[SP] > PAS[SP - 1])
								PAS[SP] = 1;
							else
								PAS[SP] = 0;
						}
						break;
					case 13: // GEQ - Finding whether one PAS value is greater than/equal to another
						strcpy(opCode, "GEQ");
						if (BP == GP)
						{
							DP--;
							if (PAS[DP] >= PAS[DP + 1])
								PAS[DP] = 1;
							else
								PAS[DP] = 0;
						}
						else
						{
							SP++;
							if (PAS[SP] >= PAS[SP - 1])
								PAS[SP] = 1;
							else
								PAS[SP] = 0;
						}
						break;
				}
				break;
			case 3: // LOD L,M - Loads data from stack at offset M from L lexicographical levels down to top of stack or DATA
				if (outputs)
					printf("%2d ", (PC - 1) / 3); // Prints out the program count

				strcpy(opCode, "LOD");
				if (BP == GP)
				{
					DP++;
					PAS[DP] = PAS[GP + IR.M];
				}
				else
				{
					if (base(IR.L, BP, PAS) == GP)
					{
						SP--;
						PAS[SP] = PAS[GP + IR.M];
					}
					else
					{
						SP--;
						PAS[SP] = PAS[base(IR.L, BP, PAS) - IR.M];
					}
				}
				break;
			case 4: // STO L,M - Stores data from top of stack into stack at offset M from L lexicographical levels down, or into DATA
				if (outputs)
					printf("%2d ", (PC - 1) / 3); // Prints out the program count

				strcpy(opCode, "STO");
				if (BP == GP)
				{
					PAS[GP + IR.M] = PAS[DP];
					DP--;
				}
				else
				{
					if (base(IR.L, BP, PAS) == GP)
					{
						PAS[GP + IR.M] = PAS[SP];
						SP++;
					}
					else
					{
						PAS[base(IR.L, BP, PAS) - IR.M] = PAS[SP];
						SP++;
					}
				}
				break;
			case 5:	// CAL L,M - Call procedure at code index M
				if (outputs)
					printf("%2d ", (PC - 1) / 3); // Prints out the program count

				strcpy(opCode, "CAL");
				PAS[SP - 1] = 0; // Functional Value
				PAS[SP - 2] = base(IR.L, BP, PAS); // Static Link
				PAS[SP - 3] = BP; // Dynamic Link
				PAS[SP - 4] = PC; // Return Address
				BP = SP - 1;
				PC = IR.M;
				break;
			case 6: // INC 0,M - Allocate M memory words in stack or DATA
				if (outputs)
					printf("%2d ", (PC - 1) / 3); // Prints out the program count

				strcpy(opCode, "INC");
				if (BP == GP)
				{
					// Data segment grows upwards
					DP = DP + IR.M;
				}
				else
				{
					// Stack grows downwards
					SP = SP - IR.M;
				}
				break;
			case 7: // JMP 0,M - Jump to instruction M
				if (outputs)
					printf("%2d ", (PC - 1) / 3); // Prints out the program count

				strcpy(opCode, "JMP");
				PC = IR.M;
				break;
			case 8: // JPC 0,M - Jump to instruction M if BP == GP and if top of stack or data element is 0.
				if (outputs)
					printf("%2d ", (PC - 1) / 3); // Prints out the program count

				strcpy(opCode, "JPC");
				if (BP == GP)
				{
					if (PAS[DP] == 0)
					{
						PC = IR.M;
					}
					DP--;
				}
				else
				{
					if (PAS[SP] == 0)
					{
						PC = IR.M;
					}
					SP++;
				}
				break;
			case 9: // SYS 0,(1-3), descriptions for each are below
				strcpy(opCode, "SYS");
				switch(IR.M) {
					case 1:	// Write the top stack element or DATA element to the screen
						if (BP == GP)
						{
							printf("Top of Stack Value: %d\n", PAS[DP]);
							DP--;
						}
						else
						{
							printf("Top of Stack Value: %d\n", PAS[SP]);
							SP++;
						}

						if (outputs)
							printf("%2d ", (PC - 1) / 3);  // Prints out the program count

						break;
					case 2: // Read in input from the user and store it on top of the stack, or in DATA
						printf("Please Enter an Integer: ");
						if (BP == GP)
						{
							DP++;
							scanf("%d", &PAS[DP]);
						}
						else
						{
							SP--;
							scanf("%d", &PAS[SP]);
						}

						if (outputs)
							printf("%2d ", (PC - 1) / 3); // Prints out the program count

						break;
					case 3: // End of program, set haltFlag to 0
						haltFlag = 0;

						if (outputs)
							printf("%2d ", (PC - 1) / 3); // Prints out the program count

						break;
				}
				break;
		}

		if (outputs)
		{
			// Print the stack trace:
			printf("%3s %d %3d  %4d %4d   %4d %4d    ", opCode, IR.L, IR.M, PC, BP, SP, DP);

			for (int i = GP; i <= DP; i++)
				printf("%d ", PAS[i]);
			printf("\n");

			printf("stack: ");
			for (int i = MAX_PAS_LENGTH - 1; i >= SP; i--)
				printf("%d ", PAS[i]);
			printf("\n");
		}
	}

	// Empty line at the end of program - required
	if (outputs)
		printf("\n");

	// VM ran successfully!
	return;
}
