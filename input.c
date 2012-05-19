	#include<stdio.h>
	#include<stdlib.h>
	#include<fcntl.h>
	#include<string.h>
	#include<sys/file.h>
	#include<syscall.h>


	/*------------------------------------------------------------------------
		* INPUT.C: The input system used by lexical analysers
	*------------------------------------------------------------------------
	*System dependent defines
	*/

	#ifdef  MSDOS
	  #define COPY(d,s,a)   memmove(d,s,a)
	#else
	  #define COPY(d,s,a)	memcpy(d,s,a)
	#endif

	#define STDIN 0	/*Standarad input*/

	#define MAXLOOK  16	/* Maximum amount of lookahead */
	#define MAXLEX	1024	/* Maximum lexemes sizes */

	#define BUFFSIZE ( (MAXLEX*3) + ( 2*MAXLOOK ) )

	#define DANGER ( End_buf  -  MAXLOOK )    /*Flushing the buffer when Next
															  passes this address */
	#define END &Start_buf[BUFFSIZE]				/* Just past last char in buf */
	
	#define NO_MORE_CHARS()	( Eof_read && Next >= End_buf )

	typedef unsigned char uchar;

	static uchar	Start_buf[BUFFSIZE]; /*Input Buffer */

	static uchar	*End_buf = END ; /* Just Past last charcter */

	static uchar	*Next = END ;	/* Next input character */

	static uchar	*sMark = END ;	/*	Start of current lexeme */

	static uchar	*eMark	= END ;	/* End of current lexeme */

	static uchar	*pMark = NULL ; /* Start of Previous lexeme */

	static	int plineno = 0;	/* Line # of Previous lexeme */

	static	int plength = 0;    /* Length of Previous lexeme */

	static	int Inp_file = STDIN ; /* Input file handle */

	static	int lineno = 1 ;    /* Line # of Current lexeme */

	static	int Mline = 1 ;	/* Line # when mark_end is called */

	static	int Termchar = 0;	/* Holds the character that was that was overwritten 
								 
								   by a \0 when we null terminated the last lexeme*/

	static int Eof_read = 0; /* End of file has been read */

	extern int open(),close(),read() ;


	static int (*openp)() = open;

	static int (*closep)() = close;

	static int (*readp)() = read;




	void ii_io (int (*open_func)(), int (*close_func)(),int (*read_func)() )
	{
		openp = open_func ;

		closep= close_func ;

		readp= read_func ;

	}

	int ii_newfile( char *name )
	{
		int fd;

		if( ( fd = !name ? STDIN : (*openp)(name,O_RDONLY)  ) != -1 )
		{
			if(Inp_file != STDIN)
				(*closep)(Inp_file);
			Inp_file = fd;
			
			Eof_read = 0;


			Next = END;

			sMark = END;

			eMark = END;

			End_buf = END;

			lineno = 1;

			Mline = 1;

		}

		return fd;

	}

	 char* ii_text()	{ return sMark ;}

	 int  ii_length()	{ return ( eMark - sMark ) ;}

	 int ii_lineno()	{ return lineno ;}

	 char* ii_ptext()	{ return pMark ;}

	 int  ii_plength()	{ return plength ;}

	 int ii_plineno()	{ return plineno ;}

	char * ii_mark_start()
	{
		Mline=lineno;

		eMark=sMark=Next;

		return sMark ;

	}

	char* ii_mark_end()
	{
		Mline =lineno ;

		return ( eMark=Next );

	}

	char* ii_move_start()
	{
		if (sMark >= eMark )
			return NULL;
		else
			return ++sMark ;
	}

	char* ii_to_mark()
	{
		lineno=Mline;
		
		return (Next = eMark);
	}

	char* ii_mark_prev()
	{
		pMark=sMark;

		plineno=lineno;

		plength = eMark-sMark;

		return (pMark);

	}

	int ii_advance()
	{
		

		if( *Next == '\\' )
			Next++;

		if( NO_MORE_CHARS() )
			return 0;

		if(!Eof_read && ii_flush(0) < 0)
			return -1;

		if( *Next =='\n' )
			lineno++;
		
		return( *Next++ );

	}
	
	uchar* min(uchar* a,uchar* b)
	{
		if(a<b)
		 return a;
		else
		 return b;
	}

	int ii_flush(int force)
	{
		int copy_amt,shift_amt ;

		uchar *left_edge;

		if( NO_MORE_CHARS() )
		
			return 0;

		if(Eof_read)   /* Nothing to read */

			return 1;

		if( Next >= DANGER || force)
		{
			left_edge = pMark ? min(sMark,pMark) : sMark;

			shift_amt = left_edge -Start_buf ;

			if( shift_amt < MAXLEX )	/* if(not enough room) */
			{
				if(!force)
					return -1 ;

				left_edge = ii_mark_start(); /*Reset the start to current character*/

				ii_mark_prev(); /* Destroying the current & previous lexeme */

				shift_amt = left_edge - Start_buf ;
			
			}

			copy_amt = End_buf - left_edge ;

			COPY(Start_buf,left_edge,copy_amt);

			if(!ii_fillbuf( Start_buf + copy_amt ))
			
				ferr("INTERNAL ERROR,ii_flush: Buffer full, can't read");

			if(pMark)
				pMark-=shift_amt;

			
			sMark-=shift_amt;

			eMark-=shift_amt;

			Next-=shift_amt;

		}

		return 1;
	}


	int ii_fillbuf( uchar* starting_at )
	{
		register unsigned need,		/*No of bytes required from input */
			               got;		/*no of bytes actually read */

		need = (( END - starting_at )/ MAXLEX ) * MAXLEX ;

		if(need <0)
		
			ferr("INTERNAL ERROR (ii_fillbuf) : Bad starting address\n");

		if(!need)
			return 0;

		if( (got = (*readp)(Inp_file,starting_at,need) ) == -1 )
			ferr("Can't read from input file\n");

		End_buf = starting_at + got ;

		if( got < need )

			Eof_read = 1; /*At the End of file */

		return got;

	}


	int ii_look(int n)
	{
		uchar *p;

		p = Next + (n-1) ;

		if( Eof_read && p >= End_buf )
			return EOF ;

		return  (( p<Start_buf || p>= End_buf ) ? 0 : *p );

	}

	int ii_pushback(int n)
	{

		while( --n >= 0 && Next > sMark )
		{
			if( *--Next == '\n' || !*Next )

				--lineno;
		}

		if( Next < eMark ) 
		{
			eMark = Next ;

			Mline =lineno ;

		}

		return ( Next > sMark );

	}


	void ii_term()
	{
		Termchar = *Next ;

		*Next = '\0' ;

	}






		














