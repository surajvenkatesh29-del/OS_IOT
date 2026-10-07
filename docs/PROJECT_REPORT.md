# Mini Project Summary

## Title
Smart Traffic Signal Using Priority Scheduling

## Objective
To design an automatic traffic signal system on a Raspberry Pi that demonstrates Operating Systems scheduling concepts by giving higher service priority to roads with greater traffic demand.

## Problem
A fixed-time traffic signal can waste green time on roads with little or no traffic while another road has a queue.

## Proposed solution
IR sensors detect vehicle presence. The application assigns a priority to each road and selects the highest-priority road. Linux threads continuously monitor sensors while a mutex protects traffic-light state changes.

## OS concepts
- Priority scheduling
- Thread creation
- Synchronization using mutexes
- Critical sections
- Ready/running/waiting states as a scheduling model
- Linux system environment

## Algorithm
1. Initialize GPIO and set all signals RED.
2. Start a sensor-monitoring thread.
3. Read all four road sensors.
4. Calculate road priorities.
5. Select the highest-priority road.
6. Set the selected road GREEN and all others RED.
7. After the green interval, change the selected road to YELLOW.
8. Return it to RED.
9. Re-evaluate traffic and repeat.
10. On shutdown, force all roads RED.

## Expected result
The road with detected traffic is selected for service. The signal changes safely through GREEN -> YELLOW -> RED, while Linux synchronization prevents conflicting GPIO updates.

## Limitation
One sensor per road demonstrates presence rather than exact vehicle counting. Multiple sensors can be added for true queue/density estimation.
