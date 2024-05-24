#include "board.h"

void get_knobs_state(MemBase knobs_memory, int *red_knob, int *green_knob,
                     int *blue_knob, int *clicked) {
    int rgb_knobs_value =
        *(volatile uint32_t *)(knobs_memory + SPILED_REG_KNOBS_8BIT_o);

    if (blue_knob) {
        *blue_knob = (rgb_knobs_value & 0x000000ff);
    }
    if (red_knob) {
        *red_knob = (rgb_knobs_value & 0x00ff0000) >> 16;
    }
    if (green_knob) {
        *green_knob = (rgb_knobs_value & 0x0000ff00) >> 8;
    }
    if (clicked) {
        *clicked = (rgb_knobs_value & 0xff000000) >> 24;
    }
}

int init_board(Screen *screen, MemBase *screen_memory, MemBase *knobs_memory) {
    *screen = (Screen)malloc(SCREEN_HEIGHT * SCREEN_WIDTH * 2);

    *screen_memory = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (!*screen_memory) {
        free(screen);
        fprintf(stderr,
                "Failed to initialize board: Screen memory allocation "
                "failed\n");
        return 1;
    }

    *knobs_memory = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    if (!*knobs_memory) {
        free(screen);
        fprintf(stderr,
                "Failed to initialize board: Knobs memory allocation "
                "failed\n");
        return 1;
    }

    parlcd_hx8357_init(*screen_memory);

    parlcd_write_cmd(*screen_memory, 0x2c);
    for (int pixel = 0; pixel < SCREEN_HEIGHT * SCREEN_WIDTH; ++pixel) {
        (*screen)[pixel] = 0;
        parlcd_write_data(*screen_memory, (*screen)[pixel]);
    }

    return 0;
}

void set_led_line(MemBase board_memory, int units) {
    *(volatile uint32_t *)(board_memory + SPILED_REG_LED_LINE_o) =
        0xFFFFFFFF << (32 - units);
}