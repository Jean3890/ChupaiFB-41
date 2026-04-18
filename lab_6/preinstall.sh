#!/bin/bash
echo "встановлення залежностей для OpenCV та збірки C++"
sudo apt update
sudo apt install -y build-essential cmake g++ libopencv-dev
echo "залежності успішно встановлено"