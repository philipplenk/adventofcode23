#!/bin/bash

id=${PWD##*/}
id=$(echo $id | awk '{print $1 + 0}' )

PART=$1
RESULT=$2

echo "Submitting result ${RESULT} for part ${PART} of day ${id} of year ${YEAR}..."
curl -X POST --data "level=${PART}" --data "answer=${RESULT}" -b session="${SESSION_KEY}" https://adventofcode.com/${YEAR}/day/${id}/answer | grep answer
