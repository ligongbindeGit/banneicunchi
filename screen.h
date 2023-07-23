#pragma once
#include<Windows.h>
namespace srn {
       void settitle( char const *const pch );
       void gotoxy( short x, short y );
       void puts( char const *const pch );
       void puts( wchar_t const *const pch );
       void putchar( wchar_t ch );
       void putchar( char ch );
       void showcursor( bool show );
}


template< typename __tp>
void
show()
{
       std::cout<<typeid(__tp).name()<<": "<<sizeof( __tp )<<"\r\n";
}