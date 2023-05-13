#!/bin/bash
clear
echo "---------------"
echo "time ./serial"
echo
time ./serial >> timing.txt
echo "---------------"

echo
echo "time ./parallel"
echo
time ./parallel >> timing.txt
echo "---------------"
echo
