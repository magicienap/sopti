#!/bin/bash

basedir=$(dirname $0)
pushd $basedir

pushd data; ./update; popd
pushd db; ./updatedb.pl; popd
./update_courses_xml.sh
./make_teachers_csv.sh
pushd db; ./updatedb.pl; popd

popd
