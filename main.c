#include "game.h"
#include <ncurses.h>
//#include <SDL2/SDL_mixer.h>
//#include <SDL2/SDL.h>


int main() {
    /*
    if (Mix_Init(MIX_INIT_MP3) == 0) {
        printf("SDL_mixer failed to initialize: %s\n", Mix_GetError());
        return -1;
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return -1;
    }
    */

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);  
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);

    display_name();
    main_menu();

    //Mix_CloseAudio();
    //Mix_Quit();

    endwin();
    return 0;
}
