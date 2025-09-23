#ifndef COLOR_H
#define COLOR_H

#include <string>

enum class Color
{
	//Стандартные цвета
	
	BLACK 	= 30,
	RED 	= 31,
	GREEN	= 32,
	YELLOW	= 33,
	BLUE	= 34,
	MAGENTA = 35,
	CYAN 	= 36,
	WHITE	= 37,
	
	//Яркие цвета
	
	BRIGHT_BLACK 	= 90,
	BRIGHT_RED 		= 91,
	BRIGHT_GREEN	= 92,
	BRIGHT_YELLOW	= 93,
	BRIGHT_BLUE		= 94,
	BRIGHT_MAGENTA 	= 95,
	BRIGHT_CYAN 	= 96,
	BRIGHT_WHITE	= 97,
	
	//Спец. значения
	
	DEFAULT = 39
};

enum class	BackgroundColor
{
	//Стандартные цвета
	
	BLACK 	= 40,
	RED 	= 41,
	GREEN	= 42,
	YELLOW	= 43,
	BLUE	= 44,
	MAGENTA = 45,
	CYAN 	= 46,
	WHITE	= 47,
	
	//Яркие цвета
	
	BRIGHT_BLACK 	= 100,
	BRIGHT_RED 		= 101,
	BRIGHT_GREEN	= 102,
	BRIGHT_YELLOW	= 103,
	BRIGHT_BLUE		= 104,
	BRIGHT_MAGENTA 	= 105,
	BRIGHT_CYAN 	= 106,
	BRIGHT_WHITE	= 107,
	
	//Спец. значения
	
	DEFAULT = 49
};


struct ColorStyle
{
	Color foreground;
	BackgroundColor background;
	bool bold;
	bool italic;
	bool underline;
	
	ColorStyle() :
		foreground(Color::DEFAULT),
		background(Color::DEFAULT),
		bold(false),
		italic(false),
		underline(false) {}
		
	ColorStyle(Color fg) :
		foreground(fg),
		background(Color::DEFAULT),
		bold(false),
		italic(false),
		underline(false) {}
		
	ColorStyle(Color fg, BackgroundColor bg) : 
        foreground(fg),
        background(bg),
        bold(false),
        italic(false),
        underline(false) {}

    ColorStyle& setForeground(Color fg) {
        foreground = fg;
        return *this;
    }
    
    ColorStyle& setBackground(BackgroundColor bg) {
        background = bg;
        return *this;
    }
    
    ColorStyle& setBold(bool value = true) {
        bold = value;
        return *this;
    }
    
    ColorStyle& setItalic(bool value = true) {
        italic = value;
        return *this;
    }
    
    ColorStyle& setUnderline(bool value = true) {
        underline = value;
        return *this;
    }
    
    // Предопределенные стили
    static ColorStyle error() {
        return ColorStyle(Color::BRIGHT_RED, BackgroundColor::DEFAULT).setBold();
    }
    
    static ColorStyle warning() {
        return ColorStyle(Color::BRIGHT_YELLOW);
    }
    
    static ColorStyle success() {
        return ColorStyle(Color::BRIGHT_GREEN);
    }
    
    static ColorStyle info() {
        return ColorStyle(Color::BRIGHT_BLUE);
    }
    
    static ColorStyle highlight() {
        return ColorStyle(Color::BRIGHT_CYAN, BackgroundColor::BLACK);
    }		
	
}
#endif