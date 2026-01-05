#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define CLEAR "clear"

// Функции для воспроизведения звука на macOS
void play_sound_mac() {
    // Вариант 1: Используем системный звук через afplay (наиболее надежный)
    system("afplay /System/Library/Sounds/Glass.aiff 2>/dev/null");
    
    // Вариант 2: Можно использовать say для голосового уведомления
    // system("say 'Таймер завершен' &");
}

// Альтернатива: использовать Terminal Bell и дополнительные звуки
void play_beep_sequence() {
    // Сначала системный beep
    printf("\a");
    fflush(stdout);
    
    // Затем более заметный звук через afplay
    system("afplay /System/Library/Sounds/Ping.aiff 2>/dev/null");
    
    // Небольшая задержка
    sleep(1);
    
    // Еще один звук для надежности
    system("afplay /System/Library/Sounds/Pop.aiff 2>/dev/null 2>/dev/null");
}

// Функция для проверки доступности звуковых утилит
int check_audio_available() {
    int result = system("which afplay > /dev/null 2>&1");
    return (result == 0);
}

// Функция отображения прогресс-бара
void display_progress(int elapsed, int total, const char* label) {
    float progress = (float)elapsed / total;
    int bar_width = 50;
    int pos = bar_width * progress;
    
    system(CLEAR);
    printf("\n=== ТАЙМЕР ПОМОДОРО ===\n\n");
    printf("%s\n", label);
    printf("Время: %02d:%02d / %02d:00\n\n", 
           elapsed / 60, elapsed % 60, total / 60);
    
    printf("[");
    for (int i = 0; i < bar_width; i++) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %.1f%%\n\n", progress * 100);
    
    printf("Нажмите Ctrl+C для выхода\n");
    fflush(stdout);
}

// Функция таймера
void start_timer(int minutes, const char* label) {
    int total_seconds = minutes * 60;
    int elapsed = 0;
    
    while (elapsed <= total_seconds) {
        display_progress(elapsed, total_seconds, label);
        sleep(1);
        elapsed++;
    }
    
    // Воспроизводим звук по окончании
    printf("\n\x1b[1;32mВремя вышло!\x1b[0m %s\n", label);
    printf("\x1b[33m♫ ♪ ♫ Звуковое уведомление... ♫ ♪ ♫\x1b[0m\n\n");
    fflush(stdout);
    
    // Проверяем доступность звука
    if (check_audio_available()) {
        play_sound_mac();
    } else {
        play_beep_sequence();
    }
    
    // Ждем немного перед продолжением
    printf("Следующий этап через 3 секунды...\n");
    sleep(3);
}

int main() {
    int pomodoro_count = 0;
    int audio_available = check_audio_available();
    
    printf("=== ТАЙМЕР ПОМОДОРО ===\n");
    printf("Режим: 45 минут работы / 5 минут отдыха\n");
    
    if (audio_available) {
        printf("Статус звука: \x1b[32mдоступен\x1b[0m (используется afplay)\n");
    } else {
        printf("Статус звука: \x1b[33mограничен\x1b[0m (используется системный beep)\n");
        printf("Для лучшего звука убедитесь, что afplay доступен\n");
    }
    
    printf("\nТаймер запущен...\n");
    sleep(2);
    
    // Список звуков для разнообразия (если доступны)
    const char* sounds[] = {
        "/System/Library/Sounds/Glass.aiff",
        "/System/Library/Sounds/Ping.aiff",
        "/System/Library/Sounds/Pop.aiff",
        "/System/Library/Sounds/Blow.aiff",
        "/System/Library/Sounds/Bottle.aiff",
        "/System/Library/Sounds/Funk.aiff",
        "/System/Library/Sounds/Morse.aiff",
        "/System/Library/Sounds/Tink.aiff"
    };
    int sound_count = 8;
    
    while (1) {
        pomodoro_count++;
        
        printf("\n=== Помодоро #%d ===\n", pomodoro_count);
        start_timer(1, "\x1b[31mРАБОТАЙТЕ! (45 минут)\x1b[0m");
        
        // Для разнообразия чередуем звуки
        if (audio_available) {
            char sound_command[100];
            snprintf(sound_command, sizeof(sound_command), 
                    "afplay \"%s\" 2>/dev/null", 
                    sounds[pomodoro_count % sound_count]);
            system(sound_command);
        }
        
        start_timer(5, "\x1b[32mОТДЫХАЙТЕ! (5 минут)\x1b[0m");
        
        if (pomodoro_count % 4 == 0) {
            printf("\n=== ДЛИННЫЙ ПЕРЕРЫВ (15 минут) ===\n");
            printf("\x1b[36mВы завершили 4 помодоро!\x1b[0m\n");
            start_timer(15, "\x1b[36mДЛИННЫЙ ПЕРЕРЫВ! (15 минут)\x1b[0m");
            
            // Особый звук для длинного перерыва
            if (audio_available) {
                system("afplay /System/Library/Sounds/Fanfare.aiff 2>/dev/null");
            }
        }
    }
    
    return 0;
}
