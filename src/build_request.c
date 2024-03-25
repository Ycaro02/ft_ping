#include "../include/ft_ping.h"



t_ping_packet build_ping_packet(char *data, size_t size)
{
    t_ping_packet packet = {0};

    packet.iphdr.version = 4;
    packet.iphdr.ihl = 5;
    packet.iphdr.tos = 0;
    packet.iphdr.tot_len = htons(sizeof(t_ping_packet));
    packet.iphdr.id = htons(0);
    packet.iphdr.frag_off = 0;
    packet.iphdr.ttl = 255;
    packet.iphdr.protocol = IPPROTO_ICMP;
    packet.iphdr.check = 0;
    packet.iphdr.saddr = inet_addr("127.0.0.1");
    packet.iphdr.daddr = inet_addr("192.160.200.10");


    packet.icmphdr.type = ICMP_ECHO;
    packet.icmphdr.code = 0;
    packet.icmphdr.checksum = 0;
    packet.icmphdr.un.echo.id = getpid();
    packet.icmphdr.un.echo.sequence = 0;
    packet.icmphdr.checksum = 0;
    ft_memcpy(packet.data, data, size);
    ft_printf_fd(1, "Size of packet struct %u\n", sizeof(t_ping_packet));

    return (packet);
}
