#ifndef _TERM_ENGINE_H
#define _TERM_ENGINE_H

#include <sstream>

////////////////////////////////////////
//
// SOME DEFINES 
//
////////////////////////////////////////
#define STR_HELPER(x)       #x
#define STR(x)              STR_HELPER(x)

#define SQ_ESC              "\x1b["
#define SQ_SET_POS(X, Y)    SQ_ESC STR(Y) ";" STR(X) "H"
#define SQ_SETPOS(s, x, y)  s << SQ_ESC << y << ";" << x <<  "H"
#define SQ_CLEAR            SQ_ESC "2J"
#define SQ_LEFT             SQ_ESC "1D"
#define SQ_RIGHT            SQ_ESC "1C"
#define SQ_UP               SQ_ESC "1A" SQ_LEFT
#define SQ_DOWN             SQ_ESC "1B" SQ_LEFT
#define SQ_NL               SQ_ESC "1E" 

// Colors
#define SQ_SET_COLOR(C)     SQ_ESC C
#define FRED		        SQ_ESC "31m"
#define BRED		        SQ_ESC "41m"
#define FGREEN		        SQ_ESC "32m" 
#define BGREEN		        SQ_ESC "42m"
#define FYELLOW 	        SQ_ESC "33m"
#define BYELLOW 	        SQ_ESC "43m"
#define FBLUE		        SQ_ESC "34m"
#define BBLUE		        SQ_ESC "44m"
#define FMAGENTA	        SQ_ESC "35m"
#define BMAGENTA	        SQ_ESC "45m"
#define FLGRAY		        SQ_ESC "37m" 
#define BLGRAY		        SQ_ESC "47m"
#define FGRAY		        SQ_ESC "90m" 
#define BGRAY		        SQ_ESC "100m"
#define CRESET		        SQ_ESC "0m" 

namespace te 
{
    struct Context
    {
        int w, h;
        bool is_running;
    };

    ////////////////////////////////////////
    //
    // PLATFORM DEP FUNCTIONS
    //
    ////////////////////////////////////////
    void enable_rawmode();
    void disable_rawmode();
    char get_key();
    bool set_size(te::Context & ctx, int w, int h);
    void get_win_size(int & w, int & h);
    void write_data(const std::stringstream &);
}

#endif

#ifdef _TERM_ENGINE_IMPL

#ifdef _WIN32

#include <windows.h>

HANDLE hOut;
HANDLE hIn;
DWORD dwOriginalInMode = 0;
DWORD dwOriginalOutMode = 0;

void te::enable_rawmode()
{
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hIn = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hOut, &dwOriginalOutMode);

    GetConsoleMode(hIn, &dwOriginalInMode);

    DWORD dwModeOut = 0;
    DWORD dwModeIn = 0;
    dwModeOut = dwOriginalOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    dwModeOut |= DISABLE_NEWLINE_AUTO_RETURN;
    dwModeOut &= ~ENABLE_WRAP_AT_EOL_OUTPUT ;
    dwModeIn = dwOriginalInMode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT); 
    dwModeIn &= ~(ENABLE_PROCESSED_INPUT); 

    SetConsoleMode(hOut, dwModeOut);
    SetConsoleMode(hIn, dwModeIn);
}

void te::disable_rawmode()
{
    SetConsoleMode(hOut, dwOriginalOutMode);
    SetConsoleMode(hIn, dwOriginalInMode);
}

void te::write_data(const std::stringstream & ss)
{
    std::string str = ss.str();
    WriteFile(hOut, str.c_str(), std.size(), NULL, NULL);
}

char te::get_key()
{
    char c;
    ReadFile(hIn, &c, 1, NULL, NULL);
    return c;
}

bool te::set_size(te::Context & ctx, int w, int h)
{
    SMALL_RECT rect{0, 0, w, h};
    auto ret = SetConsoleWindowInfo(hOut, TRUE, &rect);

    if(ret)
    {
        return true;
    }
    return false;
}

void te::get_win_size(int & w, int & h)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    auto ret = GetConsoleScreenBufferInfo(hOut, &csbi);

    if(ret)
    {
        w = csbi.dwSize.X;
        h = csbi.dwSize.Y;
    }

}
#else
// UNIX systems
#include <termios.h>
#include <unistd.h>
#include <iostream>

termios orig;
void te::enable_rawmode()
{
    if(tcgetattr(STDIN_FILENO, &orig) == -1)
        exit(1);

    termios mod = orig;

    mod.c_iflag &= (BRKINT);
    mod.c_iflag &= ~(ICRNL);
    mod.c_lflag &= ~(ECHO | ICANON);
    mod.c_cc[VMIN] = 1;
    mod.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &mod);
}

void te::disable_rawmode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
}

void te::write_data(const std::stringstream &ss)
{
    std::cout << ss.str();
}

char te::get_key()
{
    char c = 0;
    std::cin >> c;
    return c;
}

bool te::set_size(te::Context & ctx, int w, int h)
{
    return false;
}

void te::get_win_size(int & w, int & h)
{
}

#endif

#endif

/* SOME NOTES
 *
 * Context is allocated by user, and managed by our application
 *
 * The user should implement 4 functions : on_init, on_draw, on_input and on_terminate
 *
 * These functions will be used by thier corresponding internal impl, init, draw input, terminate
 *
 * enable_rawmode, disable_rawmode get_key and set_size are platform dependant,
 * each platform will have different implementations
 *
 * Still not sure if we should manage the main method internallly.
 *
 */
