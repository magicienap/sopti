#!/bin/bash

if [ ! -d data ];
then
	echo "Cannot find data/ directory";
	exit 1;
fi

for file in `ls --color=never data/courses_xml/*.xml`;
do
	./parser_prof $file > data/teachers.csv;
done
