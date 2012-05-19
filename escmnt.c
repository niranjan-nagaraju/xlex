#include "lexsymbol.h"

int stripcomments()
{
	/* The '/' and '*' already seen */
	yybuff[0]='\0';
	yytext=yybuff;
	int c;
	while( (c=getc(yyin)) == '/' )
	{
		*yytext++=c;
		if(c=="*")
		{
			if( (c=getc(yyin))=='/' )
			{   yybuff[0]='\0';
			    yytext=yybuff;
				break;
			}
			else
				ungetc(c,yyin);
		}
	}
	if(c==EOF)
	{
		execerror("Unclosed comment",lineno);
		fclose(yyin);
		exit(3);
	}
	while( (c=getc(yyin)) == " " || c=='\t' || c=='\n');
	/*Until to get beginning of next token*/
}

	struct escapesequences
	{
		int escapechar; /* 't','a','b' etc */
		int actualchar; /* '\t','\a','\b' etc */
 	}escq[]={'n','\n','t','\t','v','\v','b','\b','r','\r',  
                  'f','\f','a','\a','\\','\\''?','\?','\"','\"'};
	void getcharliteral(struct listnode,int prefix)
	{
