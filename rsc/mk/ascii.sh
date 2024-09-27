#!/bin/bash

source rsc/sh/color.sh

ascii_ft_ping() {

    echo -e "\n"
    echo -e "  ${LIGHT_CYAN}███████  ████████           ██████   ██  ███    ██   ██████  ${RESET}"
    echo -e "  ${LIGHT_CYAN}██          ██              ██   ██  ██  ████   ██  ██       ${RESET}"
    echo -e "  ${LIGHT_CYAN}█████       ██              ██████   ██  ██ ██  ██  ██   ███ ${RESET}"
    echo -e "  ${LIGHT_CYAN}██          ██              ██       ██  ██  ██ ██  ██    ██ ${RESET}"
    echo -e "  ${LIGHT_CYAN}██          ██    ███████   ██       ██  ██   ████   ██████  ${RESET}\n\n"
}

if [ "$1" == "ft_ping" ]; then
    ascii_ft_ping
fi

