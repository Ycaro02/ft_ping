#!/bin/bash

source ./rsc/sh/handle_sigint.sh

VAL_LOG="valgrind.log"

VALGRIND="valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes --log-file=${VAL_LOG}" 

PING_CMD="./ft_ping"

run_valgrind_test() {
    local args="$@"
    display_color_msg ${YELLOW} "Running ${VALGRIND} ${PING_CMD}|${args}|"
    ${VALGRIND} ${PING_CMD} "$@" > /dev/null 2>&1 &
    wait_and_display_log ${VAL_LOG} memcheck-amd64- 10
}

run_valgrind_test "$@"
