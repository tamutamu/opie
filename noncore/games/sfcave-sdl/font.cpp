#include "font.h"

#include "constants.h"

BFont *FontHandler :: menuSelFont;
BFont *FontHandler :: menuUnSelFont;
BFont *FontHandler :: whiteFont;
BFont *FontHandler :: colouredFont;
BFont *FontHandler :: helpFont;

void FontHandler :: init()
{
	// Load font images
	// Convert to fonts
	menuSelFont = new BFont( IMAGES_PATH "sel_menu_font.bmp" );
	menuUnSelFont = new BFont( IMAGES_PATH "unsel_menu_font.bmp" );
	whiteFont = new BFont( IMAGES_PATH "score_font.bmp" );
	helpFont = new BFont( IMAGES_PATH "help_font.bmp" );
	colouredFont = 0;
}

void FontHandler :: cleanUp()
{
	delete menuSelFont;
	delete menuUnSelFont;
	delete whiteFont;
	delete helpFont;

	if ( colouredFont )
		delete colouredFont;
}

int  FontHandler :: TextWidth( int font, const char *text )
{
	return getFont( font )->TextWidth( text );
}

int  FontHandler :: FontHeight( int font )
{
	return getFont( font )->FontHeight();
}

void FontHandler :: draw( SDL_Surface *screen, int font, const char *text, int x, int y )
{
	if ( x == -1 )
		getFont( font )->CenteredPutString( screen, y, text );
	else
		getFont( font )->PutString( screen, x, y, text );
}

void FontHandler :: changeColor( int font, int r, int g, int b )
{
	if ( colouredFont )
		delete colouredFont;

	colouredFont = getFont( font )->SetFontColor( r, g, b );
}


BFont *FontHandler :: getFont( int font )
{
	if ( font == FONT_MENU_HIGHLIGHTED )
		return menuSelFont;
	else if ( font == FONT_MENU_UNHIGHLIGHTED )
		return menuUnSelFont;
	else if ( font == FONT_COLOURED_TEXT )
		return colouredFont;
	else if ( font == FONT_HELP_FONT )
		return helpFont;
	else
		return whiteFont;
}
