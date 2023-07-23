#include"screen.h"

static HANDLE  cnl_out() {
     return  ::GetStdHandle( STD_OUTPUT_HANDLE );

}
void srn::showcursor( bool show ) {
       CONSOLE_CURSOR_INFO  cci;
       ::GetConsoleCursorInfo( cnl_out(), &cci );
      cci.dwSize = 25;
       cci.bVisible = show;
       ::SetConsoleCursorInfo( cnl_out(), &cci );
};

void  srn::settitle(char const* const pch) {
       ::SetConsoleTitleA( pch );
}
void srn::gotoxy( short x, short y ) {
       COORD dwCursorPosition = { x,y };
       SetConsoleCursorPosition( cnl_out(), dwCursorPosition );
}
void srn::puts( char const *const pch ) {
       DWORD w=0;
       ::WriteConsoleA( cnl_out(), pch, (int)strlen( pch ), &w, &w );
}
void srn::puts( wchar_t const *const pch ) {
       DWORD w = 0;
       ::WriteConsoleW( cnl_out(), pch, (int)wcslen( pch ), &w, &w );
}
void srn::putchar( wchar_t wch ) {
       DWORD w = 0;
       ::WriteConsoleW( cnl_out(), &wch,1, &w, &w );
}
void srn::putchar( char ch ) {
       DWORD w = 0;
       ::WriteConsoleW( cnl_out(), &ch, 1, &w, &w );
}