#!/bin/bash
echo "початок збірки"
mkdir -p build
cd build
cmake ..
make
echo "збірка завершена"