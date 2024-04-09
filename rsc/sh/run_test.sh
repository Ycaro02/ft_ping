#!/bin/bash

source ./rsc/sh/handle_sigint.sh

PING_LOG="ft_ping.log"

PING_CMD="./ft_ping"

run_ping_test() {
    local args="$@"
	display_color_msg ${YELLOW} "Running ping test ${PING_CMD} |${args}|"
	${PING_CMD} "$@" > ${PING_LOG} 2>&1 &
	wait_and_display_log ${PING_LOG} ft_ping 10
}

run_ping_test "$@"
