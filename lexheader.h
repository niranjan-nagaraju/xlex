#include<stdio.h>
#include<stddef.h>
#include<ctype.h>


#define	YYBUFFSIZE	1000

	/*	***	GLOBAL	DECLARATIONS	*** */
	/*		------------------------	    */


	

FILE *yyin ;		/* input file pointer default is stdin*/
FILE *listfile  ;		/* output file is default stdout */

int lineno;
int inpreprocessor=0;

char	yybuff[YYBUFFSIZE];	/* TOKENS BUFFER */

char *yytext = yybuff;

char *yyprogname;		/* input file */
char *yylistfile;		/* listing file */




enum keywordtypes
{
	AUTO  =	 257,
	BREAK,CASE,CHAR,CONST,CONTINUE,DEFAULT,DO,
	DOUBLE,ELSE,ENUM,EXTERN,FLOAT,FOR,GOTO,IF,
	INT,LONG,REGISTER,RETURN,SHORT,SIGNED,SIZEOF,STATIC,
	STRUCT,SWITCH,TYPEDEF,UNION,UNSIGNED,VOID,VOLATILE,WHILE
	
};



enum prefixnsuffix
{
	   /* prefices and suffices for constants */

	L = 288,
	UL,F
};




enum constanttpes
{
	CHARCONST = 291,
	INTCONST,FLOATCONST,STRINGCONST
};


enum tokentypes
{
	KEYWORD = 295,
	CONSTANT,VARIABLE,MACRODEFINE /*Concatenation operator has to be added*/
};


enum oprtrs
{
	ARROW=300,
	PLUSPLUS,MINUSMINUS,
	LS,/*Left shift*/
	RS,/*Right shift*/
	LE,/*<=*/GE,/*>=*/EQ,/*=*/NE,/*!=*/
	AND,/*&&*/OR,/*||*/
	PLUSEQ,MINUSEQ,STAREQ,SLASHEQ,MODEQ,
	ANDEQ,/*&=*/EXOREQ,/*^=*/OREQ,/*|=*/
	LSE,/*<<=*/RSE,/*>>=*/
	ELLIPSIS /*... = 320 */
};
#define MACOPRTR 321   /*To account for macroopr ##*/
#define OPERATOR 322



struct Ckeyword
{
	char *keyname;
	int   keytype;	/* auto | break ... */
};

struct Cvariable
{
	char *varname;	/* value not known to lexer */
	int datatype;	
};


struct Cmacrodefine
{
	char *macname;
	char *macdefn;
};


struct Ccharconst
{
	int prefix;	/* L -- wchar_t */

	union
	{
		wchar_t wvalue;
		char	  cvalue; 
	}value;
};
		

struct Cintconst
{
	int suffix;	/* L -- long | UL -- unsigned long */
	
	union
	{
		long lvalue;
		unsigned long uvalue;
	}value;
};



struct Cfloatconst
{
	int suffix;	/* L -- long double | F -- float */
	
	union
	{
		long double ldvalue;
		double	dvalue;
		float		fvalue;
	}value;
};


struct Cstringconst
{
	int prefix;	/* L -- wchar_t */

	union
	{
		wchar_t *wvalue;
		char	  *cvalue;
	}value;
};




struct Cconstant
{
	int consttype;	/* char | int | float | string */

	union
	{
		struct	Cfloatconst		floatconst;
		struct	Cintconst		intconst;
		struct	Cstringconst	        stringconst;
		struct	Ccharconst		charconst;
	}constval;
};



struct Coperator
{
	int code;
};


struct tokennode
{
	int tokentype;	/* keyword | variable | macrodefine | constant */
	

	union
	{
		struct	Ckeyword		*keyword;
		struct	Cconstant		*constant;
		struct	Cvariable		*variable;
		struct	Cmacrodefine	        *macrodefine;
		struct  Coperator		*operator;
	}Ctoken;

};





struct listnode
{

	/* nodes in the linked list -- hashtable */


	struct	listnode	*next;
	struct	tokennode	token;	
};

unsigned Hashstring( char *s);
struct listnode *lookup ( char *s ); 
struct listnode* install(struct listnode* newnode);
void removenode(struct listnode *remnode,char *s);
struct listnode* lookupconst(struct Cconstant *Cptr)	;
void loadkeywords();
int lookahead(int expect[],int ifyes[],int count);
int trigraph();
int preprocessor();
void define();
void undefine();
struct listnode* getnexttoken();
void getstring(struct listnode *newnode,int prefix);
void getcharliteral(struct listnode *newnode,int prefix);
void getnumconstant(struct listnode *newnode,int c);
wchar_t* wstrdup(wchar_t *str);
struct listnode* processoperators(struct listnode *newnode,int c);
void execerror( char *str,int lineno);
int stripcomments();
int wstrcmp(wchar_t *str1,wchar_t *str2);
int wstrcpy(wchar_t *str1,wchar_t *str2);
char* getmacrodefn();
void macrofreenode(char *)	;
void macroaddnode(char *,struct listnode*);
