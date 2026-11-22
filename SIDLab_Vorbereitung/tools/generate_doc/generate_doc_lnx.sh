#!/bin/bash
rm -rf ../../doc/html/
mkdir ../../doc/html/
mkdir ../../doc/html/src/
cp -R ../../src/. ../../doc/html/src/
cp -R ../../doc/coverage/ ../../doc/html/
if dpkg-query -W -f'${db:Status-Abbrev}\n' doxygen 2>/dev/null \
 | grep -q '^.i $'; then
    doxygen --version
else
    echo 'ERROR: Doxygen must be installed first...'
    echo "sudo apt-get install doxygen"
    sudo apt-get install doxygen
fi
echo " "
echo "########## DOXYGEN MESSAGES ##########"
cd ./doxygen/
doxygen ./doxygen_config.Doxyfile 
echo "########## END DOXYGEN MESSAGES ##########"
echo " "
echo "Check the above Doxygen messages!"
echo "If there's any error, NO new HTML documentation will be generated."
echo "In that case, fix the error, then repeat."
echo " "
