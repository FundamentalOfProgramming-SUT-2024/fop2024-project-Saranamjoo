#include "game.h"
#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_ROOMS 10

Room map_rooms[MAX_ROOMS];
bool corridor_map[80][210] = {false};

int rooms_overlap(Room a, Room b) {
    return !(a.x + a.width < b.x || b.x + b.width < a.x || 
             a.y + a.height < b.y || b.y + b.height < a.y);
}

void set_door_location(Room *room) {
    int side, x, y;
    bool valid = false;
    while (!valid) {
        side = rand() % 4;
        switch (side) {
            case 0:
                x = room->x + rand() % room->width;
                y = room->y;
                break;
            case 1:
                x = room->x + rand() % room->width;
                y = room->y + room->height - 1;
                break;
            case 2:
                x = room->x;
                y = room->y + rand() % room->height;
                break;
            case 3:
                x = room->x + room->width - 1;
                y = room->y + rand() % room->height;
                break;
        }
        if (mvinch(y, x) != '#' && mvinch(y, x) != '|' && mvinch(y, x) != '-' && mvinch(y, x) != '.') {
            valid = true;
        }
    }
    room->door_x = x;
    room->door_y = y;
}

bool is_inside_room(int x, int y) {
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (map_rooms[i].width == 0) continue;
        if (x >= map_rooms[i].x && x < map_rooms[i].x + map_rooms[i].width &&
            y >= map_rooms[i].y && y < map_rooms[i].y + map_rooms[i].height) {
            return true;
        }
    }
    return false;
}

void draw_room(int n) {
    for (int i = map_rooms[n].x; i < map_rooms[n].x + map_rooms[n].width; i++) {
        for (int j = map_rooms[n].y; j < map_rooms[n].y + map_rooms[n].height; j++) {
            if (j == map_rooms[n].y || j == map_rooms[n].y + map_rooms[n].height - 1)
                mvaddch(j, i, '-');
            else if (i == map_rooms[n].x || i == map_rooms[n].x + map_rooms[n].width - 1)
                mvaddch(j, i, '|');
            else
                mvaddch(j, i, '.');
        }
    }
}

void draw_corridor(int x1, int y1, int x2, int y2) {
    int current_x = x1;
    int current_y = y1;

    // ابتدا از در x1, y1 به صورت افقی یا عمودی حرکت می‌کنیم
    while (current_x != x2 || current_y != y2) {
        if (current_x < x2) current_x++;  // حرکت به سمت راست
        else if (current_x > x2) current_x--;  // حرکت به سمت چپ
        else if (current_y < y2) current_y++;  // حرکت به پایین
        else if (current_y > y2) current_y--;  // حرکت به بالا

        // بررسی اینکه آیا خانه مورد نظر داخل هیچ اتاقی نیست
        if (!is_inside_room(current_x, current_y)) {
            if (!corridor_map[current_y][current_x]) {
                corridor_map[current_y][current_x] = true;
                mvaddch(current_y, current_x, '#');
            }
        }
    }
}

int create_rooms() {
    int room_count = rand() % 5 + 6;
    for (int i = 0; i < room_count; i++) {
        map_rooms[i].width = rand() % 7 + 4;
        map_rooms[i].height = rand() % 5 + 4;
        map_rooms[i].x = rand() % (COLS - map_rooms[i].width - 2) + 1;
        map_rooms[i].y = rand() % (LINES - map_rooms[i].height - 2) + 1;
        
        bool overlap = false;
        for (int j = 0; j < i; j++) {
            if (rooms_overlap(map_rooms[i], map_rooms[j])) {
                overlap = true;
                break;
            }
        }
        
        if (overlap) {
            i--;
        } else {
            set_door_location(&map_rooms[i]);
        }
    }
    return room_count;
}

void place_doors(int total_rooms) {
    for (int i = 0; i < total_rooms; i++) {
        mvaddch(map_rooms[i].door_y, map_rooms[i].door_x, '+');
    }
}

void show_map(int total_rooms) {
    clear();
    // ابتدا همه اتاق‌ها را رسم می‌کنیم
    for (int i = 0; i < total_rooms; i++) {
        draw_room(i);
    }
    
    // سپس راهروها را از در اتاق‌ها رسم می‌کنیم
    for (int i = 0; i < total_rooms - 1; i++) {
        // رسم راهرو از در اتاق i به در اتاق i+1
        draw_corridor(map_rooms[i].door_x, map_rooms[i].door_y,
                      map_rooms[i + 1].door_x, map_rooms[i + 1].door_y);
    }
    
    // درهای اتاق‌ها را نمایش می‌دهیم
    place_doors(total_rooms);
    refresh();
}
