#include "lexheader.h"

int trigraph()
{
	/*one of two ??'s read*/
	int c;
	extern FILE *yyin;
	extern int lineno;
	c=getc(yyin);
	if(c=='?')
	{
	  c=getc(yyin);
	  switch(c)
	  {
		case '=':return '#';
		case '/':return '\\';
		case '\\':return '^';
		case '(':return '[';
		case ')':return ']';
		case '!':return '|';
		case '<':return '{';
		case '>':return '}';
		case '-':return '~';
		default:execerror("Invalid Trigraph sequence",lineno);
		ungetc(c,yyin);
		return 0;
	   }
	}
	return '\0';
}
