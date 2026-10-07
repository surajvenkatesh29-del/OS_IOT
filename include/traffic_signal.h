#ifndef TRAFFIC_SIGNAL_H
#define TRAFFIC_SIGNAL_H

#include <gpiod.h>
#include <pthread.h>
#include <signal.h>

#define ROAD_COUNT 4
#define HIGH_PRIORITY 1
#define MEDIUM_PRIORITY 2
#define LOW_PRIORITY 3
#define NO_TRAFFIC 4

#define GREEN_SECONDS 7
#define YELLOW_SECONDS 2
#define SENSOR_SAMPLE_MS 250

struct road {
    int id;
    unsigned int sensor_gpio;
    unsigned int red_gpio;
    unsigned int yellow_gpio;
    unsigned int green_gpio;
    int detected;
    int priority;
};

extern volatile sig_atomic_t running;
extern pthread_mutex_t state_mutex;

int gpio_init(struct gpiod_chip **chip);
void gpio_cleanup(struct gpiod_chip *chip);
int set_signal(struct gpiod_chip *chip, const struct road *r, int red, int yellow, int green);
int set_all_red(struct gpiod_chip *chip);
int read_sensor(struct gpiod_chip *chip, const struct road *r);
int calculate_priority(int detected);
int choose_road(void);
void sleep_ms(unsigned int ms);

#endif
