#include "../include/ft_ping.h"


static void compute_average_time(t_ping_sum *summary, t_list *time_lst)
{
	suseconds_t sum = 0;
	uint32_t nb_elem = 0;
	if (!time_lst) {
		// ft_printf_fd(1, "No time list\n");
		return;
	}
	for (t_list *current = time_lst; current; current = current->next) {
		sum += *(suseconds_t *)current->content;
		++nb_elem;
	}
	summary->average = sum / nb_elem;
}

static void compute_standard_deviation(t_ping_sum *summary, t_list *time_lst)
{
	suseconds_t diff = 0;
	suseconds_t sum = 0;
	uint32_t nb_elem = 0;
	
	if (!time_lst) {
		return;
	}
	for (t_list *current = time_lst; current; current = current->next) {
		diff = (*(suseconds_t *)current->content) - summary->average;
		sum += (diff * diff);
		++nb_elem;
	}
	summary->stddev = sqrt(sum / nb_elem);
}

void display_clear_summary(t_context *c)
{
	char *name = c->dest.name ? c->dest.name : inet_ntoa(*(struct in_addr *)&c->dest.sockaddr.sin_addr.s_addr);
	uint32_t packet_loss = 0;
	
	if (c->summary.nb_send == 0) {
		return;
	}
	packet_loss = 100 - ((c->summary.nb_rcv * 100) / c->summary.nb_send);
	compute_average_time(&c->summary, c->summary.rcv_time_lst);
	compute_standard_deviation(&c->summary, c->summary.rcv_time_lst);
	ft_printf_fd(1, CYAN"--- %s ping statistics ---\n"RESET, name);
	ft_printf_fd(1, "%u packets transmitted, %u packets received, %u%% packet loss\n", c->summary.nb_send, c->summary.nb_rcv, packet_loss);
	ft_printf_fd(1, "round-trip "GREEN"min"RESET"/"YELLOW"avg"RESET"/"RED"max"RESET"/"CYAN"stddev"RESET" = ", c->summary.min, c->summary.average, c->summary.max, c->summary.stddev);
	display_ms_time(GREEN, c->summary.min, 0);
	display_ms_time(YELLOW ,c->summary.average, 0);
	display_ms_time(RED, c->summary.max, 0);
	display_ms_time(CYAN, c->summary.stddev, 1);
	// round-trip min/avg/max/stddev = 0.849/1.249/2.561/0.661 ms
}
