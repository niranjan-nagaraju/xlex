
/*	***	SYMBOL	TABLE		***	*/
/*		-----------------			*/



/*	
	* SYMBOL TABLE :
				Hash table with linear chaining
*/

#include "lexheader.h"
#define	HASHSIZE	158

static struct listnode  *Hashtab[HASHSIZE];
/* Hash table */

/* array of head pointers to various linked lists --
 * each linked list has nodes whose members are hashed to the same 
 * value
 */

/* Hashtab -- last index : constants */

/* constants : No basis to hash */




unsigned Hashstring( char *s)
{

	/* hash function */

	unsigned Hashval = 0;

	while( *s++ != '\0' )
		Hashval = *s + 31 * Hashval;

	
	return 	( Hashval % ( HASHSIZE - 1 ) );
}



struct listnode *lookup ( char *s )
{
	/* look for s in symbol table and return node containing it. */

	struct listnode *trav;	/* to traverse list */
	
	trav = Hashtab[Hashstring(s)];
	/* get head pointer */


while ( trav != NULL )
{
	switch( trav -> token.tokentype )
	{
		case KEYWORD :	
			if( strcmp( s,trav -> token.Ctoken.keyword->keyname) == 0 )
				return trav;	
		case MACRODEFINE :
			if( strcmp( s,trav -> token.Ctoken.macrodefine->macname) == 0 )
				return trav;	
			
		case VARIABLE : 
			if( strcmp( s,trav -> token.Ctoken.variable->varname) == 0 )
				return trav;	
		
	}	/* switch */

	
	trav = trav -> next;

}	/* while */



}	/* lookup() */











struct key
{
	char *word;
	int type;
}keytab[]	=  {
		"auto",        AUTO,
		"break",	   BREAK,
		"case",        CASE,
		"char",        CHAR,
		"const",	   CONST,
		"continue",  CONTINUE,
		"default",	    DEFAULT,
		"do",            DO,
		"double",     DOUBLE,
		"else",    	    ELSE,
		"enum",       ENUM,
		"extern",	    EXTERN,
		"float",	    FLOAT,
		"for",	    FOR,
		"goto",	    GOTO,
		"if",	    IF,
		"int",            INT,
		"long",	   LONG,
		"register",  REGISTER,
		"return",     RETURN,
		"short",	   SHORT,
		"signed",     SIGNED,
		"sizeof",	   SIZEOF,
		"static",	   STATIC,
		"struct",	   STRUCT,
		"switch",	   SWITCH,
		"typedef",   TYPEDEF,
		"union",	   UNION,
		"unsigned", UNSIGNED,
		"void",	   VOID,
		"volatile",   VOLATILE,
		"while",WHILE,

};



			
struct listnode* install(struct listnode* newnode)
{
	unsigned Hashval;
	struct listnode *trav;
	
/*Installs node pointed by newnode in Hashtable*/
if(newnode!=NULL)
{
switch(newnode->token.tokentype)
{
  case KEYWORD:     return NULL;
			
  case MACRODEFINE: if((trav=lookup(newnode->token.Ctoken.macrodefine->macname))!=NULL)
		    { 	 			/*Already defined --override*/
	            
                       macrofreenode(trav->token.Ctoken.macrodefine->macname);
                       /*To realease the entries macro table*/
			free(trav->token.Ctoken.macrodefine->macdefn);
			free(trav->token.Ctoken.macrodefine->macname);
			free(trav);
		       Hashval=Hashstring(newnode->token.Ctoken.macrodefine->macname);
                       /*To get the header ptr --index in Hashtab*/
		     }
			break;
  case VARIABLE:   if( ( trav=lookup(newnode->token.Ctoken.variable->varname) ) != NULL )	
		  {     
			Hashval=Hashstring(newnode->token.Ctoken.variable->varname);
			break;
		  }
		  else
		  {
			free(newnode->token.Ctoken.variable->varname);
			free(newnode);
			return NULL;
		   }
  case CONSTANT:  if( ( trav=lookupconst(newnode->token.Ctoken.constant) ) != NULL )	
		  {
			if(newnode->token.Ctoken.constant->consttype==STRINGCONST)
			{ if(newnode->token.Ctoken.constant->constval.stringconst.prefix==L)
			 free(newnode->token.Ctoken.constant->constval.stringconst.value.wvalue);
			  else	
		       free(newnode->token.Ctoken.constant->constval.stringconst.value.cvalue);	
			}
			free(newnode);
			return trav;
		   }
		   else
		   {
			Hashval=HASHSIZE-1;
			break;
		    }
}/*end of switch*/
  trav=Hashtab[Hashval]	;
  newnode->next=NULL;
	if(trav==NULL)/*No value is hashed to the index yet*/
	{ /*first==NULL*/
		trav=newnode;/*add to list*/
		return newnode;
	}
	
	while(trav->next==NULL)		
		trav=trav->next; /*Traversing through till end*/
	trav->next=newnode;
	return newnode;
 }/* end of if*/
	return NULL;
}

void removenode(struct listnode *remnode,char *s)
{	/*removes a node pointed by remnode from Hashtab*/
	struct listnode *trav;
	unsigned Hashval=Hashstring(s);
	trav=Hashtab[Hashval];
	if(trav==NULL)
		return; /*Not found*/
	while(trav->next!=remnode && trav->next!=NULL)
	 	trav=trav->next;
	if(trav->next==NULL)
		return; /*last node--Not found*/
	trav->next=remnode->next;
	free(remnode->token.Ctoken.macrodefine->macname);
	free(remnode->token.Ctoken.macrodefine->macdefn);
	free(remnode);
}
	


struct listnode* lookupconst(struct Cconstant *Cptr)				
{
	struct listnode *trav;
	trav=Hashtab[HASHSIZE-1];
	while(trav!=NULL)
	{
		switch(trav->token.Ctoken.constant->consttype)
		{
	case CHAR:if(trav->token.Ctoken.constant->constval.charconst.prefix==L)
		  {
       if(trav->token.Ctoken.constant->constval.charconst.value.wvalue==Cptr->constval.charconst.value.wvalue)
			return trav;
		  }
		  else
		  {
		if(trav->token.Ctoken.constant->constval.charconst.value.cvalue==Cptr->constval.charconst.value.cvalue) 
			return trav;
		  }
		break;
	case INTCONST:if(trav->token.Ctoken.constant->constval.intconst.suffix==UL)	 			
if(trav->token.Ctoken.constant->constval.intconst.value.uvalue==Cptr->constval.intconst.value.uvalue)
				return trav;
		      else /*L or none*/
if(trav->token.Ctoken.constant->constval.intconst.value.lvalue==Cptr->constval.intconst.value.lvalue)		
				return trav;
			break;
	case FLOATCONST:if(trav->token.Ctoken.constant->constval.floatconst.suffix==L)	 			
			if(trav->token.Ctoken.constant->constval.floatconst.value.ldvalue==Cptr->constval.floatconst.value.ldvalue)
				return trav;
		      else if(trav->token.Ctoken.constant->constval.floatconst.value.fvalue==Cptr->constval.floatconst.value.fvalue)		
				return trav;
		      else /* none means =>double*/
			if(trav->token.Ctoken.constant->constval.floatconst.value.dvalue==Cptr->constval.floatconst.value.dvalue)		
				return trav;
			break;

	case STRINGCONST:if(trav->token.Ctoken.constant->constval.stringconst.prefix==L)
			if(!wstrcmp(trav->token.Ctoken.constant->constval.stringconst.value.wvalue,Cptr->constval.stringconst.value.wvalue))
				return trav;
			 else /*None*/	
			  if(!strcmp(trav->token.Ctoken.constant->constval.stringconst.value.cvalue,Cptr->constval.stringconst.value.cvalue))
				return trav;
			  break;
	}/*end of switch*/
	trav=trav->next;
    }
	return NULL;
}/*End of lookupconst*/
	
  void loadkeywords()
  {
	struct listnode *trav,*keynode;
	int i;
	unsigned Hashval;
	for(i=0 ; keytab[i].word!='\0' ; i++ )
	{
	  keynode=(struct listnode*)malloc(sizeof(struct listnode));
	  keynode->token.tokentype=KEYWORD;
	  strcpy(keynode->token.Ctoken.keyword->keyname,keytab[i].word);
	  keynode->token.Ctoken.keyword->keytype=keytab[i].type;
	  keynode->next=NULL;

	Hashval=Hashstring(keytab[i].word);
	trav=Hashtab[Hashval];
	
	if(trav=NULL)
	{
		trav=keynode;
		continue;
	}
	while(trav->next==NULL)
	  trav=trav->next;
	trav->next=keynode;
	}
  }
	
	int lookahead(int expect[],int ifyes[],int count)
	{
		/*expect[]--next char expected
		  ifyes[]--code to return if expect[] matches
		  count--no of chars to expect
		  (No of valid chars in expect[])*/
	
		/*look ahead for >=,+=,++,etc*/
		int i;
		int c=getc(yyin);

		for(i=0 ; i<count ; i++)
			if(c==expect[i])
			  return ifyes[i];
		ungetc(c,yyin);
		return 0;
	}
	int wstrcmp(wchar_t *str1,wchar_t *str2)
	{
		int i;
		for(  i=0; *str1==*str2 && *str1!=0 && *str2!=0 ;str1++,str2++, i++ );
		if(i==0)
			return 1;
		else 
			return 0;
	}
		
			
			
			
