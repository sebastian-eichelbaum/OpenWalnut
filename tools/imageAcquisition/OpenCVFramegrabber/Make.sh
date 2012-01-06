#!/bin/bash
g++ main.cpp -I/opt/local/include -L/opt/local/lib -lopencv_highgui -I/usr/local/include/igtl/ -L/usr/local/lib/igtl -lOpenIGTLink -o OpenCVFramegrabber

