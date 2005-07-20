#!/bin/bash

if [ ! -d data ];
then
	echo "Cannot find data/ directory";
	exit 1;
fi

rm -f data/teachers.csv

for file in `ls --color=never data/courses_xml/*.xml`;
do
	./parser_prof $file >> data/teachers.csv;
done
