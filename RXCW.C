/*
   Copyright 2012 Mirko Caserta

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <bios.h>
#include <graph.h>

#define porta inp(0x3FE)
#define PUN 0
#define LIN 1
#define SI  1
#define NO  0

void cls_window(unsigned char a, unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
void cursor_pos(unsigned char x, unsigned char y);
void apre_file( void );
void new_car( int CARATTERE );
void help( void );

FILE *RX;
unsigned int riga_video = 0, beep = 0, suono = 1;
unsigned CONTA = 0;
unsigned char carattere[5];
int index, col_video = -1, control;
unsigned int frequenza;
double punto , linea, velo;

main()
{
	velo = 500;
	_setbkcolor( 0 );
	_settextcolor( 7 );
	presentazione();
	apre_file();
	inizializza_video( 0 );
    ricezione();
}

inizializza_video( unsigned int stato )
{
	int i;
	char line[80];
	_clearscreen( _GCLEARSCREEN );
	for ( i = 0 ; i <=79 ; ++i )
		line[i] = 176;
	line[80] = '\0';
	cursor_pos( 0 , 21 );
	printf(line);
	cursor_pos( 0 , 23 );
	printf(line);
	for ( i = 0 ; i <=79 ; ++i )
		line[i] = 32;
	line[80] = '\0';
	cursor_pos( 0 , 22 );
	printf(line);
	cursor_pos( 0, 24 );
	printf("RXCW 1.20 I0-5252/FR  Oscillofono:          VelocitÖ:         ? o H per l' help");
	cursor_pos( 0 , 22);
	if ( stato == 1 )
		dati();
	_settextwindow( 21, 1, 21, 80 );
	_clearscreen( _GWINDOW );
	_settextposition( 21, 13 );
	_outtext("P");
	_settextposition( 21, 40 );
	_outtext("L");
	_settextwindow( 1, 1, 20, 80 );
	_settextposition( 1, 1 );
}

dati()
{
	cursor_pos ( 35 , 24 );
	if ( suono == 1 )
		printf("%99999d Hz ",frequenza);
	else
		printf("OFF     ");
	cursor_pos ( 54 , 24 );
	printf("%99999.f   ",velo);
}

ricezione()
{
	unsigned int notpress, press, i, conta = 20, new = SI;
	int p;
	double parola;
	unsigned char line[80], col_nastro, val;
	struct rccoord oldpos;
	struct rccoord oldpos2;

	index = -1;
	frequenza = 750;
	for ( i = 0 ; i <=79 ; ++i )
		line[i] = 255;
	line[80] = 0;
	cursor_pos( 0 , 22 );
	col_nastro = 0;
	printf(line);
	dati();

inizio:
	press = notpress = 0;
	punto = velo + velo * 0.1;
	linea = punto * 3;
	linea = linea + linea * 0.1;
	parola = punto * 5;
		if ( kbhit() )
			{
			val = tasto();

				if ( val == '?' || val == 'h' || val == 'H' )
					help();
				if ( val == '+' && suono == 1 )
					{
					if ( frequenza <= 1475 )
						frequenza = frequenza + 25;
					}
				if ( val == '-' && suono == 1 )
					{
					if ( frequenza >= 125 )
						frequenza = frequenza - 25;
					}
				if ( val == '<' )
					{
					if ( velo >= 60)
						velo = velo - 10 ;
					}
				if ( val == '>' )
					{
					if ( velo <= 4990)
						velo = velo + 10 ;
					}
				if ( val == 27 )
					{
					fclose(RX);
					end_screen();
					exit();
					}
				if ( val == 's' || val == 'S' )
					{
					_setbkcolor( _getbkcolor() + 1 );
					inizializza_video( 0 );
					}
				if ( val == 'c' || val == 'C' )
					{
					if ( _gettextcolor() < 15 )
						_settextcolor( _gettextcolor() + 1 );
					else
						_settextcolor( 0 );
					inizializza_video( 0 );
					}
				if ( val == 'b' || val == 'B' )
					{
					if ( suono == 1 )
						suono = 0;
					else
						suono = 1;
					dati();
					}
				dati();
			}
	if ( porta )
	{
	if ( col_nastro == 0 )
		{
		cursor_pos( 0 , 22 );
		printf(line);
		}

	oldpos = _gettextposition();
	_settextwindow( 21, 1, 21, 80 );
	_clearscreen( _GWINDOW );
	_settextposition( 21, 13 );
	_outtext("P");
	_settextposition( 21, 40 );
	_outtext("L");

	while ( notpress <= punto * 0.2 )
	{
		if ( porta )
		{
			++press;
			Startbeep( frequenza );

			p = /* (int *) */ ((press * 40) / punto);

			if ( conta != p && p < 80 )
			{
				if ( p == 0 ) p = 1;
				_settextposition( 21, p );

				if ( p != 40 && p != 13 )
					_outtext(">");
				else
				{
					if ( p == 40 )
						_outtext("L");
					if ( p == 13 )
						_outtext("P");
				}
			}

			conta = p;
		}
		else
		{
			++notpress;
			Stopbeep();
		}
	}

	oldpos2 = _gettextposition();
	_settextposition( oldpos2.row, oldpos2.col - 1 );
	_outtext("±");
	_settextwindow( 1, 1, 20, 80 );
	_settextposition( oldpos.row, oldpos.col );

	if ( col_nastro > 70 )
		{
		col_nastro = 0;
		cursor_pos( 0 , 22 );
		printf(line);
		}
	if ( press <= punto )
		{
		cursor_pos( col_nastro , 22 );
		printf("€€");
		col_nastro = col_nastro + 4;
		carattere[++index] = PUN;
		}
	else if ( press > punto )
		{
		cursor_pos( col_nastro , 22 );
		printf("€€€€€€");
		col_nastro = col_nastro + 8;
		carattere[++index] = LIN;
		}

	while ( notpress <= linea )
	{
		if ( porta )
			goto inizio;
		else
			++notpress;
	}
	col_nastro = 0;
	trova_carattere( index );
	index = -1;
	while ( notpress <= parola )
		{
		if ( porta )
			goto inizio;
		else
			++notpress;
		}
	new_car (' ');
	}
	goto inizio;
}

trova_carattere( unsigned int maxindex )
{
	switch ( maxindex )
		{

		case 0:
			if ( carattere[0] == PUN )
				new_car ('E');
			else if ( carattere[0] == LIN )
				new_car ('T');
			break;

		case 1:
			if ( carattere[0] == PUN )
				{
				if ( carattere[1] == PUN )
					new_car ('I');
				if ( carattere[1] == LIN )
					new_car ('A');
				break;
				}
			if ( carattere[0] == LIN )
				{
				if ( carattere[1] == PUN )
					new_car ('N');
				if ( carattere[1] == LIN )
					new_car ('M');
				break;
				}

		case 2:

			if ( carattere[0] == PUN )
				{
				if ( carattere[1] == PUN )
					{
					if ( carattere[2] == PUN )
						new_car ('S');
					if (carattere[2] == LIN )
						new_car ('U');
					break;
					}
				if ( carattere[1] == LIN )
					{
					if ( carattere[2] == PUN )
						new_car ('R');
					if ( carattere[2] == LIN )
						new_car ('W');
					break;
					}
				}
			if ( carattere[0] == LIN )
				{
				if ( carattere[1] == PUN )
					{
					if ( carattere[2] == PUN )
						new_car ('D');
					if ( carattere[2] == LIN )
						new_car ('K');
					break;
					}
				if ( carattere[1] == LIN )
					{
					if ( carattere[2] == PUN )
						new_car ('G');
					if ( carattere[2] == LIN )
						new_car ('O');
					break;
					}
				}

		case 3:

			if ( carattere[0] == PUN )
				{
				if ( carattere[1] == PUN )
					{
					if ( carattere[2] == PUN )
						{
						if ( carattere[3] == PUN )
							new_car ('H');
						if ( carattere[3] == LIN )
							new_car ('V');
						break;
						}
					if ( carattere[2] == LIN )
						{
						if ( carattere[3] == PUN )
							new_car ('F');
						break;
						}
					}
				if ( carattere[1] == LIN )
					{
					if ( carattere[2] == PUN )
						{
						if ( carattere[3] == PUN )
							new_car ('L');
						break;
						}
					if ( carattere[2] == LIN )
						{
						if ( carattere[3] == PUN )
							new_car ('P');
						if ( carattere[3] == LIN )
							new_car ('J');
						break;
						}
					}

				}
			if ( carattere[0] == LIN )
				{
				if ( carattere[1] == PUN )
					{
					if ( carattere[2] == PUN )
						{
						if ( carattere[3] == PUN )
							new_car ('B');
						if ( carattere[3] == LIN )
							new_car ('X');
						break;
						}
					if ( carattere[2] == LIN )
						{
						if ( carattere[3] == PUN )
							new_car ('C');
						if ( carattere[3] == LIN )
							new_car ('Y');
						break;
						}
					}
				if ( carattere[1] == LIN )
					{
					if ( carattere[2] == PUN )
						{
						if ( carattere[3] == PUN )
							new_car ('Z');
						if ( carattere[3] == LIN )
							new_car ('Q');
						break;
						}
					}

				}
		case 4:
			cinque();
			break;
		case 5:
			sei();
			break;
		}
	}

cinque()
{
	if ( carattere[0] == PUN )
	{

		if ( carattere[1] == PUN )
		{

			if ( carattere[2] == PUN )
			{

				if ( carattere[3] == PUN )
				{

					if ( carattere[4] == PUN )
						new_car ('5');
					if ( carattere[4] == LIN )
						new_car ('4');
				}

				if ( carattere[3] == LIN )
				{

					if ( carattere[4] == LIN )
						new_car ('3');
				}

			}

			if ( carattere[2] == LIN )
			{

				if ( carattere[3] == LIN )
				{

					if ( carattere[4] == LIN )
						new_car ('2');
				}

			}

		}
		if ( carattere[1] == LIN )
		{
			if ( carattere[2] == PUN )
			{
				if ( carattere[4] == PUN )
				{
					if ( carattere[3] == PUN )
					{
						new_car ('<');
						new_car ('A');
						new_car ('S');
						new_car ('>');
					}
					if ( carattere[3] == LIN )
					{
						new_car ('<');
						new_car ('A');
						new_car ('R');
						new_car ('>');
					}
				}
			}

			if ( carattere[2] == LIN &&
			     carattere[3] == LIN &&
				 carattere[4] == LIN )  new_car ('1');
		}
	}

	if ( carattere[0] == LIN )
	{
		if ( carattere[4] == LIN &&
		     carattere[3] == LIN &&
		     carattere[2] == LIN &&
			 carattere[1] == LIN )      new_car ('0');

		if ( carattere[4] == PUN &&
		     carattere[3] == LIN &&
		     carattere[2] == LIN &&
			 carattere[1] == LIN )      new_car ('9');

		if ( carattere[4] == PUN &&
		     carattere[3] == PUN &&
		     carattere[2] == LIN &&
			 carattere[1] == LIN )      new_car ('8');

		if ( carattere[4] == PUN &&
		     carattere[3] == PUN &&
		     carattere[2] == PUN &&
			 carattere[1] == LIN )      new_car ('7');

		if ( carattere[4] == PUN &&
		     carattere[3] == PUN &&
		     carattere[2] == PUN &&
			 carattere[1] == PUN )      new_car ('6');

		if ( carattere[4] == PUN &&
		     carattere[3] == LIN &&
		     carattere[2] == LIN &&
			 carattere[1] == PUN )      new_car ('(');

		if ( carattere[1] == PUN &&
		     carattere[2] == PUN &&
		     carattere[3] == PUN &&
			 carattere[4] == LIN     )
			 {
				new_car ('<');
				new_car ('B');
				new_car ('T');
				new_car ('>');
			 }

		if ( carattere[1] == PUN &&
		     carattere[2] == PUN &&
		     carattere[3] == LIN &&
			 carattere[4] == PUN     )   new_car ('/');

	}
}

sei()
{

	if ( carattere[0] == PUN &&
	     carattere[1] == PUN &&
	     carattere[2] == PUN &&
	     carattere[3] == PUN &&
	     carattere[4] == PUN &&
		 carattere[5] == PUN )      new_car ('#');

	if ( carattere[0] == PUN &&
	     carattere[1] == LIN &&
	     carattere[2] == PUN &&
	     carattere[3] == LIN &&
	     carattere[4] == PUN &&
		 carattere[5] == LIN )      new_car ('.');

	if ( carattere[0] == LIN &&
	     carattere[1] == LIN &&
	     carattere[2] == PUN &&
	     carattere[3] == PUN &&
	     carattere[4] == LIN &&
		 carattere[5] == LIN )      new_car (',');

	if ( carattere[0] == PUN &&
	     carattere[1] == PUN &&
	     carattere[2] == LIN &&
	     carattere[3] == LIN &&
	     carattere[4] == PUN &&
		 carattere[5] == PUN )      new_car ('?');

	if ( carattere[0] == PUN &&
	     carattere[1] == LIN &&
	     carattere[2] == PUN &&
	     carattere[3] == PUN &&
	     carattere[4] == LIN &&
		 carattere[5] == PUN )
		 {
			new_car ( 39 );  /* Virgolette */
			new_car ( 39 );
		 }

	if ( carattere[0] == LIN &&
	     carattere[1] == LIN &&
	     carattere[2] == LIN &&
	     carattere[3] == PUN &&
	     carattere[4] == PUN &&
		 carattere[5] == PUN )      new_car (':');

	if ( carattere[0] == LIN &&
	     carattere[1] == PUN &&
	     carattere[2] == LIN &&
	     carattere[3] == PUN &&
	     carattere[4] == LIN &&
		 carattere[5] == PUN )      new_car (';');

	if ( carattere[0] == LIN &&
	     carattere[1] == PUN &&
	     carattere[2] == LIN &&
	     carattere[3] == LIN &&
	     carattere[4] == PUN &&
		 carattere[5] == LIN )      new_car (')');

	if ( carattere[0] == PUN &&
	     carattere[1] == PUN &&
	     carattere[2] == PUN &&
	     carattere[3] == LIN &&
	     carattere[4] == PUN &&
		 carattere[5] == LIN )
		 {
			new_car ('<');
			new_car ('S');
			new_car ('K');
			new_car ('>');
		 }

}

/*
void cls_window(unsigned char a, unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{
	union REGS inregs, outregs;

	inregs.h.ah = 6;
	inregs.h.al = 0;
	inregs.h.bh = a;
	inregs.h.ch = y1;
	inregs.h.cl = x1;
	inregs.h.dh = y2;
	inregs.h.dl = x2;

	int86(0x10, &inregs, &outregs);
}
*/

void cursor_pos(unsigned char x, unsigned char y)
{
	union REGS inregs, outregs;
	/* int 10H AH = 02H  BH=page  DH=y  DL x */
	inregs.h.ah = 2;
	inregs.h.bh = 0;
	inregs.h.dh = y;
	inregs.h.dl = x;

	int86(0x10, &inregs, &outregs);
}

Startbeep( frequenza )
{
	struct rccoord oldpos;

	if ( beep == 0 && suono == 1 )
	{
		outp( 0x43, 0xb6 );
		frequenza = (unsigned)(1193180L / frequenza);
		outp( 0x42, (char)frequenza );
		outp( 0x42, (char)(frequenza >> 8) );
		control = inp( 0x61 );
		outp( 0x61, control | 0x3 );
		beep = 1;
	}
}

Stopbeep()
{
	if ( beep == 1 )
		{
		outp( 0x61, control );
		beep = 0;
		}
}

tasto(unsigned char x)
{
	union REGS inregs, outregs;
	/* int 16H AH = 00H */
	inregs.h.ah = 0;
	inregs.h.al = x;

	int86(0x16, &inregs, &outregs);
}

void apre_file( void )
{
	char    inizio_sessione[83]="\n\n* RXCW 1.20 - Data sessione : ";
	char    tmpbuf[128];
	int     i = 0;

	_strdate( tmpbuf );
	i = tmpbuf[0]; tmpbuf[0]=tmpbuf[3]; tmpbuf[3]= i;
	i = tmpbuf[1]; tmpbuf[1]=tmpbuf[4]; tmpbuf[4]= i;
	tmpbuf[2] = '-'; tmpbuf[5] = '-';
	strcat( tmpbuf, ".LOG" );

	if( ( RX = fopen( tmpbuf, "at" )) == NULL )
	{
		_clearscreen( _GCLEARSCREEN );
		printf("Errore nell' apertura del file di log %s\n", tmpbuf );
		printf("Non posso continuare l' esecuzione del programma\n\n\n");
		exit( 1 );
	}

	_strdate( tmpbuf );
	i = tmpbuf[0]; tmpbuf[0]=tmpbuf[3]; tmpbuf[3]= i;
	i = tmpbuf[1]; tmpbuf[1]=tmpbuf[4]; tmpbuf[4]= i;
	strcat( inizio_sessione, tmpbuf );
	strcat( inizio_sessione, ", ore : " );
	_strtime( tmpbuf );
	tmpbuf[5] = '\0';
	strcat( inizio_sessione, tmpbuf );
	strcat( inizio_sessione, ". *\n\n" );
	fputs( inizio_sessione, RX);

}

void new_car( int CARATTERE )
{
	char STRINGA[2];

	STRINGA[1] = '\0';
	STRINGA[0] = CARATTERE;

	CONTA++;

	if ( CONTA > 80 )
	{
		fputc( 13, RX );
		CONTA = 1;
	}

	_outtext( STRINGA );
	fputc( CARATTERE, RX );

}

void help( void )
{
	short text_color = _gettextcolor();
	long  bk_color = _getbkcolor();

	_settextcolor( 7 );
	_setbkcolor( 0 );
	_clearscreen( _GWINDOW );
	_outtext("\n\n");
	_outtext(" + o -   Aumenta o diminuisce la frequenza dell' oscillofono\n\n");
	_outtext(" < o >   Aumenta o diminuisce la velocita'\n\n");
	_outtext(" B       Attiva / disattiva l' oscillofono\n\n");
	_outtext(" S       Cambia il colore dello sfondo\n\n");
	_outtext(" C       Cambia il colore del testo\n\n");
	_outtext(" H o ?   Mostra questo piccolo testo di aiuto\n\n");
	_outtext(" ESC     Esce dal programma\n\n");
	_settextcolor( _gettextcolor() + 16 );
	_outtext("     Premi un tasto ...");
	_settextcolor( _gettextcolor() - 16 );
	getch();
	_settextcolor( text_color );
	_setbkcolor( bk_color );
	_clearscreen( _GWINDOW );
}

