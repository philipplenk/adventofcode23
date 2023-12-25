#!/bin/bash

echo "Fetching input for day ${1} of year ${YEAR}, do this only once please..."
curl -A "philipp.lenk@codemetas.de via curl" -b session="${SESSION_KEY}" https://adventofcode.com/${YEAR}/day/${1}/input > input
