# Smart Traffic Signal Using Priority Scheduling

A Raspberry Pi + Ubuntu Linux project that demonstrates **Operating Systems priority scheduling** using a four-road traffic signal model.

Vehicles are detected using IR sensors. The application assigns traffic priority and schedules one road at a time using Linux threads, synchronization, and GPIO control.

> **Important:** This project implements an application-level traffic scheduler. It does not replace or modify the Linux kernel CPU scheduler.

## Features

- 4-road traffic junction
- Automatic vehicle detection using IR sensors
- Priority-based traffic scheduling
- Red / Yellow / Green LED control
- POSIX threads (`pthread`)
- Mutex synchronization
- Raspberry Pi GPIO using `libgpiod`
- Safe all-red transition between roads
- Online hardware-independent simulation
- Configurable green/yellow timing
- Clean GitHub project structure

## Hardware

- Raspberry Pi 4 (or compatible Pi)
- Ubuntu Linux for Raspberry Pi
- 4 x IR obstacle/vehicle sensors
- 12 x LEDs: 4 red, 4 yellow, 4 green
- 12 x 220Ω resistors
- Breadboard
- Jumper wires
- Suitable Raspberry Pi power supply

### GPIO safety

Raspberry Pi GPIO is **3.3V logic**. Do not connect a 5V sensor output directly to a GPIO pin. Use a 3.3V-compatible IR sensor configuration or an appropriate level shifter.

Each LED must have a series resistor. Do not connect an LED directly to a GPIO pin.

## GPIO Map (BCM numbering)

| Road | IR Sensor | Red | Yellow | Green |
|---|---:|---:|---:|---:|
| Road 1 | GPIO20 | GPIO4 | GPIO17 | GPIO27 |
| Road 2 | GPIO21 | GPIO22 | GPIO23 | GPIO24 |
| Road 3 | GPIO26 | GPIO25 | GPIO5 | GPIO6 |
| Road 4 | GPIO19 | GPIO12 | GPIO13 | GPIO16 |

Use a common Raspberry Pi GND for the sensor/LED circuit.

## Software requirements

On Ubuntu/Raspberry Pi Linux:

```bash
sudo apt update
sudo apt install -y gcc make libgpiod-dev gpiod
```

Check the GPIO chip:

```bash
gpiodetect
gpioinfo
```

## Build and run on Raspberry Pi

```bash
git clone <YOUR_GITHUB_REPO_URL>
cd smart-traffic-signal
make
sudo ./bin/traffic_signal
```

Stop with `Ctrl+C`.

The program starts in a safe all-red state and continuously evaluates the four roads.

## Simulation (works without Raspberry Pi GPIO)

The simulation is included so the scheduling logic can be tested on a normal PC or online C compiler.

```bash
gcc simulation/traffic_simulation.c -o traffic_simulation
./traffic_simulation
```

The simulation uses keyboard-entered traffic levels and prints the scheduler decisions.

## Scheduling logic

Each road is treated as a traffic task. The scheduler chooses the highest-priority road among roads with detected traffic.

Priority values:

- High traffic: `1` (highest priority)
- Medium traffic: `2`
- Low traffic: `3`
- No traffic: `4`

If multiple roads have the same priority, the scheduler uses round-robin tie breaking so one road does not permanently dominate the junction.

## OS concepts demonstrated

1. **Threads** – sensor monitoring and scheduler execution.
2. **Mutex synchronization** – prevents simultaneous signal-state changes.
3. **Scheduling** – application-level priority scheduling.
4. **Critical section** – traffic light state updates are protected.
5. **Waiting / ready / running behavior** – traffic roads move between conceptual scheduling states.
6. **Linux environment** – the project runs as a normal Linux application on Ubuntu.

## Project structure

```text
smart-traffic-signal/
├── README.md
├── LICENSE
├── Makefile
├── .gitignore
├── include/
│   └── traffic_signal.h
├── src/
│   └── traffic_signal.c
├── simulation/
│   └── traffic_simulation.c
├── docs/
│   ├── WIRING.md
│   ├── PROJECT_REPORT.md
│   └── GITHUB.md
└── scripts/
    └── install_dependencies.sh
```

## Notes about IR sensors

One IR sensor per road is enough to demonstrate **vehicle presence**, but it does not accurately count an arbitrary number of vehicles. For a physical density-counting model, use multiple sensors per road or a more suitable counting method.

For a simple academic prototype, the sensor's detected/not-detected state is converted into a traffic level using configurable sampling logic.

## Troubleshooting

### `fatal error: gpiod.h: No such file or directory`

You are either compiling on a normal online compiler or the Raspberry Pi package is missing.

On Ubuntu Raspberry Pi:

```bash
sudo apt install libgpiod-dev
```

For an online compiler, use the hardware-independent program in `simulation/` instead.

### `main.cpp` appears in the online compiler error

That means the online compiler is treating the source as C++. The hardware version is intended for a Raspberry Pi Linux environment. The simulation is portable C and can be compiled on a PC.

## Future enhancements

- Emergency vehicle priority
- Multiple IR sensors per road for vehicle counting
- OLED/LCD status display
- Web dashboard
- Logging to CSV/SQLite
- Dynamic green-light duration based on queue length
- Manual maintenance mode

## Author

**V. Suraj**

Academic project — Operating Systems / Embedded Linux
