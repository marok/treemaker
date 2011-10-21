#!/bin/bash

 sum=0
 for i in `find . -name '*.h' -type f`; do
    a=`wc -l $i | cut -f 1 -d ' '`
    sum=`expr $sum + $a`
 done

 for i in `find . -name '*.cpp' -type f`; do
    a=`wc -l $i | cut -f 1 -d ' '`
    sum=`expr $sum + $a`
 done

 echo "Treemaker: "$sum" lines of code"