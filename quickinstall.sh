#!/bin/sh
cd libwinegame
qmake
make
sudo make install
cd ..
qmake
make
sudo make install