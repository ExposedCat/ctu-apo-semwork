#include "main-menu.h"

int main_menu_screen(Screen screen, struct timespec *loop_delay,
                     MemBase elements_memory, MemBase screen_memory) {
    int pressed = 0;
    while (pressed != RED_KNOB + BLUE_KNOB) {
        int green_knob = 0;
        get_knobs_state(elements_memory, NULL, &green_knob, NULL, &pressed);

        if (pressed == GREEN_KNOB) {
            set_led_line(elements_memory, 0);

            parlcd_write_cmd(screen_memory, 0x2c);
            for (int pixel = 0; pixel < SCREEN_WIDTH * SCREEN_HEIGHT; pixel++) {
                parlcd_write_data(screen_memory, 0);
            }
            return 42;
        }

        set_speed(loop_delay, elements_memory, green_knob);

        clear_screen(screen);

        draw_string(screen, 100, 100, "Multiplayer Snake Game", BLACK);
        draw_string(screen, 100, 120,
                    "by Ievgeniia Rachkovska and Artem Prokop", BLACK);
        draw_string(screen, 100, 185, "Change speed using GREEN knob", BLACK);
        draw_string(screen, 100, 205, "Quit by pressing GREEN knob", BLACK);

        draw_string(screen, 10, SCREEN_HEIGHT - 50, "Player 1", RED);
        draw_string(screen, 10, SCREEN_HEIGHT - 20,
                    pressed == RED_KNOB ? "Ready!" : "Ready? Hold RED knob",
                    BLACK);
        draw_string(screen, SCREEN_WIDTH - 90, SCREEN_HEIGHT - 50, "Player 2",
                    BLUE);
        draw_string(screen, SCREEN_WIDTH - (pressed == BLUE_KNOB ? 80 : 180),
                    SCREEN_HEIGHT - 20,
                    pressed == BLUE_KNOB ? "Ready!" : "Ready? Hold BLUE knob",
                    BLACK);

        parlcd_write_cmd(screen_memory, 0x2c);
        for (int pixel = 0; pixel < SCREEN_WIDTH * SCREEN_HEIGHT; ++pixel) {
            parlcd_write_data(screen_memory, screen[pixel]);
        }
    }

    return 0;
}
