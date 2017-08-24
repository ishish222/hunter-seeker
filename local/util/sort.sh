#!/bin/sh

MAIN_DIR="/home/ish/malware_malware_malware"
ARCHIVE=$MAIN_DIR"/_ARCHIVE"
BACKUP=$MAIN_DIR"/_BACKUP"
SAMPLE=$1

SHA1=$(sha1sum $SAMPLE | cut -d' ' -f1)
echo $SHA1
echo $MAIN_DIR
echo $ARCHIVE

FIRST_LETTER=$(echo $SHA1 | head -c 1)
SECOND_LETTER=$(echo $SHA1 | head -c 2 | tail -c 1)
echo $FIRST_LETTER
echo $SECOND_LETTER

mkdir -p $ARCHIVE
mkdir -p $BACKUP
mkdir -p $ARCHIVE"/"$FIRST_LETTER
mkdir -p $ARCHIVE"/"$FIRST_LETTER"/"$SECOND_LETTER
cp $SAMPLE $BACKUP"/"$SAMPLE
mv $SAMPLE $ARCHIVE"/"$FIRST_LETTER"/"$SECOND_LETTER"/"$SHA1
ln -s $ARCHIVE"/"$FIRST_LETTER"/"$SECOND_LETTER"/"$SHA1 $SAMPLE
