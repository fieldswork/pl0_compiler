// COP3402 - Fall 2021
// Group Members: Arthur Upfield and Henry Graves
// HW3 Parser - Code Generator

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

// Creating Data Structure
instruction *code;
int cIndex = 0;
symbol *table;
int tIndex = 0;
int currToken = 0;

// Functional Prototypes
void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();
void MARK(int level);
int MULTIPLEDECLARATIONCHECK(int level, char *symbolName);
int FINDSYMBOL(char *symbolName, int kind);
int STATEMENT(lexeme *list, int level);
int CONST_DECLARATION(lexeme *list, int level);
int VAR_DECLARATION(lexeme *list, int level);
int PROCEDURE_DECLARATION(lexeme *list, int level);
int BLOCK(lexeme *list, int level);
int CONDITION(lexeme *list, int level);
int EXPRESSION(lexeme *list, int level);
int TERM(lexeme *list, int level);
int FACTOR(lexeme *list, int level);
int PROGRAM(lexeme *list);
instruction *parse(lexeme *list, int printTable, int printCode);

// Parse function; handles initialization and beginning calls
instruction *parse(lexeme *list, int printTable, int printCode)
{
	// Initializing code & table
	code = NULL;
	code = (instruction*)calloc(MAX_CODE_LENGTH, sizeof(instruction));
	table = (symbol*)calloc(MAX_SYMBOL_COUNT, sizeof(symbol));

	// Runs PROGRAM (and all functions included in it), stops code execution if an error is encountered.
	if (PROGRAM(list) == -1)
		return NULL;

    // Very important, and provided line!
	code[cIndex].opcode = -1;

	// Handles printing flags
	if (printTable)
		printsymboltable();
	if (printCode)
		printassemblycode();

	return code;
}

// PROGRAM; Returns -1 if error
int PROGRAM(lexeme *list)
{
	emit(7, 0, 0); // Emits JMP
	addToSymbolTable(3, "main", 0, 0, 0, 0);
	int level = -1;

	if (BLOCK(list, level) == -1)
		return -1;

	if (list[currToken].type != periodsym)
	{
		// Error: Program must be closed by a period
		printparseerror(1);
		return -1;
	}

	emit(9, 0, 3); // Emits HALT

	// For each code line, set m as table address
	for (int i = 0; i < cIndex; i++)
	{
		if (code[i].opcode == 5)
			code[i].m = table[code[i].m].addr;
	}

	code[0].m = table[0].addr;
}

// BLOCK; returns -1 if error
int BLOCK(lexeme *list, int level)
{
	level++; // Incrementing level
	int procedureIDX = tIndex - 1;

	int numConsts = CONST_DECLARATION(list, level);

	if (numConsts == -1)
		return -1; // Error case

	int numVars = VAR_DECLARATION(list, level);
	if (numVars == -1)
		return -1; // Error case

	int numProcs = PROCEDURE_DECLARATION(list, level);
	if (numProcs == -1)
		return -1; // Error case

	// Sets procedure table address to proper code index
	table[procedureIDX].addr = cIndex * 3;

	if (level == 0)
		emit(6, 0, numVars); // Emits INC
	else
		emit(6, 0, numVars + 4); // Emits INC for 4 control information

	if (STATEMENT(list, level) == -1)
		return -1;

	MARK(level);

	level--; // Decrementing level

	return 0; // Run successful
}

// MARK; no return value
void MARK(int level)
{
	// Linear search; starting from the end of the table
	for (int i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 0 && table[i].level == level)
			table[i].mark = 1;
		if (table[i].mark == 0 && table[i].level < level)
			break;
	}

	return;
}

// MULTIPLEDECLARATIONCHECK; returns index of the level of an unmarked index
int MULTIPLEDECLARATIONCHECK(int level, char *symbolName)
{
	// Start at last index in symbol table and work backwards
	for (int i = tIndex - 1; i >= 0; i--)
 	{
    	if (strcmp(table[i].name, symbolName) == 0 && table[i].mark == 0 && table[i].level == level)
     		return i; // Target was found!
 	}

 	return -1; // Nothing was found!
}

// FINDSYMBOL; finds the symbol whose level is closest to the current level
int FINDSYMBOL(char *symbolName, int kind)
{
	int maxLevelIndex = -1;
	// Start at last index in symbol table and work backwards
	for (int i = tIndex - 1; i >= 0; i--)
 	{
		// Finds matching symbols and tries to maximize the level value
    	if (strcmp(table[i].name, symbolName) == 0 && table[i].mark == 0 && table[i].kind == kind)
    	{
     		if (maxLevelIndex == -1)
     			maxLevelIndex = i;
     		else if (table[i].level > table[maxLevelIndex].level)
     			maxLevelIndex = i;
    	}
 	}

 	return maxLevelIndex;
}

// CONST-DECLARATION; returns -1 if error
int CONST_DECLARATION(lexeme *list, int level)
{
	int numConsts = 0;

	// If the current token is a constant...
	if (list[currToken].type == constsym)
	{
		do
		{
			numConsts++; // Increment constant count!
			currToken++; // Get next token
			if (list[currToken].type != identsym)
			{
				// Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'
				printparseerror(2);
				return -1;
			}

			// Check for multiple declarations
			int symidx = MULTIPLEDECLARATIONCHECK(level, list[currToken].name); // May need to pass level as well.

			if (symidx != -1)
			{
				// Error: Confliciting symbol declarations
				printparseerror(18);
				return -1;
			}

			// Store the name of the current identifier
			char identName[12];
			strcpy(identName, list[currToken].name);

			currToken++; // Get next token

			if (list[currToken].type != assignsym)
			{
				// Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'
				printparseerror(2);
				return -1;
			}

			currToken++; // Get next token

			if (list[currToken].type != numbersym)
			{
				// Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'
				printparseerror(2);
				return -1;
			}

			// Add the identifier to the symbol table
			addToSymbolTable(1, identName, list[currToken].value, level, 0, 0);

			currToken++; // Get next token
		} while (list[currToken].type == commasym);

		if (list[currToken].type != semicolonsym)
			if(list[currToken].type == identsym)
			{
				// Error: Multiple symbols in variable and constant declarations must be separated by commas
				printparseerror(13);
				return -1;
			}
			else
			{
				// Error: Symbol declarations should close with a semicolon
				printparseerror(14);
				return -1;
			}

		// Get next token
		currToken++;
	}

	return numConsts;
}

// VAR_DECLARATION; returns -1 if error
int VAR_DECLARATION(lexeme *list, int level)
{
	int numVars = 0;

	// If the current token is a variable...
	if (list[currToken].type == varsym)
	{
		do
		{
			numVars++; // Increment number of variables
			currToken++; // Get next token

			if (list[currToken].type != identsym)
			{
				// Error: Variable declarations should follow the pattern 'ident {, ident}'
				printparseerror(3);
				return -1;
			}

			int symidx = MULTIPLEDECLARATIONCHECK(level, list[currToken].name);

			if (symidx != -1)
			{
				// Error: Confliciting symbol declarations
				printparseerror(18);
				return -1;
			}

			if (level == 0)
			{
				// Add the current variable to the symbol table
				addToSymbolTable(2, list[currToken].name, 0, level, numVars-1, 0);
			}
			else
			{
				// Add the current variable to the symbol table
				addToSymbolTable(2, list[currToken].name, 0, level, numVars+3, 0);
			}

			currToken++; // Get next token
		} while (list[currToken].type == commasym);

		// If token isn't a semicolon...
		if (list[currToken].type != semicolonsym)
			if(list[currToken].type == identsym) // But is identsym!
			{
				// Error: Multiple symbols in variable and constant declarations must be separated by commas
				printparseerror(13);
				return -1;
			}
			else // Otherwise...
			{
				// Error: Symbol declarations should close with a semicolon
				printparseerror(14);
				return -1;
			}

		currToken++; // Get next token
	}

	return numVars;
}

// PROCEDURE_DECLARATION; returns -1 if error
int PROCEDURE_DECLARATION(lexeme *list, int level)
{
	int numProcs = 0;

	// While token is procsym
	while (list[currToken].type == procsym)
	{
		currToken++; // Get next token

		if (list[currToken].type != identsym)
		{
			// Error: Procedure declarations should follow the pattern 'ident ;'
			printparseerror(4);
			return -1;
		}


		// If there is already a symbol with the given name, get its index.
		int symidx = MULTIPLEDECLARATIONCHECK(level, list[currToken].name);

		if (symidx != -1)
		{
			// Error: Confliciting symbol declarations
			printparseerror(18);
			return -1;
		}

		// Add current procedure to the symbol table
		addToSymbolTable(3, list[currToken].name, 0, level, 0, 0);

		currToken++; // Get next token

		if (list[currToken].type != semicolonsym)
		{
			// Error: Procedure declarations should follow the pattern 'ident ;'
			printparseerror(4);
			return -1;
		}

		currToken++; // Get next token

		if (BLOCK(list, level) == -1)
			return -1;

		if (list[currToken].type != semicolonsym)
		{
			// Error: Symbol declarations should close with a semicolon
			printparseerror(14);
			return -1;
		}

		currToken++; // Get next token

		emit(2, 0, 0); // Emit RTN
	}

	return numProcs;
}

// STATEMENT; returns -1 if error
int STATEMENT(lexeme *list, int level)
{
	if (list[currToken].type == identsym)
	{
		int symbolIDX = FINDSYMBOL(list[currToken].name, 2);
		if (symbolIDX == -1)
		{
			// Error if symbol was a constant or procedure.
			if (FINDSYMBOL(list[currToken].name, 1) != FINDSYMBOL(list[currToken].name, 3))
			{
				// Error: Only variables may be assigned to or read.
				printparseerror(6);
				return -1;
			}

			else
			{
				// Error: Undeclared identifier.
				printparseerror(19);
				return -1;
			}
		}

		currToken++; // Get next token

		if (list[currToken].type != assignsym)
		{
			// Error: Variables must be assigned using :=
			printparseerror(5);
			return -1;
		}

		currToken++; // Get next token

		if (EXPRESSION(list, level) == -1)
			return -1;

		emit(4, level - table[symbolIDX].level, table[symbolIDX].addr); // Emit STO

		return 0;
	}

	// If the current token type is a do symbol
	if (list[currToken].type == dosym)
	{
		do
		{
			currToken++; // Get next token

			if (STATEMENT(list, level) == -1)
				return -1;
		} while (list[currToken].type == semicolonsym);

		if (list[currToken].type != odsym)
		{
			if (list[currToken].type == identsym || list[currToken].type == dosym || list[currToken].type == whensym || list[currToken].type == whilesym ||
				list[currToken].type == readsym || list[currToken].type == writesym || list[currToken].type == callsym)
			{
				// Error: Statements within do-od must be separated by a semicolon
				printparseerror(15);
				return -1;
			}
			else
			{
				// Error: Do must be followed by od
				printparseerror(16);
				return -1;
			}
		}

		currToken++; // Get next token

		return 0; // Run successful
	}

	// If token type is whensym
	if (list[currToken].type == whensym)
	{
		currToken++; // Get next token

		if (CONDITION(list, level) == -1)
			return -1;

		int jpcIDX = cIndex;
		emit(8, 0, 0); // Emit JPC

		if (list[currToken].type != dosym)
		{
			// Error: if must be followed by then
			printparseerror(8);
			return -1;
		}

		currToken++; // Get next token

		if (STATEMENT(list, level) == -1)
			return -1;

		// If token type is elsedosym
		if (list[currToken].type == elsedosym)
		{
			int jmpIDX = cIndex;
			emit(7, 0, 0); // JMP
			code[jpcIDX].m = cIndex * 3;
			currToken++; // Get next token

			if (STATEMENT(list, level) == -1)
				return -1;

			code[jmpIDX].m = cIndex * 3;
		}
		else
		{
			code[jpcIDX].m = cIndex * 3;
		}

		return 0; // Run successful
	}

	// If token type is whilesym
	if (list[currToken].type == whilesym)
	{
		currToken++; // Get next token

		// Store the current code index
		int loopIDX = cIndex;

		if (CONDITION(list, level) == -1)
			return -1;

		if (list[currToken].type != dosym)
		{
			// Error: while must be followed by do
			printparseerror(9);
			return -1;
		}

		currToken++; // Get next token

		// Store the current code index
		int jpcIDX = cIndex;

		emit(8, 0, 0); // Emit JPC

		if (STATEMENT(list, level) == -1)
			return -1;

		emit(7, 0, loopIDX * 3); // Emit JMP

		code[jpcIDX].m = cIndex * 3;

		return 0;
	}

	// If token type is readsym
	if (list[currToken].type == readsym)
	{
		currToken++; // Get next token

		if (list[currToken].type != identsym)
		{
			// Error: Only variables may be assigned to or read
			printparseerror(6);
			return -1;
		}

		// Get index of symbol from FINDSYMBOL. -1 if not in symbol table.
		int symIDX = FINDSYMBOL(list[currToken].name, 2);

		if (symIDX == -1)
		{
			// Error if symbol was a constant or procedure.
			if (FINDSYMBOL(list[currToken].name, 1) != FINDSYMBOL(list[currToken].name, 3))
			{
				// Error: Only variables may be assigned to or read
				printparseerror(6);
				return -1;
			}
			else
			{
				// Error: Undeclared identifier
				printparseerror(19);
				return -1;
			}
		}

		currToken++; // Get next token

		emit(9, 0, 2); // Emit READ
		emit(4, level - table[symIDX].level, table[symIDX].addr); // Emit STO

		return 0;
	}

	// If current token is writesym
	if (list[currToken].type == writesym)
	{
		currToken++; // Get next token

		if (EXPRESSION(list, level) == -1)
			return -1;

		emit(9, 0, 1); // Emit WRITE

		return 0;
	}

	// If current token is callsym
	if (list[currToken].type == callsym)
	{
		currToken++; // Get next token

		// Get index of symbol from FINDSYMBOL. -1 if not in symbol table.
		int symIDX = FINDSYMBOL(list[currToken].name, 3);

		if (symIDX == -1)
		{
			// Error if symbol was a const or var.
			if (FINDSYMBOL(list[currToken].name, 1) != FINDSYMBOL(list[currToken].name, 2))
			{
				// Error: call must be followed by a procedure identifier
				printparseerror(7);
				return -1;
			}
			else
			{
				// Error: Undeclared identifier
				printparseerror(19);
				return -1;
			}
		}

		currToken++; // Get next token

		emit(5, level - table[symIDX].level, symIDX); // Emit CAL
	}

	return 0;
}

// CONDITION; Returns -1 if error
int CONDITION(lexeme *list, int level)
{
	// If current token type is oddsym
	if (list[currToken].type == oddsym)
	{
		currToken++; // Get next token

		if (EXPRESSION(list, level) == -1)
			return -1;

		emit(2, 0, 6); // ODD
	}
	else
	{
		if (EXPRESSION(list, level) == -1)
			return -1;

		if (list[currToken].type == eqlsym) // If eqlsym
		{
			currToken++; // Get next token

			if (EXPRESSION(list, level) == -1)
				return -1;

			emit(2, 0, 8); // EQL
		}
		else if (list[currToken].type == neqsym) // If neqsym
		{
			currToken++; // Get next token

			if (EXPRESSION(list, level) == -1)
				return -1;

			emit(2, 0, 9); // NEQ
		}
		else if (list[currToken].type == lsssym) // If lsssym
		{
			currToken++; // Get next token

			if (EXPRESSION(list, level) == -1)
				return -1;

			emit(2, 0, 10); // LSS
		}
		else if (list[currToken].type == leqsym) // If leqsym
		{
			currToken++; // Get next token

			if (EXPRESSION(list, level) == -1)
				return -1;

			emit(2, 0, 11); // LEQ
		}
		else if (list[currToken].type == gtrsym) // If gtrsym
		{
			currToken++; // Get next token

			if (EXPRESSION(list, level) == -1)
				return -1;

			emit(2, 0, 12); // GTR
		}
		else if (list[currToken].type == geqsym) // If geqsym
		{
			currToken++; // Get next token

			if (EXPRESSION(list, level) == -1)
				return -1;

			emit(2, 0, 13); // GEQ
		}
		else
		{
			// Error: Relational operator missing from condition
			printparseerror(10);
			return -1;
		}
	}

	return 0;
}

// EXPRESSION; returns -1 if error
int EXPRESSION(lexeme *list, int level)
{

	if (list[currToken].type == subsym)
	{
		currToken++; // Get next token

		if (TERM(list, level) == -1)
			return -1;

		emit(2, 0, 1); // Emit NEG

		while (list[currToken].type == addsym || list[currToken].type == subsym)
		{
			if (list[currToken].type == addsym) // If addsym
			{
				currToken++; // Get next token

				if (TERM(list, level) == -1)
					return -1;

				emit(2, 0, 2); // ADD
			}
			else // Otherwise...
			{
				currToken++; // Get next token

				if (TERM(list, level) == -1)
					return -1;

				emit(2, 0, 3); // SUB
			}
		}
	}
	else
	{
		if (list[currToken].type == addsym)
			currToken++; // Get next token

		if (TERM(list, level) == -1)
			return -1;

		while (list[currToken].type == addsym || list[currToken].type == subsym)
		{
			if (list[currToken].type == addsym)
			{
				currToken++; // Get next token

				if (TERM(list, level) == -1)
					return -1;

				emit(2, 0, 2); // ADD
			}
			else
			{
				currToken++; // Get next token

				if (TERM(list, level) == -1)
					return -1;

				emit(2, 0, 3); // SUB
			}
		}
	}
	if (list[currToken].type == lparensym || list[currToken].type == identsym ||
		list[currToken].type == numbersym || list[currToken].type == oddsym)
	{
		// Error: Bad arithmetic
		printparseerror(17);
		return -1;
	}

	return 0;
}

// TERM; returns -1 if a function it calls encounters an error.
int TERM(lexeme *list, int level)
{
	if (FACTOR(list, level) == -1)
		return -1;

	while (list[currToken].type == multsym || list[currToken].type == divsym || list[currToken].type == modsym)
	{
		if (list[currToken].type == multsym)
		{
			currToken++; // Get next token

			if (FACTOR(list, level) == -1)
				return -1;

			emit(2, 0, 4); // MUL
		}
		else if (list[currToken].type == divsym)
		{
			currToken++; // Get next token

			if (FACTOR(list, level) == -1)
				return -1;

			emit(2, 0, 5); // DIV
		}
		else
		{
			currToken++; // Get next token

			if (FACTOR(list, level) == -1)
				return -1;

			emit(2, 0, 7); // MOD
		}
	}

	// No errors
	return 0;
}

// FACTOR; returns -1 if error
int FACTOR(lexeme *list, int level)
{
	if (list[currToken].type == identsym)
	{
		int symIdx_var = FINDSYMBOL(list[currToken].name, 2);
		int symIdx_const = FINDSYMBOL(list[currToken].name, 1);

		// If findsymbol didn't find a var or const
		if (symIdx_var == -1 && symIdx_const == -1)
		{
			// If symbol is a procedure, error
			if (FINDSYMBOL(list[currToken].name, 3) != -1)
			{
				// Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables
				printparseerror(11);
				return -1;
			}
			else
			{
				// Error: Undeclared identifier
				printparseerror(19);
				return -1;
			}
		}

		if (symIdx_var == -1)
			emit(1, 0, table[symIdx_const].val); // LIT
		else if (symIdx_const == -1 || table[symIdx_var].level > table[symIdx_const].level)
			emit(3, level - table[symIdx_var].level, table[symIdx_var].addr); // LOD
		else
			emit(1, 0, table[symIdx_const].val); // LIT

		currToken++; // Get next token
	}
	else if (list[currToken].type == numbersym)
	{
		emit(1, 0, list[currToken].value); // LIT
		currToken++; // Get next token
	}
	else if (list[currToken].type == lparensym)
	{
		currToken++; // Get next token
		if (EXPRESSION(list, level) == -1)
			return -1;

		if (list[currToken].type != rparensym)
		{
			// Error: ( must be followed by )
			printparseerror(12);
			return -1;
		}

		currToken++; // Get next token
	}
	else
	{
		// Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables
		printparseerror(11);
		return -1;
	}

	return 0;
}

// Below here are functions that came with the program. Thanks, TAs!

void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: when must be followed by do\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within do-od must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: do must be followed by od\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}

	free(code);
	free(table);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}
