#!/bin/bash
# convert raw IR input on the form of <+number -number...> to binary 
# Assumes 1500 to be LOW, 500 to be HIGH
# Assumes pulses are always constant (arround 500), and values are in spaces

# Usage: convert_from_binary.sh [FILE]
# If no file is informed, reads from stdin

# read from stdin
if [ "$#" -lt 1 ]
then
  cat - | sed -E 's/(-[0-9]+)\s/\1\n/g ; s/\+[0-9]+\s//g' $FILE  | tr -d '-' | awk '{
    if ($1 > 1000 && $1 < 2000) {
      print 1;
    } else if ($1 < 1000){
    print 0;
    }
    if ((NR -1) % 4 == 0 && NR -1 != 0)
      print " ";
  }' | tr -d '\n' 
else
  # read from file
  FILE=$1

  if [ ! -f $FILE ]
  then
    echo "file does not exist"
    exit 1
  fi

  FILE_BASE=$(basename $FILE)
  sed -E 's/(-[0-9]+)\s/\1\n/g ; s/\+[0-9]+\s//g' $FILE  | tr -d '-' | awk '{
    if ($1 > 1000 && $1 < 2000) {
      print 1;
    } else if ($1 < 1000){
    print 0;
    }
    if ((NR -1) % 4 == 0 && NR -1 != 0)
      print " ";
  }' | tr -d '\n' 
  echo " # ${FILE_BASE%.*}"
fi
