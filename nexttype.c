#include "lexheader.h"

/*Function prototype*/




struct listnode* getnexttoken()
{
	FILE *fin;
	int c;
	yybuff[0]='\0';
	yytext=yybuff; /*Flush buffer*/
	struct listnode *temp,*newnode;
	int alphacount=0,maxlen=255,varlen=0;
	while( (c=getc(yyin))==' ' || c=='\t' ); /*Ignore white spaces*/	
	if(c=='/')
	{
		if( (c=getc(yyin)) == '*' )
			stripcomments();
		else
			ungetc(c,yyin);
	}
	newnode=(struct listnode*)malloc(sizeof(struct listnode));
	if(isalpha(c) || c=='_' )
	{
	  if(tolower(c)=='l')
		if( (c=getc(yyin))=='"')
		{
			getstring(newnode,L)	;/*prefix*/
			return newnode;
		}
		else if(c=='\'')
		{
			getcharliteral(newnode,L);
			return newnode;
		}
		ungetc(c,yyin);
		do
		{
			varlen++;
			if(isalpha(c))
			  alphacount++;
			if(varlen>=maxlen)
			{
				execerror("Identifier is too long\n",lineno);
				*yytext='\0';/*Truncate*/
				break;
			}
			*yytext++=c;
		}while( (c=getc(yyin))!=EOF && isalnum(c)||c=='_');
		if(alphacount==0)
		{	
				execerror("Invalid identifier \n",lineno);
				fclose(yyin);
				exit(4);
		}
		ungetc(c,yyin);
		*yytext='\0';/*close string*/
		if( (temp=lookup(yybuff)) != NULL )
		{
			return temp;/*found*/
		}
		if(inpreprocessor)
		{
			newnode->token.tokentype=MACRODEFINE;
			strcpy(newnode->token.Ctoken.macrodefine->macname,yytext);
			return newnode;
		}
		/*else variable*/
		newnode->token.tokentype=VARIABLE;
		strcpy(newnode->token.Ctoken.variable->varname,yytext);
		
		return (install(newnode)); /*Install variable in symbol table & return */
		/*There may error because if the variable is declared secondtime*/
	}/*end if(isalpha(c))*/
	else if(c=='.'||isdigit(c))
	{
		getnumconstant(newnode,c);
		if(newnode!=NULL)
			return newnode; /*return numeric const*/
	}
	if(c=='.')
	{
		int expect[]={'.'};
		int ifyes[]={ELLIPSIS};
		lookahead(expect,ifyes,1);
		if(ifyes[0]==ELLIPSIS)
		{
			lookahead(expect,ifyes,1);
			if(ifyes[0]==ELLIPSIS)
			{
				newnode->token.tokentype=OPERATOR;
				newnode->token.Ctoken.operator->code=ELLIPSIS;
				return newnode;
			}
			else
			{ /* .. --Invalid token */
				ungetc(c,yyin);
				execerror("Invalid token\n",lineno);
				fclose(yyin);
				exit(5);
			}
		}
		else
		{
			newnode->token.tokentype=OPERATOR;
			newnode->token.Ctoken.operator->code='.';
			return newnode;
		}
	}/*end of if(c=='.')*/
	if(c=='"')
	{
	 	getstring(newnode,0)	;/*NOT LONG*/
		return newnode;
	}
	if(c=='\'')
	{	
		getcharliteral(newnode,0);
		return newnode;
	}
	if(c=='\\')
	{
		if(inpreprocessor)
		{
			c=getc(yyin);
			while( (c=getc(yyin)) == ' ' || c == '\t' );
			/*till next token*/
		}
		else
		{
			ungetc(c,yyin);
			execerror("Stray !!",lineno);
		}
		while((c=getc(yyin)) ==  ' ' || c == '\t' );
		/*till next token */
	}
	if( c == EOF )
	  return 0;
	return processoperators(newnode,c);

}/* end of getnext token */
	


void getstring(struct listnode *newnode,int prefix)
{
	/* " is already seen */
	int c;
	wchar_t *wytext;
	while( (c=getc(yyin)) != '\"')
	{
		if(c=='\n')
		{
			execerror("Unclosed quotes !",lineno);
			exit(8);
		}
	   	*yytext++=c;
		if(prefix==L)
		  *wytext++=c;
	}
	*yytext='\0';/*close the string by overwriting the last seen " */
	yytext=yybuff;	/*restore the base address*/
	newnode->token.tokentype=CONSTANT;
	newnode->token.Ctoken.constant->consttype=STRINGCONST;
	newnode->token.Ctoken.constant->constval.stringconst.prefix=prefix;
	

	if(prefix==L)
	{
		wstrcpy(newnode->token.Ctoken.constant->constval.stringconst.value.wvalue,wytext);
		/*strdup copies yytext to heap & returns the pointer to that location*/
		/*Returns wchar_t * */
	}
	else
		strcpy(newnode->token.Ctoken.constant->constval.stringconst.value.cvalue,yytext);
	
	if( (lookupconst(newnode->token.Ctoken.constant)) != NULL )
		install(newnode); /*Installs in symbol table if not found */

}


wchar_t* wstrdup(wchar_t *str)
{
		int c,i;
		str=(wchar_t*)malloc(sizeof(wchar_t)*strlen(yytext));
		if(str==NULL)
		{
			execerror("Out of memory!\n",lineno);
			exit(9);
		}
		for(i=0;yytext[i];i++)
			str[i]=yytext[i];

		str[i]='\0';
		return str;
}
int wstrcpy(wchar_t *str1,wchar_t *str2)
{
	register int i;
	for( i=0 ; *str1++=*str2++ ; i++ ) ;
	return i;
}
	
struct escapesequences
{
		int escapechar; /* 't','a','b' etc */
		int actualchar; /* '\t','\a','\b' etc */
 }escq[]={'n','\n','t','\t','v','\v','b','\b','r','\r',  
                  'f','\f','a','\a','\\','\\','?','\?','\"','\"','\0','0'}; /*Null character is to indicate the end*/
	

void getcharliteral(struct listnode* newnode,int prefix)
{
	int c,i;
	int expect[2],ifyes[2]	;
	/* ' is already seen */
	newnode->token.tokentype=CONSTANT;
	c=getc(yyin);
	if(c==EOF)
	{
		newnode=NULL;
		ungetc(c,yyin);
		return;
	}
	if(c!='\\')
	{
		newnode->token.Ctoken.constant->consttype=CHARCONST;
		newnode->token.Ctoken.constant->constval.charconst.prefix=prefix;
		if(prefix==L)
			newnode->token.Ctoken.constant->constval.charconst.value.wvalue=c;
		else
			newnode->token.Ctoken.constant->constval.charconst.value.cvalue=c;
		goto finish;
	}
	else  /*Given chars may be escape sequences*/
	{
	for(i=0;escq[i].escapechar;i++)
	{
			expect[0]=escq[i].escapechar;
			ifyes[0]=escq[i].actualchar;
	if(lookahead(expect,ifyes,1)!=0)
	{	
		newnode->token.Ctoken.constant->consttype=CHARCONST;
		newnode->token.Ctoken.constant->constval.charconst.prefix=prefix;
		if(prefix==L)
			newnode->token.Ctoken.constant->constval.charconst.value.wvalue=ifyes[0];
		else
			newnode->token.Ctoken.constant->constval.charconst.value.cvalue=ifyes[0];
		goto finish;
	}
	}/*end of for loop*/
	/*else other chars --\m = m */
	c=getc(yyin);
	/*if lookahead fails --ptr would have been @ '\\' */
	if(isalpha(c) && (c!='X' || c!='x'))
	{
		newnode->token.Ctoken.constant->consttype=CHARCONST;
		newnode->token.Ctoken.constant->constval.charconst.prefix=prefix;
		if(prefix==L)
			newnode->token.Ctoken.constant->constval.charconst.value.wvalue=c;
		else
			newnode->token.Ctoken.constant->constval.charconst.value.cvalue=c;
		goto finish;
	}
		
	if(c== 'X' || c=='x')
	{ /*Hexadecimal character*/
		newnode->token.Ctoken.constant->consttype=INTCONST;
		newnode->token.Ctoken.constant->constval.intconst.suffix=L;/* long const*/
		fscanf(yyin,"%x",&newnode->token.Ctoken.constant->constval.intconst.value.lvalue);
		goto finish;
	}
	else
	{
		/*octal -- '\0','\12' etc */
		newnode->token.Ctoken.constant->consttype=INTCONST;
		newnode->token.Ctoken.constant->constval.intconst.suffix=L;/* long const*/
		fscanf(yyin,"%o",&newnode->token.Ctoken.constant->constval.intconst.value.lvalue);
		goto finish;
	}
     }/* if('\\')------	end*/
	finish:
		if(!lookupconst(newnode->token.Ctoken.constant))
		   install(newnode);
}/*end of getcharliteral() */

void getnumconstant(struct listnode *newnode,int c)
{
	int digits=0;
	newnode->token.tokentype=CONSTANT;
	if(c=='.')
	{
		newnode->token.Ctoken.constant->consttype=FLOATCONST;
		*yytext++ = c;
		c= getc(yyin);
	}
	while(isdigit(c))
	{
		digits++;
		*yytext++=c;
		c=getc(yyin);
	}
	*yytext='\0';
	yytext=yybuff;
	if(digits==0)
	{
		ungetc(c,yyin);
		yybuff[0]='\0';
		yytext=yybuff;
		newnode=NULL;
		return ;
	}
	switch(tolower(c))
	{
	case 'l':if(newnode->token.Ctoken.constant->consttype==FLOATCONST)
		{
			newnode->token.Ctoken.constant->constval.floatconst.suffix=L;	
			/* In case of long double*/
  sscanf(yytext,"%lf",&newnode->token.Ctoken.constant->constval.floatconst.value.ldvalue);
			return;
		}
		else
		{
			newnode->token.Ctoken.constant->consttype=INTCONST;	
                        newnode->token.Ctoken.constant->constval.intconst.suffix=L;	
			/* In case of long */
  sscanf(yytext,"%lf",&newnode->token.Ctoken.constant->constval.intconst.value.lvalue);
			return;
		}
	case 'f':if(newnode->token.Ctoken.constant->consttype!=FLOATCONST)
		{
			execerror("Illegal modifier !",lineno);
			newnode=NULL;
			return;
		}
		else
		{
                        newnode->token.Ctoken.constant->constval.floatconst.suffix=F;	
  sscanf(yytext,"%f",&newnode->token.Ctoken.constant->constval.floatconst.value.fvalue);
			return;
		}
	case 'u':if(newnode->token.Ctoken.constant->consttype==FLOATCONST)
		{
			execerror("Illegal modifier !",lineno);
			newnode=NULL;
			return;
		}
		else
		{
			newnode->token.Ctoken.constant->consttype=INTCONST;	
			c=getc(yyin);
			if(tolower(c)=='l')
                        newnode->token.Ctoken.constant->constval.intconst.suffix=UL;
			else
				ungetc(c,yyin);
  sscanf(yytext,"%u",&newnode->token.Ctoken.constant->constval.intconst.value.uvalue);
			return;
		}		
	default: if(newnode->token.Ctoken.constant->consttype==FLOATCONST)
		{
			newnode->token.Ctoken.constant->constval.floatconst.suffix=0;/*double*/	
  sscanf(yytext,"%lf",&newnode->token.Ctoken.constant->constval.floatconst.value.dvalue);
			return;
		}
		else
		{
			newnode->token.Ctoken.constant->consttype=INTCONST;	
                        newnode->token.Ctoken.constant->constval.intconst.suffix=L;	
			/* In case of long */
  sscanf(yytext,"%ld",&newnode->token.Ctoken.constant->constval.intconst.value.lvalue);
			return;
		}			
		newnode=NULL;
		return;
	}/* End of switch */
}/* end of getnumconstant() */
		

	
	
	
			



			
			
	
	
	
	
