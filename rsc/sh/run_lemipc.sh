#!/bin/bash

source ./rsc/sh/color.sh
LEMIPC="./lemipc"
VAL_LOG="valgrind.log"
VALGRIND="valgrind --leak-check=full --track-fds=yes --log-file=${VAL_LOG}"
LEMIPC_DISPLAY="./lemipc_display"
SLEEP_VAL=0.02



handle_opt() {
	if [ -z "${1}" ]; then
		display_color_msg ${RED} "Usage: ${0} <VALGRIND_BOOLEAN_OPT>"
		exit 1
	fi

	if [ "${1}" == "1" ]; then
		LEMIPC="${VALGRIND} ${LEMIPC}"
		LEMIPC_DISPLAY="${VALGRIND} ${LEMIPC_DISPLAY}"
		SLEEP_VAL=0.1
	fi


	if [ ! -f "${LEMIPC}" ]; then
		make -s
	fi
}

display_valgrind_log() {
	if [ -f "${VAL_LOG}" ]; then
		display_color_msg ${GREEN} "Valgrind log found."
		cat ${VAL_LOG}
		rm ${VAL_LOG}
	fi
}


run_test() {
	
	./rsc/mk/ascii.sh "tester"
	# rm_pid_log ${PID_LOG}

	for i in {0..49}
	do
		local team_id=$(((i % 49) + 1))
		display_color_msg ${GREEN} "Lauching number ${i} team ${team_id} ..."
		if [ ${i} -eq 3 ]; then
			display_color_msg ${YELLOW} "Lauch display handler ${LEMIPC_DISPLAY} ..."
			${LEMIPC_DISPLAY} &
			DISPLAY_PID=$!
		fi
		${LEMIPC} ${team_id} &
		sleep ${SLEEP_VAL}
	done

	# wait all children pid
	display_color_msg ${GREEN} "Waiting all children to finish ..."
	wait $(jobs -p)
	display_valgrind_log
	# display_color_msg ${RED} "Killing all processes ..."
	# ./rsc/sh/check_ipcs_free.sh
}

handle_opt "$@"
run_test
