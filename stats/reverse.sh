#!/bin/bash

for name in `cat count.txt | awk '{ print($2) }' | sort -u`; 
do 
	echo "$name : " `host $name`; 
done 
