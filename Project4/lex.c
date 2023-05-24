/* 
	This is the lex.c file for the UCF Fall 2021 Systems Software Project.
	For HW2, you must implement the function lexeme *lexanalyzer(char *input).
	You may add as many constants, global variables, and support functions
	as you desire.

	If you choose to alter the printing functions or delete list or lex_index, 
	you MUST make a note of that in you readme file, otherwise you will lose 
	5 points.
*/

// COP3402 - Fall 2021
// Group Members: Arthur Upfield and Henry Graves
// HW2 lex

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

lexeme *list;
int lex_index;

void printlexerror(int type);
void printtokens();

lexeme *lexanalyzer(char *input, int printFlag)
{
	int i, j, flag = 1;
	lex_index = 0;

	// Malloc an array of lexemes.
	list = (lexeme *)malloc(sizeof(lexeme) * MAX_NUMBER_TOKENS);;

	// Sorts through every character, finding appropriate tokens and symbols.
	for (i = 0; input[i] != '\0'; i++)
	{

		if (isspace(input[i]) || iscntrl(input[i]))
		{
			// Ignore whitespace
		}
		else if (isalpha(input[i]))
		{
			// Identify reserved words and add them to the list of lexemes.

			// Reserved word: const
			if (input[i] == 'c' && input[i+1] == 'o' && input[i+2] == 'n' && 
				input[i+3] == 's' && input[i+4] == 't' && !isalpha(input[i+5]) && !isdigit(input[i+5]))
			{
				list[lex_index].type = constsym;
				lex_index++;
				i = i + 4;
			}

			// Reserved word: var
			else if (input[i] == 'v' && input[i+1] == 'a' && input[i+2] == 'r' && !isalpha(input[i+3]) && !isdigit(input[i+3]))
			{
				list[lex_index].type = varsym;
				lex_index++;
				i = i + 2;
			}

			// Reserved word: procedure
			else if (input[i+0] == 'p' && input[i+1] == 'r' && input[i+2] == 'o' && 
					 input[i+3] == 'c' && input[i+4] == 'e' && input[i+5] == 'd' && 
					 input[i+6] == 'u' && input[i+7] == 'r' && input[i+8] == 'e' && !isalpha(input[i+9]) && !isdigit(input[i+9]))
			{
				list[lex_index].type = procsym;
				lex_index++;
				i = i + 8;
			}

			// Reserved word: call
			else if (input[i+0] == 'c' && input[i+1] == 'a' && input[i+2] == 'l' && 
					 input[i+3] == 'l' && !isalpha(input[i+4]) && !isdigit(input[i+4]))
			{
				list[lex_index].type = callsym;
				lex_index++;
				i = i + 3;
			}

			// Reserved word: when
			else if (input[i+0] == 'w' && input[i+1] == 'h' && input[i+2] == 'e' && input[i+3] == 'n' && !isalpha(input[i+4]) && !isdigit(input[i+4]))
			{
				list[lex_index].type = whensym;
				lex_index++;
				i = i + 3;
			}

			// Reserved word: elsedo
			else if (input[i+0] == 'e' && input[i+1] == 'l' && input[i+2] == 's' && 
					 input[i+3] == 'e' && input[i+4] == 'd' && input[i+5] == 'o' && !isalpha(input[i+6]) && !isdigit(input[i+6]))
			{
				list[lex_index].type = elsedosym;
				lex_index++;
				i = i + 5;
			}

			// Reserved word: while
			else if (input[i+0] == 'w' && input[i+1] == 'h' && input[i+2] == 'i' && 
					 input[i+3] == 'l' && input[i+4] == 'e' && !isalpha(input[i+5]) && !isdigit(input[i+5]))
			{
				list[lex_index].type = whilesym;
				lex_index++;
				i = i + 4;
			}

			// Reserved word: do
			else if (input[i+0] == 'd' && input[i+1] == 'o' && !isalpha(input[i+2]) && !isdigit(input[i+2]))
			{
				list[lex_index].type = dosym;
				lex_index++;
				i = i + 1;
			}

			// Reserved word: od
			else if (input[i+0] == 'o' && input[i+1] == 'd' && !isalpha(input[i+2]) && !isdigit(input[i+2]))
			{
				list[lex_index].type = odsym;
				lex_index++;
				i = i + 1;
			}

			// Reserved word: read
			else if (input[i+0] == 'r' && input[i+1] == 'e' && input[i+2] == 'a' && 
					 input[i+3] == 'd' && !isalpha(input[i+4]) && !isdigit(input[i+4]))
			{
				list[lex_index].type = readsym;
				lex_index++;
				i = i + 3;
			}

			// Reserved word: write
			else if (input[i+0] == 'w' && input[i+1] == 'r' && input[i+2] == 'i' && 
					 input[i+3] == 't' && input[i+4] == 'e' && !isalpha(input[i+5]) && !isdigit(input[i+5]))
			{
				list[lex_index].type = writesym;
				lex_index++;
				i = i + 4;
			}

			// Reserved word: odd
			else if (input[i+0] == 'o' && input[i+1] == 'd' && input[i+2] == 'd' && !isalpha(input[i+3]) && !isdigit(input[i+3]))
			{
				list[lex_index].type = oddsym;
				lex_index++;
				i = i + 2;
			}

			// Process an identifier and add it to the lexeme list.
			else
			{
				list[lex_index].type = identsym;

				// Find and store all valid characters of the identifier name.
				for (j = 0; j < MAX_IDENT_LEN; j++)
				{
					if (isalpha(input[i + j]) || isdigit(input[i + j]))
					{
						list[lex_index].name[j] = input[i + j];
					}
					else
					{
						break;
					}
				}

				// If the next character of input after the identifier is alphanumerical, excessive identifier length error.
				if (isalnum(input[i + j]))
				{
					printlexerror(4);
					return NULL;
				}

				lex_index++;

				// Move input index counter forward by the amount of characters in the identifier.
				i = i + (j - 1);
			}
		}

		// Process a number and add it to the lexeme list.
		else if (isdigit(input[i]))
		{
			// Convert the number from a character to an integer using an ascii conversion.
			list[lex_index].value = input[i] - '0';
			list[lex_index].type = numbersym;

			// Starts at 1 since line above fills 0th index
			for (j = 1; j < MAX_NUMBER_LEN; j++)
			{
				if (isdigit(input[i + j]))
				{
					// Increase the current value by a factor of 10, then add the new digit to the ones place.
					list[lex_index].value = (list[lex_index].value * 10) + (input[i + j] - '0');
				}

				// If the next character of input for the number is alphabetical, invalid identifier error.
				else if (isalpha(input[i + j]))
				{
					printlexerror(2);
					return NULL;
				}
				else
				{
					break;
				}
			}

			// If the next character of input after the number is a digit, excessive number length error.
			if (isdigit(input[i + j]))
			{
				printlexerror(3);
				return NULL;
			}

			lex_index++;

			// Move input index counter forward by the amount of characters in the number.
			i = i + (j - 1);
		}

		// Identify special symbols and add them to the list of lexemes.

		// Special symbol: =
		else if (input[i] == '=')
		{
			list[lex_index].type = eqlsym;
			lex_index++;
			i++;
		}

		// Special symbol: !=
		else if (input[i] == '!')
		{
			if (input[i + 1] == '=')
			{
				list[lex_index].type = neqsym;
				lex_index++;
				i++;
			}

			// If the next character of input after ! is not =, invalid symbol error.
			else
			{
				printlexerror(1);
				return NULL;
			}
		}

		// Special symbols: <= and <
		else if (input[i] == '<')
		{
			if (input[i + 1] == '=') // <=
			{
				list[lex_index].type = leqsym;
				lex_index++;
				i++;
			}
			else // <
			{
				list[lex_index].type = lsssym;
				lex_index++;
			}
		}

		// Special symbols: >= and >
		else if (input[i] == '>')
		{
			if (input[i + 1] == '=') // >=
			{
				list[lex_index].type = geqsym;
				lex_index++;
				i++;
			}
			else // >
			{
				list[lex_index].type = gtrsym;
				lex_index++;
			}
		}

		// Special symbol: %
		else if (input[i] == '%')
		{
			list[lex_index].type = modsym;
			lex_index++;
		}

		// Special symbol: *
		else if (input[i] == '*')
		{
			list[lex_index].type = multsym;
			lex_index++;
		}

		// Special symbol: /
		// Identify comments: //
		else if (input[i] == '/')
		{
			// / = divsym, // = comment
			if (!(input[i + 1] == '/'))
			{
				list[lex_index].type = divsym;
				lex_index++;
			}

			// Comment found. Loop until \n or \0
			else for (i; !(input[i] == '\n' || input[i] == '\0'); i++)
			{
				// Ignore content within comment
			}
		}

		// Special symbol: +
		else if (input[i] == '+')
		{
			list[lex_index].type = addsym;
			lex_index++;
		}

		// Special symbol: -
		else if (input[i] == '-')
		{
			list[lex_index].type = subsym;
			lex_index++;
		}

		// Special symbol: (
		else if (input[i] == '(')
		{
			list[lex_index].type = lparensym;
			lex_index++;
		}

		// Special symbol: )
		else if (input[i] == ')')
		{
			list[lex_index].type = rparensym;
			lex_index++;
		}

		// Special symbol: ,
		else if (input[i] == ',')
		{
			list[lex_index].type = commasym;
			lex_index++;
		}

		// Special symbol: .
		else if (input[i] == '.')
		{
			list[lex_index].type = periodsym;
			lex_index++;
		}

		// Special symbol: ;
		else if (input[i] == ';')
		{
			list[lex_index].type = semicolonsym;
			lex_index++;
		}

		// Special symbol: :=
		else if (input[i] == ':')
		{
			if (input[i + 1] == '=')
			{
				list[lex_index].type = assignsym;
				lex_index++;
				i++;
			}

			// If the next character of input after : is not =, invalid symbol error.
			else
			{
				printlexerror(1);
				return NULL;
			}
		}

		// If current character doesn't match any of the previous conditions, invalid symbol error.
		else
		{
			printlexerror(1);
			return NULL;
		}
	}

	// Call printtokens() with the completed lexeme table.
	if (printFlag)
		printtokens();

	// Return the completed lexeme list.
	return list;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "=", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case odsym:
				printf("%11s\t%d", "od", odsym);
				break;
			case whensym:
				printf("%11s\t%d", "when", whensym);
				break;
			case elsedosym:
				printf("%11s\t%d", "elsedo", elsedosym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	return;
}