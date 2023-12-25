#!/bin/bash

id=${PWD##*/}
id=$(echo $id | awk '{print $1 + 0}' )

fetch_input.sh $id
copy_from.sh base

cat ${BASE_DIR}/tools/kitty_session.conf | kitty --session -
