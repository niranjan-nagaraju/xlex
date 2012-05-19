#include "lexheader.h"

struct listnode* processoperators(struct listnode *newnode,int c)
{	
	int expect[10],ifyes[10],code;
	newnode->token.tokentype=OPERATOR;
	switch(c)
	{
		
		case '+' : expect[0]='+';
			   ifyes[0]=PLUSPLUS;
			   expect[1]='='	;
			   ifyes[1]=PLUSEQ;
			   code=lookahead(expect,ifyes,2);
			   if(code)
				newnode->token.Ctoken.operator->code=code;
			   else
				newnode->token.Ctoken.operator->code=c;
			   	return newnode;
		case '-' : expect[0]='-';
			   ifyes[0]=MINUSMINUS;
			   expect[1]='=';
			   ifyes[1]=MINUSEQ;
			   expect[2]='>';
			   ifyes[2]=ARROW;
			   code=lookahead(expect,ifyes,3);
			   if(code)
				newnode->token.Ctoken.operator->code=code;
			   else
				newnode->token.Ctoken.operator->code=c;
				return newnode;
		case '*' : expect[0]='=';
			   ifyes[0]=STAREQ;
			   if( (code=lookahead(expect,ifyes,1)) != 0 )
				newnode->token.Ctoken.operator->code=code;
			   else
				newnode->token.Ctoken.operator->code=c;		
				return newnode;
		case '/': newnode->token.Ctoken.operator->code=c;
				expect[0]='=';
				ifyes[0]=SLASHEQ;
				if( (code=lookahead(expect,ifyes,1)) != 0 )
				  newnode->token.Ctoken.operator->code=code;
			        return newnode;
		case '^' : newnode->token.Ctoken.operator->code=c;
				expect[0]='=';
				ifyes[0]=EXOREQ;
				if( (code=lookahead(expect,ifyes,1)) != 0 )
				  newnode->token.Ctoken.operator->code=code;
			        return newnode;	
		case '%':  newnode->token.Ctoken.operator->code=c;
				expect[0]='=';
				ifyes[0]=MODEQ;
				if( (code=lookahead(expect,ifyes,1)) != 0 )
				  newnode->token.Ctoken.operator->code=code;
			        return newnode;
		case '=':  newnode->token.Ctoken.operator->code=c;
				expect[0]='=';
				ifyes[0]=EQ;
				if( (code=lookahead(expect,ifyes,1)) != 0 )
				  newnode->token.Ctoken.operator->code=code;
			        return newnode;
		case '!':  newnode->token.Ctoken.operator->code=c;
				expect[0]='=';
				ifyes[0]=NE;
				if( (code=lookahead(expect,ifyes,1)) != 0 )
				  newnode->token.Ctoken.operator->code=code;
			        return newnode;
		case '|':  newnode->token.Ctoken.operator->code=c;
				expect[0]='|';
				ifyes[0]=OR;
				expect[1]='=';
				ifyes[1]=OREQ;
				if( (code=lookahead(expect,ifyes,2)) != 0 )
				  newnode->token.Ctoken.operator->code=code;
			        return newnode;
		case '&':  newnode->token.Ctoken.operator->code=c;
				expect[0]='&';
				ifyes[0]=AND;
				expect[1]='=';
				ifyes[1]=ANDEQ;
				if( (code=lookahead(expect,ifyes,2)) != 0 )
				  newnode->token.Ctoken.operator->code=code;
			        return newnode;
		case '<':  newnode->token.Ctoken.operator->code=c;
				expect[0]='=';
				ifyes[0]=LE;
				expect[1]='<';
				ifyes[1]=LS;
				if( (code=lookahead(expect,ifyes,2)) != 0 )
				  newnode->token.Ctoken.operator->code=code;
				 	if(code==LS)
					{	expect[0]='=';
						ifyes[0]=LSE;
					  if( (code=lookahead(expect,ifyes,1))!=0 )
						newnode->token.Ctoken.operator->code=code;
					}
			        return newnode;
		case '>':  newnode->token.Ctoken.operator->code=c;
				expect[0]='=';
				ifyes[0]=GE;
				expect[1]='>';
				ifyes[1]=RS;
				if( (code=lookahead(expect,ifyes,2)) != 0 )
				  newnode->token.Ctoken.operator->code=code;
				 	if(code==RS)
					{	expect[0]='=';
						ifyes[0]=RSE;
					  if( (code=lookahead(expect,ifyes,1))!=0 )
						newnode->token.Ctoken.operator->code=code;
					}
			        return newnode;
		case '?' : newnode->token.Ctoken.operator->code=c;
			   	if( (code=trigraph()) != 0 )
					return processoperators(newnode,c); /*recursion*/
			 /*else return '?' to stream*/
				ungetc(c,yyin);
				return newnode;
		default: newnode->token.Ctoken.operator->code=c;
			return newnode;
	}/*end of switch*/
}/*End of process operators*/
