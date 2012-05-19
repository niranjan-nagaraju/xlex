#include "lexheader.h"


enum preprocessorkeywords
{
	HDEFINE=321,HUNDEF,HASHHASH,/*##*/
	HIF,HIFDEF,HIFNDEF,HELIF,HELSE,HENDIF,
	HDEFINED,HASHLINE,HERROR,HPRAGMA,HINCLUDE
};


static struct prekey
{
	char *word;
	int code;
}prekeytab[]={ "define",HDEFINE,"undef",HUNDEF,
		"if",HIF,"ifdef",HIFDEF,"ifndef",HIFNDEF,
		"elif",HELIF,"else",HELSE,"endif",HENDIF,
		"defined",HDEFINED,"line",HASHLINE,"pragma",HPRAGMA,
		"\0",0};


int preprocessor()
{
	/*Implements #define,#undef,#ifdef,#ifndef,#error,#include.....
	  and aids parser to process conditional directives */

	/* # is already seen*/
	
	int c,code,i;
	struct listnode *temp;
	inpreprocessor=1;
	/* INPROCESSOR=true; */
	temp=getnexttoken();
	switch(temp->token.tokentype)
	{
		case MACRODEFINE:strcpy(yytext,temp->token.Ctoken.macrodefine->macname);
				 free(temp->token.Ctoken.macrodefine->macname);
				 free(temp);
				 break;
		case MACOPRTR   :if(temp->token.Ctoken.operator->code=='#')
				  return HASHHASH	;
				 else
				 {	
				  execerror("Invalid Preprocessor directive\n",lineno);
					fclose(yyin);
				        exit(2)	;
				  }	
		default: execerror("Illegal Preprocessor directive\n",lineno);
					fclose(yyin);
			                exit(2);
		/*Only  one of the preprocessor keyword is accepted*/
	}
	for(i=0 ; strcmp(prekeytab[i].word,"\0") ; i++)
	{
		if(!strcmp(yytext,prekeytab[i].word))
		{	code=prekeytab[i].code;
			break;
		}
	}
	if(code==0)
	{
		execerror("Illegal preprocessor directive\n",lineno);
		fclose(yyin);
		exit(2);
	}
	else
	{
		switch(code)
		{
			case HDEFINE: define();/*Install in symbol table*/
					break;
			case HUNDEF: undefine();/*Remove entry from symbol table*/
					break;
			/*If preprocessor directive contain control pass the control to parser*/
		}
	}
	inpreprocessor=0;
	return code;
}
void define()
{/*to many mistakes*/
	struct listnode *temp,*next;
	char *macname,*macdefn;
	temp=getnexttoken();
 
	/* to get macro name*/
	if(temp->token.tokentype!=MACRODEFINE)	
	{
		execerror("Invalid macroname\n",lineno);
		exit(6);
	}
	
	/*Get macro definition i e every thing till '\n'*/
	FILE *fin=yyin;
	/*What is function of strdup next statement may be wrong & func getmacrodefn not written*/
	strcpy(temp->token.Ctoken.macrodefine->macdefn,getmacrodefn());
	/*To restore yyin to current position*/
		yyin=fin;
	/*Now start processing macrodefn & store it in macrotable for future references*/
	install(temp);
	while( (temp=getnexttoken())->token.tokentype!='\n' && temp!=NULL )
	{
		macroaddnode(macname,temp);
		/*Add to macro table*/
	}
	/*Next time macname is referenced,the scanned result from macrotable returned*/
}
void undefine()
{
	struct listnode *temp,*trav;
	temp=getnexttoken();
	if(temp->token.tokentype!=MACRODEFINE)		
	{
		execerror("Unknown Macro name or Illegal macro name\n",lineno);
		exit(6);
	}
	/*Look up for macroname*/
	if( (trav=lookup(temp->token.Ctoken.macrodefine->macname))==NULL)
	{
		execerror("Unknown Macro name or Illegal macro name\n",lineno);
		exit(6);
	}
	macrofreenode(temp->token.Ctoken.macrodefine->macname)	;
	/*clear entry from macro table*/
	removenode(trav,trav->token.Ctoken.macrodefine->macname);
	/*To remove node from symbol*/
}
	

	

