#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define CLEAR "clear"

int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    
    return 0;
}

void play_sound() {
    system("afplay /System/Library/Sounds/Ping.aiff 2>/dev/null");
}

void display_timer(int elapsed, int total, const char* label, int paused) {
    system(CLEAR);
    printf("\n=== ТАЙМЕР ПОМОДОРО ===\n\n");
    
    if (paused) {
        printf("⏸️  \x1b[33m[ПАУЗА]\x1b[0m\n");
    }
    
    printf("%s\n", label);
    printf("Время: %02d:%02d / %02d:00\n\n", 
           elapsed / 60, elapsed % 60, total / 60);
    
    int width = 40;
    int progress = (elapsed * width) / total;
    printf("[");
    for (int i = 0; i < width; i++) {
        if (i < progress) printf("#");
        else printf(".");
    }
    printf("]\n\n");
    
    printf("Управление: [ПРОБЕЛ] - пауза, [Q] - выход\n");
}

int main() {
    int work_time = 45 * 60;
    int break_time = 5 * 60;
    int paused = 0;
    int should_quit = 0;
    int pomodoro_count = 0;
    
    printf("=== ТАЙМЕР ПОМОДОРО ===\n");
    printf("Пауза по пробелу. Начинаем через 2 секунды...\n");
    sleep(2);
    
    while (!should_quit) {
        pomodoro_count++;
        
        // Рабочий таймер
        printf("\n=== Помодоро #%d (Работа) ===\n", pomodoro_count);
        for (int i = 0; i <= work_time && !should_quit; i++) {
            display_timer(i, work_time, "РАБОТАЙТЕ!", paused);
            
            if (kbhit()) {
                char c = getchar();
                if (c == ' ') {
                    paused = !paused;
                    if (paused) {
                        printf("\n\x1b[33mПауза! Нажмите ПРОБЕЛ для продолжения\x1b[0m\n");
                    }
                } else if (c == 'q' || c == 'Q') {
                    should_quit = 1;
                    break;
                }
            }
            
            if (!paused) {
                sleep(1);
            } else {
                usleep(100000);
                i--; 
		   	}
        }
        
        if (!should_quit) {
            play_sound();
            printf("\n\x1b[32mВремя работы закончилось! Начинается перерыв.\x1b[0m\n");
            sleep(2);
        }
        
        // Перерыв
        if (!should_quit) {
            printf("\n=== Перерыв ===\n");
            for (int i = 0; i <= break_time && !should_quit; i++) {
                display_timer(i, break_time, "ОТДЫХАЙТЕ!", paused);
                
                if (kbhit()) {
                    char c = getchar();
                    if (c == ' ') {
                        paused = !paused;
                    } else if (c == 'q' || c == 'Q') {
                        should_quit = 1;
                        break;
                    } else if (c == 's' || c == 'S') {
                        i = break_time;
                    }
                }
                
                if (!paused) {
                    sleep(1);
                } else {
                    usleep(100000);
                    i--;
                }
            }
            
            if (!should_quit) {
                play_sound();
                printf("\n\x1b[32mПерерыв закончился! Возвращайтесь к работе.\x1b[0m\n");
                sleep(2);
            }
        }
        
        // Длинный перерыв после 4 помодоро
        if (pomodoro_count % 4 == 0 && !should_quit) {
            printf("\n\x1b[36m Длинный перерыв 15 минут! Вы хорошо поработали!\x1b[0m\n");
            sleep(3);
        }
    }
    
    printf("\n\x1b[35mТаймер остановлен. До свидания!\x1b[0m\n");
    
    return 0;
}
