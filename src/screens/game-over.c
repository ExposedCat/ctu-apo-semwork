#include "game-over.h"

void game_over_screen(Screen screen, struct timespec *loop_delay,
                      MemBase elements_memory, MemBase screen_memory,
                      int winner, int snake1_score, int snake2_score) {
    int pressed = 0;
    while (pressed != GREEN_KNOB) {
        get_knobs_state(elements_memory, NULL, NULL, NULL, &pressed);

        clear_screen(screen);

        draw_string(screen, 100, 100, "GAME OVER", BLACK);
        char label[100];
        if (winner == 0) {
            sprintf(label, "game ended in a DRAW (%d:%d)", snake1_score,
                    snake2_score);
        } else {
            sprintf(label, "%s player wins! (%d:%d)",
                    winner == 1 ? "YELLOW" : "BLUE", snake1_score,
                    snake2_score);
        }
        draw_string(screen, 100, 120, label, BLACK);
        draw_string(screen, 100, 200, "Press GREEN knob to continue...", BLACK);

        parlcd_write_cmd(screen_memory, 0x2c);
        for (int pixel = 0; pixel < SCREEN_WIDTH * SCREEN_HEIGHT; ++pixel) {
            parlcd_write_data(screen_memory, screen[pixel]);
        }
    }
    while (pressed == GREEN_KNOB) {
        get_knobs_state(elements_memory, NULL, NULL, NULL, &pressed);
        // Waiting to release the knob
    }
}