#!/bin/sh
#release our translations
case $1 in
-c)
make distclean
rm l10n/*.qm
cd libwinegame
make distclean
cd ..
exit 0;;
#*)
#echo "USAGE: $0 [-c or no options to install winegame]"
#exit -1;;
esac

lrelease winegame.pro
#go to winegame lib...
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