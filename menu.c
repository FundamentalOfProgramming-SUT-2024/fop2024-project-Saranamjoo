#include "game.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <regex.h>
#include<string.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>


void draw_border() {
    attron(COLOR_PAIR(1));
    for (int i = 0; i < COLS; i++) {
        mvprintw(0, i, "#");
        mvprintw(LINES - 1, i, "#");
    }
    for (int i = 0; i < LINES; i++) {
        mvprintw(i, 0, "#");
        mvprintw(i, COLS - 1, "#");
    }
    attroff(COLOR_PAIR(1));
}

void display_name() {
    draw_border();
    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(LINES / 2 - 2, (COLS - 10) / 2, "ROGUE");
    attroff(A_BOLD | COLOR_PAIR(2));

    mvprintw(LINES / 2 + 5, (COLS - 30) / 2, "Press any key to continue...");
    attroff(COLOR_PAIR(1));
    refresh();
    getch();
}

void main_menu() {
    clear();
    draw_border();
    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(LINES / 2 - 4, (COLS - 30) / 2, "MAIN MENU");
    attroff(A_BOLD | COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    mvprintw(LINES / 2 -2, (COLS - 30) / 2, "1)Create Account");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(4));
    mvprintw(LINES / 2 -1, (COLS - 30) / 2, "2)Login");
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(5));
    mvprintw(LINES / 2, (COLS - 30) / 2, "3)Login as Guest");
    attroff(COLOR_PAIR(5));
    refresh();
    
    char choice;
    do {
        choice = getch();
        switch (choice) {
            case '1':
                clear();
                create_account();
                break;
            case '2':
                clear();
                login();
                break;
            case '3':
                clear();
                guest();
                break;
            default:
                mvprintw(LINES - 2, (COLS - 30) / 2, "Invalid choice. Try again.");
                refresh();
                break;
        }
    } while (choice != '1' && choice != '2' && choice != '3');
}

void create_account() {
    User new_user;
    while(1) {
        clear();
        draw_border();
        attron(A_BOLD | COLOR_PAIR(3));
        mvprintw(LINES / 2 - 6, (COLS - 30) / 2, "CREATE ACCOUNT");
        attroff(A_BOLD | COLOR_PAIR(3));

        attron(COLOR_PAIR(1));
        mvprintw(LINES / 2 - 4, (COLS - 30) / 2, "ENTER YOUR NAME: ");
        attroff(COLOR_PAIR(1));
        echo();
        getstr(new_user.name);
        noecho();

        if (!check_name(new_user.name)) {
            clear();
            draw_border();
            attron(A_BOLD | COLOR_PAIR(2));
            mvprintw(LINES / 2 - 4, (COLS - 30) / 2, "Username already exists. Try again.");
            attroff(A_BOLD | COLOR_PAIR(2));
            refresh();
            getch();
            continue;
        }

        attron(COLOR_PAIR(1));
        mvprintw(LINES / 2 - 2, (COLS - 30) / 2, "ENTER YOUR EMAIL: ");
        attroff(COLOR_PAIR(1));
        echo();
        getstr(new_user.email);
        noecho();

        if (!check_email(new_user.email)) {
            clear();
            draw_border();
            attron(A_BOLD | COLOR_PAIR(2));
            mvprintw(LINES / 2 - 4, (COLS - 30) / 2, "Invalid email format. Try again.");
            attroff(A_BOLD | COLOR_PAIR(2));
            refresh();
            getch();
            continue;
        }

        attron(COLOR_PAIR(1));
        mvprintw(LINES / 2 + 2, (COLS - 30) / 2, "Press '0' to generate a random password");
        mvprintw(LINES / 2, (COLS - 30) / 2, "ENTER YOUR PASSWORD: ");
        attroff(COLOR_PAIR(1));
        echo(); 
        getstr(new_user.pass); 
        noecho();

        if (strcmp(new_user.pass, "0") == 0) {
            generatePassword(new_user.pass);
            clear();
            draw_border();
            attron(A_BOLD | COLOR_PAIR(3));
            mvprintw(LINES / 2 - 6, (COLS - 30) / 2, "Random Password: ");
            attroff(A_BOLD | COLOR_PAIR(3));
            attron(COLOR_PAIR(1));
            mvprintw(LINES / 2 - 4, (COLS - 30) / 2, "%s", new_user.pass);
            attroff(COLOR_PAIR(1));
            refresh();
            getch();
        }

        if (!check_pass(new_user.pass)) {
            clear();
            draw_border();
            attron(A_BOLD | COLOR_PAIR(2));
            mvprintw(LINES / 2 - 4, (COLS - 30) / 2, "Invalid password format. Try again.");
            attroff(A_BOLD | COLOR_PAIR(2));
            refresh();
            getch();
            continue;
        }

        new_user.music = 1;
        new_user.difficulty = 1;
        new_user.player_color = 1;
        new_user.score = 0;
        new_user.level = 0;
        new_user.game_played = 0;
        new_user.join_time = time(NULL);

        save_user_settings(new_user);
        game_menu(&new_user);
        break;
    }
}

int check_name(const char *name) {
    FILE *file = fopen("users.txt", "r");
    if (!file) {
        return 1; 
    }

    char line[300];
    char existing_name[100];
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^|]|", existing_name);
        if (strcmp(name, existing_name) == 0) {
            fclose(file);
            return 0; 
        }
    }

    fclose(file);
    return 1; 
}


int check_pass(const char *password) {
    if (strlen(password) < 7) return 0;

    int upper = 0, lower = 0, digit = 0;
    for (int i = 0; i < strlen(password); i++) {
        if (isupper(password[i])) upper = 1;
        if (islower(password[i])) lower = 1;
        if (isdigit(password[i])) digit = 1;
        if (isspace(password[i])) return 0;  
    }

    return upper && lower && digit;
}

int check_email(const char *email) {
    regex_t reg;
    int result;

    const char *pattern = "^[a-zA-Z0-9]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,63}$";
    result = regcomp(&reg, pattern, REG_EXTENDED | REG_NOSUB);
    if (result) return 0;

    result = regexec(&reg, email, 0, NULL, 0);
    regfree(&reg);

    return !result; 
}

void generatePassword(char pass[]) {
    const char lowercase[] = "abcdefghijklmnopqrstuvwxyz";
    const char uppercase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char digits[] = "0123456789";
    const int length = 8;

    srand(time(NULL));

    
    pass[0] = lowercase[rand() % 26];
    pass[1] = uppercase[rand() % 26];
    pass[2] = digits[rand() % 10];

    for (int i = 3; i < length; i++) {
        if (rand() % 2 == 0) {
            pass[i] = lowercase[rand() % 26];
        } else {
            pass[i] = digits[rand() % 10];
        }
    }

    for (int i = 0; i < length; i++) {
        int j = rand() % length;
        char temp = pass[i];
        pass[i] = pass[j];
        pass[j] = temp;
    }

    pass[length] = '\0'; 
}

void login(){
    char name[100], pass[100];
    attron(COLOR_PAIR(2));
    mvprintw(LINES / 2 - 6, (COLS - 30) / 2, "LOGIN");
    attroff(COLOR_PAIR(2));


    attron(COLOR_PAIR(1));
    mvprintw(LINES / 2 - 4, (COLS - 30) / 2, "ENTER YOUR NAME: ");
    attroff(COLOR_PAIR(1));
    echo();
    getstr(name);
    noecho();

    attron(COLOR_PAIR(1));
    mvprintw(LINES / 2 - 2, (COLS - 30) / 2, "ENTER YOUR PASSWORD: ");
    attroff(COLOR_PAIR(1));
    echo();
    getstr(pass);
    noecho();

    User user = load_user_settings(name);
   if (strcmp(pass, user.pass) == 0) {
       attron(COLOR_PAIR(2));
       mvprintw(LINES / 2 + 2, (COLS - 30) / 2, "Login successful!");
       attroff(COLOR_PAIR(2));
       getch();
       game_menu(&user);
   } else {
       attron(COLOR_PAIR(1));
       mvprintw(LINES / 2 + 2, (COLS - 30) / 2, "Invalid name or password.");
       attroff(COLOR_PAIR(1));
       getch();
       main_menu();
   }

}


User load_user_settings(const char *name) {
    User user;
    FILE *file = fopen("users.txt", "r");

    if (file) {
        char line[300];
        while (fgets(line, sizeof(line), file)) {
            char file_name[100];
            sscanf(line, "%[^|]|%[^|]|%[^|]|%d|%d|%d|%d|%d|%d|%ld", 
                   file_name, 
                   user.email,
                   user.pass, 
                   &user.difficulty, 
                   &user.player_color,
                   &user.music, 
                   &user.score,
                   &user.level,
                   &user.game_played,
                   &user.join_time);

            if (strcmp(name, file_name) == 0) {
                strcpy(user.name, file_name);
                fclose(file);
                return user;
            }
        }
        fclose(file);
    }

    strcpy(user.name, name);
    strcpy(user.email, "unknown@example.com");
    strcpy(user.pass, "");
    user.difficulty = 1;
    user.player_color = 2;
    user.music = 1;
    user.score = 0;
    user.level = 0;
    user.game_played = 0;
    user.join_time = time(NULL);
    return user;
}

void save_user_settings(User user) {
    FILE *file = fopen("users.txt", "a");
    if (file) {
        fprintf(file, "%s|%s|%s|%d|%d|%d|%d|%d|%d|%ld\n", 
                user.name, 
                user.email,
                user.pass,  
                user.difficulty, 
                user.player_color, 
                user.music, 
                user.score, 
                user.level,
                user.game_played, 
                user.join_time);
        fclose(file);
    }
}


void guest() {
    User guest_user;
    strcpy(guest_user.name, "Guest");
    strcpy(guest_user.email, "guest@example.com");
    guest_user.difficulty = 1;
    guest_user.player_color = 1;
    guest_user.music = 1;
    guest_user.score = 0;
    guest_user.level = 0;
    guest_user.game_played = 0;
    guest_user.join_time = time(NULL);

    attron(COLOR_PAIR(5));
    mvprintw(LINES / 2, (COLS - 30) / 2, "Logged in as Guest");
    attroff(COLOR_PAIR(5));
    refresh();
    getch();

    game_menu(&guest_user);
}


void game_menu(User *user) {
    char current_user_name[100];
    strcpy(current_user_name, user->name);
    clear();
    draw_border();

    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(LINES / 2 - 4, (COLS - 30) / 2, "GAME MENU");
    attroff(A_BOLD | COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    mvprintw(LINES / 2 -2, (COLS - 30) / 2, "1)New GAME");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(4));
    mvprintw(LINES / 2 -1, (COLS - 30) / 2, "2)Continue Game");
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(5));
    mvprintw(LINES / 2, (COLS - 30) / 2, "3)Scoreboard");
    attroff(COLOR_PAIR(5));
    attron(COLOR_PAIR(2));
    mvprintw(LINES / 2 +1, (COLS - 30) / 2, "4)Settings");
    attroff(COLOR_PAIR(2));
    refresh();
    char choice = getch();
    switch (choice) {
        case '1':
            new_game(user); 
            break;
        case '2':
            continue_game(user);
            break;
        case '3':
            show_scores(current_user_name);
            break;
        case '4':
            settings(user); 
            break;
        default:
            game_menu(user); 
    }
}


void settings(User *user) {
    clear();
    draw_border();
    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(LINES / 2 - 2, (COLS - 30) / 2, "GAME SETTINGS");
    attroff(A_BOLD | COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    mvprintw(LINES / 2, (COLS - 30) / 2, "1) Change Difficulty");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(4));
    mvprintw(LINES / 2 + 1, (COLS - 30) / 2, "2) Change Music");
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(5));
    mvprintw(LINES / 2 + 2, (COLS - 30) / 2, "3) Change Character Color");
    attroff(COLOR_PAIR(5));
    refresh();

    char choice = getch();
    switch (choice) {
        case '1':
            mvprintw(LINES / 2 + 4, (COLS - 30) / 2, "Select difficulty (1-3): ");
            echo();
            char diff[2];
            getstr(diff);
            noecho();
            user->difficulty = atoi(diff);
            break;
        case '2':
            mvprintw(LINES / 2 + 4, (COLS - 30) / 2, "Select music file name (1-3): ");
            echo();
            char mus[2];
            getstr(mus);
            noecho();
            user->music = atoi(mus);
            //play_music(user);
            break;
        case '3':
            mvprintw(LINES / 2 + 4, (COLS - 30) / 2, "Enter character color (0-7): ");
            echo();
            char color[2];
            getstr(color);
            noecho();
            user->player_color = atoi(color);
            break;
        default:
            break;
    }

    save_user_settings(*user); 
    game_menu(user);
}


void show_scores(const char *current_user_name) {
    clear();
    draw_border();
    mvprintw(1, (COLS - 30) / 2, "SCOREBOARD");

    FILE *file = fopen("users.txt", "r");
    User *users = NULL;
    int user_count = 0;
    size_t capacity = 10;
    users = malloc(capacity * sizeof(User));
    
    while (fscanf(file, "%99[^|]|%199[^|]|%99[^|]|%d|%d|%d|%d|%d|%d|%ld\n", 
                  users[user_count].name, 
                  users[user_count].email, 
                  users[user_count].pass, 
                  &users[user_count].difficulty, 
                  &users[user_count].player_color, 
                  &users[user_count].music, 
                  &users[user_count].score, 
                  &users[user_count].level, 
                  &users[user_count].game_played, 
                  &users[user_count].join_time) != EOF) {
        user_count++;
        if (user_count >= capacity) {
            capacity *= 2;
            users = realloc(users, capacity * sizeof(User));
        }
    }
    fclose(file);

    for (int i = 0; i < user_count - 1; i++) {
        for (int j = i + 1; j < user_count; j++) {
            if (users[i].score < users[j].score) {
                User temp = users[i];
                users[i] = users[j];
                users[j] = temp;
            }
        }
    }

    int offset = 0;
    int ch;

    while (1) {
        clear();
        draw_border();
        attron(COLOR_PAIR(3));
        mvprintw(1, (COLS - 30) / 2, "SCOREBOARD");
        mvprintw(3, (COLS - 70) / 2, "Rank   Username             Score      Games   Join Time");
        mvhline(4, (COLS - 70) / 2, '-', 70);
        attroff(COLOR_PAIR(3));

        for (int i = offset; i < user_count && i < offset + LINES - 7; i++) {
            char join_time_str[50];
            strftime(join_time_str, sizeof(join_time_str), "%b %d %Y", localtime(&users[i].join_time));

            int color_pair = 1;
            char display_name[120];

            if (strcmp(users[i].name, current_user_name) == 0) {
                if (i == 0 || i == 1 || i == 2) {
                    color_pair = (i == 0) ? 2 : (i == 1) ? 5 : 4;
                    snprintf(display_name, sizeof(display_name), "%s (%s)", users[i].name, 
                             (i == 0) ? "GOAT" : (i == 1) ? "Legend" : "Champion");
                } else {
                    color_pair = 6;
                    snprintf(display_name, sizeof(display_name), "%s", users[i].name);
                }
            } 
            else if (i == 0) {
                color_pair = 2;
                snprintf(display_name, sizeof(display_name), "%s (GOAT)", users[i].name);
            } 
            else if (i == 1) {
                color_pair = 5;
                snprintf(display_name, sizeof(display_name), "%s (Legend)", users[i].name);
            } 
            else if (i == 2) {
                color_pair = 4;
                snprintf(display_name, sizeof(display_name), "%s (Champion)", users[i].name);
            } 
            else {
                strcpy(display_name, users[i].name);
            }

            attron(COLOR_PAIR(color_pair));
            mvprintw(6 + (i - offset), (COLS - 70) / 2, "%-6d %-20s %-10d %-8d %-15s", 
                     i + 1, display_name, users[i].score, users[i].game_played, join_time_str);
            attroff(COLOR_PAIR(color_pair));
        }

        mvprintw(LINES - 2, (COLS - 70) / 2, "Use UP/DOWN to scroll, 'q' to quit.");
        refresh();

        ch = getch();
        if (ch == 'q') {
            break;
        } else if (ch == KEY_UP && offset > 0) {
            offset--;
        } else if (ch == KEY_DOWN && offset + LINES - 7 < user_count) {
            offset++;
        }
    }

    free(users);
    endwin();
}


int load_game(GameState *state) {
    FILE *file = fopen("save_game.txt", "r");
    if (file) {
        fscanf(file, "%d %d %d %d", 
               &state->score, 
               &state->level, 
               &state->player_x, 
               &state->player_y);
        fclose(file);
        return 1;
    } else {
        return 0; 
    }
}

void continue_game(User *user) {
    GameState state;
    if (load_game(&state)) {
        clear();
        draw_border();
        mvprintw(LINES / 2, (COLS - 30) / 2, "Continuing your previous game...");
        refresh();
        getch();

        // منطق ادامه بازی
        game_menu(user);
    } else {
        clear();
        draw_border();
        mvprintw(LINES / 2, (COLS - 30) / 2, "No saved game found.");
        refresh();
        getch();
        game_menu(user);
    }
}


void new_game(User *user) {
    clear();
    draw_border();
    int total_rooms = create_rooms();
    show_map(total_rooms);
    //شروع بازی
    refresh();
    getch();
    game_menu(user);
}

/*
void play_music(User *user) {
    char filename[100];
    sprintf(filename, "music/music%d.mp3", user->music); 

    Mix_Music *music = Mix_LoadMUS(filename);
    if (!music) {
        mvprintw(LINES - 2, 0, "Failed to load music: %s", Mix_GetError());
        refresh();
        return;
    }

    if (Mix_PlayingMusic()) {
        Mix_HaltMusic(); 
    }

    Mix_PlayMusic(music, -1); 
}
*/
