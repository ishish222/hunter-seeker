#!/bin/sh

LOCAL_ROOT=../
TARGET=$1
TARGETS_DIR=../../targets/
TARGET_DIR=$TARGETS_DIR$TARGET
TARGET_CONFIGS_DIR=$LOCAL_ROOT/target-configs/

echo "cleaning"
rm -rf $TARGET_DIR

mkdir -p $TARGET_DIR/server
mkdir -p $TARGET_DIR/client
mkdir -p $TARGET_DIR/common
mkdir -p $TARGET_DIR/samples/shared
mkdir -p $TARGET_DIR/samples/saved
mkdir -p $TARGET_DIR/samples/other
mkdir -p $TARGET_DIR/samples/binned
mkdir -p $TARGET_DIR/samples/original

echo "server"
cat $TARGET_CONFIGS_DIR$TARGET | grep s: | cut -d":" -f2 |
while read line;
do
    target_line=$(basename $line)
    echo "cp -r $LOCAL_ROOT$line $TARGET_DIR/server/$target_line"
    cp -Lr $LOCAL_ROOT$line $TARGET_DIR/server/$target_line
done
    
echo "client"
cat $TARGET_CONFIGS_DIR$TARGET | grep c: | cut -d":" -f2 |
while read line;
do
    target_line=$(basename $line)
    echo "cp -r $LOCAL_ROOT$line $TARGET_DIR/client/$target_line"
    cp -Lr $LOCAL_ROOT$line $TARGET_DIR/client/$target_line
done

echo "settings"
cat $TARGET_CONFIGS_DIR$TARGET | grep t: | cut -d":" -f2 |
while read line;
do
    target_line=$(basename $line)
    echo "cp -r $LOCAL_ROOT$line $TARGET_DIR/settings/$target_line"
    cp -Lr $LOCAL_ROOT$line $TARGET_DIR/common/$target_line
done

echo "original samples"
cat $TARGET_CONFIGS_DIR$TARGET | grep o: | cut -d":" -f2 |
while read line;
do
    target_line=$(basename $line)
    echo "cp -r $LOCAL_ROOT$line $TARGET_DIR/samples/original/$target_line"
    cp -Lr $LOCAL_ROOT$line $TARGET_DIR/samples/original/$target_line
done

echo "Finished deploying"
