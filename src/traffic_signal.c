#define _GNU_SOURCE
#include "traffic_signal.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

volatile sig_atomic_t running = 1;
pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;

static struct road roads[ROAD_COUNT] = {
    {1, 20, 4, 17, 27, 0, NO_TRAFFIC},
    {2, 21, 22, 23, 24, 0, NO_TRAFFIC},
    {3, 26, 25, 5, 6, 0, NO_TRAFFIC},
    {4, 19, 12, 13, 16, 0, NO_TRAFFIC}
};

static struct gpiod_line *sensor_lines[ROAD_COUNT];
static struct gpiod_line *red_lines[ROAD_COUNT];
static struct gpiod_line *yellow_lines[ROAD_COUNT];
static struct gpiod_line *green_lines[ROAD_COUNT];

static void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

void sleep_ms(unsigned int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

static struct gpiod_line *get_line(struct gpiod_chip *chip, unsigned int offset) {
    struct gpiod_line *line = gpiod_chip_get_line(chip, offset);
    if (!line) fprintf(stderr, "GPIO %u unavailable: %s\n", offset, strerror(errno));
    return line;
}

int gpio_init(struct gpiod_chip **chip) {
    *chip = gpiod_chip_open_by_number(0);
    if (!*chip) {
        fprintf(stderr, "Unable to open GPIO chip 0: %s\n", strerror(errno));
        return -1;
    }

    for (int i = 0; i < ROAD_COUNT; i++) {
        sensor_lines[i] = get_line(*chip, roads[i].sensor_gpio);
        red_lines[i] = get_line(*chip, roads[i].red_gpio);
        yellow_lines[i] = get_line(*chip, roads[i].yellow_gpio);
        green_lines[i] = get_line(*chip, roads[i].green_gpio);
        if (!sensor_lines[i] || !red_lines[i] || !yellow_lines[i] || !green_lines[i]) return -1;

        if (gpiod_line_request_input(sensor_lines[i], "traffic-sensor") < 0) return -1;
        if (gpiod_line_request_output(red_lines[i], "traffic-red", 1) < 0) return -1;
        if (gpiod_line_request_output(yellow_lines[i], "traffic-yellow", 0) < 0) return -1;
        if (gpiod_line_request_output(green_lines[i], "traffic-green", 0) < 0) return -1;
    }
    return 0;
}

void gpio_cleanup(struct gpiod_chip *chip) {
    for (int i = 0; i < ROAD_COUNT; i++) {
        if (sensor_lines[i]) gpiod_line_release(sensor_lines[i]);
        if (red_lines[i]) gpiod_line_release(red_lines[i]);
        if (yellow_lines[i]) gpiod_line_release(yellow_lines[i]);
        if (green_lines[i]) gpiod_line_release(green_lines[i]);
    }
    if (chip) gpiod_chip_close(chip);
}

int set_signal(struct gpiod_chip *chip, const struct road *r, int red, int yellow, int green) {
    (void)chip;
    int i = r->id - 1;
    if (gpiod_line_set_value(red_lines[i], red) < 0) return -1;
    if (gpiod_line_set_value(yellow_lines[i], yellow) < 0) return -1;
    if (gpiod_line_set_value(green_lines[i], green) < 0) return -1;
    return 0;
}

int set_all_red(struct gpiod_chip *chip) {
    (void)chip;
    for (int i = 0; i < ROAD_COUNT; i++) {
        if (gpiod_line_set_value(red_lines[i], 1) < 0) return -1;
        if (gpiod_line_set_value(yellow_lines[i], 0) < 0) return -1;
        if (gpiod_line_set_value(green_lines[i], 0) < 0) return -1;
    }
    return 0;
}

int read_sensor(struct gpiod_chip *chip, const struct road *r) {
    (void)chip;
    int value = gpiod_line_get_value(sensor_lines[r->id - 1]);
    if (value < 0) return -1;

    /* Most LM393-style IR modules are LOW when an object is detected. */
    return value == 0;
}

int calculate_priority(int detected) {
    return detected ? HIGH_PRIORITY : NO_TRAFFIC;
}

int choose_road(void) {
    static int last = -1;
    int best = -1;
    int best_priority = NO_TRAFFIC + 1;

    for (int offset = 1; offset <= ROAD_COUNT; offset++) {
        int i = (last + offset) % ROAD_COUNT;
        if (roads[i].priority < best_priority) {
            best_priority = roads[i].priority;
            best = i;
        }
    }

    if (best >= 0) last = best;
    return best;
}

static void update_sensors(struct gpiod_chip *chip) {
    pthread_mutex_lock(&state_mutex);
    for (int i = 0; i < ROAD_COUNT; i++) {
        int detected = read_sensor(chip, &roads[i]);
        if (detected >= 0) {
            roads[i].detected = detected;
            roads[i].priority = calculate_priority(detected);
        }
    }
    pthread_mutex_unlock(&state_mutex);
}

static void print_status(void) {
    printf("Traffic: ");
    for (int i = 0; i < ROAD_COUNT; i++)
        printf("R%d=%s(P%d) ", roads[i].id, roads[i].detected ? "YES" : "NO", roads[i].priority);
    printf("\n");
}

static void *sensor_thread(void *arg) {
    struct gpiod_chip *chip = arg;
    while (running) {
        update_sensors(chip);
        sleep_ms(SENSOR_SAMPLE_MS);
    }
    return NULL;
}

int main(void) {
    struct gpiod_chip *chip = NULL;
    pthread_t sensor_tid;

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    printf("==============================================\n");
    printf(" Smart Traffic Signal - Priority Scheduling\n");
    printf(" Raspberry Pi + Ubuntu Linux + C\n");
    printf("==============================================\n");

    if (gpio_init(&chip) < 0) {
        fprintf(stderr, "GPIO initialization failed. Check wiring and libgpiod.\n");
        gpio_cleanup(chip);
        return EXIT_FAILURE;
    }

    set_all_red(chip);

    if (pthread_create(&sensor_tid, NULL, sensor_thread, chip) != 0) {
        perror("pthread_create");
        gpio_cleanup(chip);
        return EXIT_FAILURE;
    }

    while (running) {
        pthread_mutex_lock(&state_mutex);
        print_status();
        int selected = choose_road();
        pthread_mutex_unlock(&state_mutex);

        if (selected < 0) {
            sleep_ms(500);
            continue;
        }

        struct road selected_road = roads[selected];
        printf("Scheduler -> Road %d selected (priority %d)\n", selected_road.id, selected_road.priority);

        pthread_mutex_lock(&state_mutex);
        set_all_red(chip);
        set_signal(chip, &selected_road, 0, 0, 1);
        pthread_mutex_unlock(&state_mutex);
        printf("Road %d: GREEN\n", selected_road.id);

        for (int s = 0; s < GREEN_SECONDS && running; s++) sleep_ms(1000);

        if (!running) break;

        pthread_mutex_lock(&state_mutex);
        set_signal(chip, &selected_road, 0, 1, 0);
        pthread_mutex_unlock(&state_mutex);
        printf("Road %d: YELLOW\n", selected_road.id);

        for (int s = 0; s < YELLOW_SECONDS && running; s++) sleep_ms(1000);

        pthread_mutex_lock(&state_mutex);
        set_all_red(chip);
        pthread_mutex_unlock(&state_mutex);
        printf("Road %d: RED\n\n", selected_road.id);
    }

    pthread_join(sensor_tid, NULL);
    pthread_mutex_lock(&state_mutex);
    set_all_red(chip);
    pthread_mutex_unlock(&state_mutex);
    gpio_cleanup(chip);
    pthread_mutex_destroy(&state_mutex);

    printf("System stopped safely. All roads are RED.\n");
    return EXIT_SUCCESS;
}
