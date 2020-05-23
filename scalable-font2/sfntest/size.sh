#!/bin/sh

# get the symbol table, grep for ssfn stuff, convert more spaces to one with
# while+read, then add the 3rd coloumn (size), finally display the last sum
function getsize
{
  readelf -s $1 | grep ssfn_$2 | while read line; do
    sum=$[$sum+`echo $line | cut -d ' ' -f 3`]
    echo $sum bytes
  done | tail -1
}

echo -n "Simple renderer: "
getsize sfntest1 putc

echo -n "Normal renderer: "
getsize sfntest2

