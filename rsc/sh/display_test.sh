#!/bin/bash

FIRST="first_pid.txt"
SECOND="second_pid.txt"

# this clear all lemipc pid when sourced
source ./rsc/sh/handle_sigint.sh


solo_exec() {
	local file="${1}"
	${LEMIPC} "${2}" &
	local loc_pid=$!
	echo ${loc_pid} > ${file}
	sleep 0.5
}

clear_exec() {
	local file="${1}"
	if [ -f "${file}" ]; then
		local pid=$(cat ${file})
		if [ "${pid}" != "" ]; then
			send_sigint ${pid}
			rm  ${file}
		fi
	fi
}

if [ "$1" == "rm" ]; then
	clear_exec ${FIRST}
	sleep 0.5
	clear_exec ${SECOND}
	sleep 0.5
else
	source ./rsc/sh/color.sh
	clear_exec ${FIRST}
	clear_exec ${SECOND}
	solo_exec ${FIRST} 1
	solo_exec ${SECOND} 2
	display_color_msg ${YELLOW} "Lauch display handler ..."
	./lemipc_display
fi