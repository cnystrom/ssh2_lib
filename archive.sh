#!/bin/sh

echo
echo BE SURE TO RESET PASSWORDS FIRST!
echo

. ../build.sh

FILENAME=ssh2_lib

TAR_FILE=${FILENAME}.${VERSION}_${BUILD}_${DATE}.tar

make clean

cd ..

tar cvf ${TAR_FILE} ${FILENAME}

gzip ${TAR_FILE}

scp ${TAR_FILE}.gz olenska.newio.org:/export/home/cnystrom/ftp/d0-libs

