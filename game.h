#ifndef GAME_H
#define GAME_H

#include<time.h>
#include<stdlib.h>
#include<stdbool.h>
//#include <SDL2/SDL_mixer.h> 
//#include <SDL2/SDL.h>


typedef struct{
    char name[100];
    char email[200];
    char pass[100];
    int difficulty;
    int player_color;
    int music;
    int score;
    int level;
    int game_played;
    time_t join_time;
} User;

typedef struct {
    int score;
    int level;
    int player_x, player_y;
} GameState;

#define MAX_ROOMS 10

typedef struct {
    int x, y, width, height;
    int door_x, door_y;
} Room;
extern Room map_rooms[MAX_ROOMS];


void display_name();
void main_menu();
void create_account();
void generatePassword(char pass[]);
int check_name(const char *name);
int check_pass(const char *password);
int check_email(const char *email);
void login();
User load_user_settings(const char *name);
void save_user_settings(User user);
void guest();
void game_menu(User *user);
void new_game(User *user);
void continue_game(User *user);
int load_game(GameState *state);
void show_scores(const char *current_user_name);
void settings(User *user);
//void play_music(User *user);
int rooms_overlap(Room a, Room b);
int create_rooms();
void draw_room(int n);
void draw_corridor(int x1, int y1, int x2, int y2);
void place_doors(int total_rooms);
void show_map(int total_rooms);
void set_door_location(Room *room);
bool is_inside_room(int x, int y);
#endif
