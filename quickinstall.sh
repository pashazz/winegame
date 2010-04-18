#!/bin/sh
cd libwinegame
qmake
make
echo "Installing winegame library libwinestuff.so"
sudo make install
cd ..
qmake
make
echo "Installing WineGame"
sudo make install