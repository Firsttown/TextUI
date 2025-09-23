#ifndef BOX_STYLE_H
#define BOX_STYLE_H

struct BoxStyle {
    char horizontal;
    char vertical;
    char top_left;
    char top_right;
    char bottom_left;
    char bottom_right;
    
    // Конструкторы для разных стилей
    BoxStyle() : 
        horizontal('-'), vertical('|'),
        top_left('+'), top_right('+'),
        bottom_left('+'), bottom_right('+') {}
    
    BoxStyle(char h, char v, char tl, char tr, char bl, char br) :
        horizontal(h), vertical(v),
        top_left(tl), top_right(tr),
        bottom_left(bl), bottom_right(br) {}
    
    // Предопределенные стили
    static BoxStyle simple() {
        return BoxStyle('-', '|', '+', '+', '+', '+');
    }
    
    static BoxStyle doubleLine() {
        return BoxStyle('═', '║', '╔', '╗', '╚', '╝');
    }
    
    static BoxStyle singleLine() {
        return BoxStyle('─', '│', '┌', '┐', '└', '┘');
    }
    
    static BoxStyle rounded() {
        return BoxStyle('─', '│', '╭', '╮', '╰', '╯');
    }
    
    static BoxStyle bold() {
        return BoxStyle('━', '┃', '┏', '┓', '┗', '┛');
    }
    
    static BoxStyle dotted() {
        return BoxStyle('╌', '╎', '┌', '┐', '└', '┘');
    }
    
    static BoxStyle dashed() {
        return BoxStyle('┄', '┆', '┌', '┐', '└', '┘');
    }
};

#endif