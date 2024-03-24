#!/bin/bash

# source $PWD/rsc/sh/color.sh

source $PWD/rsc/sh/handle_sigint.sh

all_check() {
	ipcs | grep -e "0x" | grep "666" | cut -d ' ' -f 2 > /dev/null
	if [ $? -ne 0 ]; then
		display_color_msg ${RED} "Leak found after clean."
		free_ipcs ${IPCS_OPT} ${MSG}
	else
		display_color_msg ${GREEN} "No leak after clear."
	fi

}

free_ipcs() {
	IPCS_OPT="$1"
	MSG="$2"
	local ipcs_id=$(ipcs ${IPCS_OPT} | grep -v dest | grep 666 | cut -d ' ' -f 2 | tr '\n' ' ')
	# echo  DATA ${ipcs_id}
	if [ "${ipcs_id}" != "" ]; then
		# ID2=$(ipcs ${IPCS_OPT} | grep -v dest | grep 666 | cut -d ' ' -f 2 | tr '\n' ' ' | cut -d ' ' -f 1)
		ID=$(ipcs ${IPCS_OPT} | grep -v dest | grep 666 | cut -d ' ' -f 2 | tr '\n' ' ' | awk '{print $1}')
		# echo id = ${ID}
		# echo id 2 = ${ID2}
		display_color_msg ${YELLOW} "Clear ${ipcs_id} ${MSG} leak found."
		ipcrm ${IPCS_OPT} ${ID}
		free_ipcs ${IPCS_OPT} ${MSG}
	else
		display_color_msg ${GREEN} "Nothing to clean no ${MSG} leak found."
	fi
}

# Hard coded for Kali Linux same for 2 maybe use awk instead of cut in ips free DONE need to test on kali pc
# check_kali() {
# 	local os_version=$(uname -n | cut -d ' ' -f 1)
# 	if [ ${os_version} != "" ]; then
# 		if [ ${os_version} == "Kali" ]; then
# 			FREE_ARGS="3"
# 			display_color_msg ${YELLOW} "${os_version} detected."
# 		fi
# 	fi

# }
# FREE_ARGS="2"
# check_kali

send_sigint_all
sleep 2
free_ipcs -m "Shared memory"
free_ipcs -s "Semaphore"
free_ipcs -q "Message queue"
all_check
