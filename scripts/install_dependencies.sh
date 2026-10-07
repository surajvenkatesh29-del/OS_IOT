#!/bin/bash
set -e
sudo apt update
sudo apt install -y gcc make libgpiod-dev gpiod
printf '\nDependencies installed. Check GPIO with: gpiodetect && gpioinfo\n'
