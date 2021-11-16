/* WAVE Short Message Protocol Send - IEEE 1609
 * Copyright (C) 2018 Billy Kihei <billy@reachrf.com>,
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include <errno.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
//#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <syslog.h>
#include <netpacket/packet.h>
#include <pthread.h>

#define MY_DEST_MAC0	0xFF
#define MY_DEST_MAC1	0xFF
#define MY_DEST_MAC2	0xFF
#define MY_DEST_MAC3	0xFF
#define MY_DEST_MAC4	0xFF
#define MY_DEST_MAC5	0xFF

#define ETHER_TYPE	0x88DC

#define DEFAULT_IF	"wlp1s0"
#define BUF_SIZ		100
//1518

int main(int argc, char *argv[])
{
	pthread_t thread_id;
	printf("Starting...");
	pthread_create(&thread_id, NULL, receive, NULL);
	pthread_join(thread_id, NULL);
	printf("Received");
	exit(0);
}

int *receive(){
	int sockfd, ret, i;
	ssize_t numbytes;
	struct sockaddr saddr;
	int saddr_len = sizeof (saddr);
	uint8_t buf[BUF_SIZ];
	char ifName[IFNAMSIZ];
	
	strcpy(ifName, DEFAULT_IF);

	/* Header structures */
	struct ether_header *eh = (struct ether_header *) buf;

	/* Open AF_PACKET socket, listening for EtherType ETHER_TYPE */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
		perror("error listener: socket");	
		return -1;
	}

	/* Bind to device */
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

repeat:	/*printf("listener: Waiting to recvfrom...\n")*/;
	numPkt++;
	printf("RX: %i\n", numPkt);
	numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, &saddr, (socklen_t *)&saddr_len);
	//printf("listener: got packet %lu bytes\n", numbytes);

	struct ethhdr *eth = (struct ethhdr *)(buf);
//printf("\nEthernet Header\n");
//printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
//printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
//printf("\t|-Protocol : %.2X\n",eth->h_proto);
	
	i=sizeof(struct ethhdr);
i++;
i+=3;
i+=3;
i+=3;
i++;
i+=2;
i+=1;

	/*printf("\t\t|Version: %d\n", buf[i]); i++;
	printf("\t\t|PSID: %d\n", buf[i]); i+=3;
	printf("\t\t|Channel: %d\n", buf[i]); i+=3;
	printf("\t\t|Data Rate: %.1f\n", (double)(buf[i]/2)); i+=3;
	printf("\t\t|Tx Power: %d\n", buf[i]); i++;
	printf("\t\t|WAVE Element ID: %d\n", buf[i]); i++;

	printf("\t\t|WSM Length: %d\n", ((int)buf[i])+((int)buf[i+1]));* 
	i+=2;
	
	//i+=9; //Skipping other items...
	
	
	//struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));*/
//print_BSM(buf,numbytes,i);
done:	goto repeat;

	close(sockfd);
	return ret;
}
int send(){
	int sockfd;
	struct ifreq if_idx;
	struct ifreq if_mac;
	int tx_len = 0;
	char sendbuf[BUF_SIZ];
	struct ether_header *eh = (struct ether_header *) sendbuf;
	struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
	struct sockaddr_ll socket_address={0};
	char ifName[IFNAMSIZ];
	
	strcpy(ifName, DEFAULT_IF);

	//openlog("v2vsend",(LOG_NDELAY||LOG_PID),LOG_USER);
	//syslog(LOG_MAKEPRI(LOG_USER,LOG_INFO),"Open RAW socket to send on");
	/* Open RAW socket to send on */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
	    perror("socket");
	}
	//closelog();

	//openlog("v2vsend",(LOG_NDELAY||LOG_PID),LOG_USER);
	//syslog(LOG_MAKEPRI(LOG_USER,LOG_INFO),"Get the index of the interface to send on ");
	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
	printf("%s\n",if_idx.ifr_name);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
	    perror("SIOCGIFINDEX");


	//closelog();
	
	//openlog("v2vsend",(LOG_NDELAY||LOG_PID),LOG_USER);
	//syslog(LOG_MAKEPRI(LOG_USER,LOG_INFO),"Get the MAC address of the interface to send on");
	/* Get the MAC address of the interface to send on */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
	    perror("SIOCGIFHWADDR");


	//closelog();


	//openlog("v2vsend",(LOG_NDELAY||LOG_PID),LOG_USER);
	//syslog(LOG_MAKEPRI(LOG_USER,LOG_INFO),"Construct the Ethernet header");
	/* Construct the Ethernet header */
	memset(sendbuf, 0, BUF_SIZ);
	/* Ethernet header */
	eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	eh->ether_dhost[0] = MY_DEST_MAC0;
	eh->ether_dhost[1] = MY_DEST_MAC1;
	eh->ether_dhost[2] = MY_DEST_MAC2;
	eh->ether_dhost[3] = MY_DEST_MAC3;
	eh->ether_dhost[4] = MY_DEST_MAC4;
	eh->ether_dhost[5] = MY_DEST_MAC5;
	/* Ethertype field */
	eh->ether_type = htons(ETHER_TYPE);
	tx_len += sizeof(struct ether_header);

	//closelog();

	//openlog("v2vsend",(LOG_NDELAY||LOG_PID),LOG_USER);
	//syslog(LOG_MAKEPRI(LOG_USER,LOG_INFO),"Packet data");
	/* Packet data */
   	sendbuf[tx_len++] = 0x02;
	sendbuf[tx_len++] = 0x20; 
	sendbuf[tx_len++] = 0x0f; 
	sendbuf[tx_len++] = 0x01;
	sendbuf[tx_len++] = 0xac;  
	sendbuf[tx_len++] = 0x10; 
	sendbuf[tx_len++] = 0x01; 
	sendbuf[tx_len++] = 0x0c; 
	sendbuf[tx_len++] = 0x04; 
	sendbuf[tx_len++] = 0x01; 
	sendbuf[tx_len++] = 0x01; 
	sendbuf[tx_len++] = 0x80; 
	sendbuf[tx_len++] = 0x00; 
	sendbuf[tx_len++] = 0x31; 
	sendbuf[tx_len++] = 0x02;  
	sendbuf[tx_len++] = 0x00; 
	sendbuf[tx_len++] = 0x30; 
	sendbuf[tx_len++] = 0x2d;
   	sendbuf[tx_len++] = 0x80; 
	sendbuf[tx_len++] = 0x01; 
	sendbuf[tx_len++] = 0x02; 
	sendbuf[tx_len++] = 0x81; 
	sendbuf[tx_len++] = 0x26; 

	sendbuf[tx_len++] = 0x00; //msgCount
 
	sendbuf[tx_len++] = 0x11; 
	sendbuf[tx_len++] = 0x22; 
	sendbuf[tx_len++] = 0x33; 
	sendbuf[tx_len++] = 0x44; // temp ID

	sendbuf[tx_len++] = 0x87; 
	sendbuf[tx_len++] = 0x28; // secondMark

	sendbuf[tx_len++] = 0x35; 
	sendbuf[tx_len++] = 0xa4; 
	sendbuf[tx_len++] = 0xe9; 
	sendbuf[tx_len++] = 0x00; // lat

	sendbuf[tx_len++] = 0x5f; 
	sendbuf[tx_len++] = 0x5e;
	sendbuf[tx_len++] = 0xe1; 
	sendbuf[tx_len++] = 0x00; // long 

	sendbuf[tx_len++] = 0x00; 
	sendbuf[tx_len++] = 0x00; // elev

	sendbuf[tx_len++] = 0xff; 
	sendbuf[tx_len++] = 0xff; 
	sendbuf[tx_len++] = 0xff; 
	sendbuf[tx_len++] = 0xff; // accuracy

	sendbuf[tx_len++] = 0x00; 
	sendbuf[tx_len++] = 0x00; // Transmisison Speed
 
	sendbuf[tx_len++] = 0x00; 
	sendbuf[tx_len++] = 0x00; // heading

	sendbuf[tx_len++] = 0x00; // Steering Wheel Angle

	sendbuf[tx_len++] = 0xff;
   	sendbuf[tx_len++] = 0xff; // accelsetlong

	sendbuf[tx_len++] = 0x07; 
	sendbuf[tx_len++] = 0xd1; // accelsetlat

	sendbuf[tx_len++] = 0x81; // accelsetvert

	sendbuf[tx_len++] = 0x7f; 
	sendbuf[tx_len++] = 0xff; // accelsetyaw

	sendbuf[tx_len++] = 0x08;
	sendbuf[tx_len++] = 0x00; // brakes
 
	sendbuf[tx_len++] = 0x07; 
	sendbuf[tx_len++] = 0x80; 
	sendbuf[tx_len++] = 0x1e; // tx size

	sendbuf[tx_len++] = 0xa3; 
	sendbuf[tx_len++] = 0x00;

	//63bytes
	//closelog();


	//openlog("v2vsend",(LOG_NDELAY||LOG_PID),LOG_USER);
	//syslog(LOG_MAKEPRI(LOG_USER,LOG_INFO),"Index of the network device");
	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;

	//closelog();

	//openlog("v2vsend",(LOG_NDELAY||LOG_PID),LOG_USER);
	//syslog(LOG_MAKEPRI(LOG_USER,LOG_INFO),"Address length");
	/* Address length*/
	socket_address.sll_halen = ETHER_ADDR_LEN;
	//closelog();

	socket_address.sll_family = AF_PACKET;
	socket_address.sll_protocol = htons(ETHER_TYPE);

	//openlog("v2vsend",(LOG_NDELAY||LOG_PID),LOG_USER);
	//syslog(LOG_MAKEPRI(LOG_USER,LOG_INFO),"Destination MAC");
	/* Destination MAC */
	socket_address.sll_addr[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	socket_address.sll_addr[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	socket_address.sll_addr[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	socket_address.sll_addr[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	socket_address.sll_addr[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	socket_address.sll_addr[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	const unsigned char ether_broadcast_addr[] = {0xff,0xff,0xff,0xff,0xff,0xff};
	memcpy(socket_address.sll_addr,ether_broadcast_addr,ETHER_ADDR_LEN);
	//closelog();
	printf("test2");
	printf("%d\n",tx_len);

	//printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eh->ether_shost[0],eh->ether_shost[1],eh->ether_shost[2],eh->ether_shost[3],eh->ether_shost[4],eh->ether_shost[5]);
	//printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eh->ether_dhost[0],eh->ether_dhost[1],eh->ether_dhost[2],eh->ether_dhost[3],eh->ether_dhost[4],eh->ether_dhost[5]);

	//openlog("v2vsend",(LOG_NDELAY||LOG_PID),LOG_USER);
	//syslog(LOG_MAKEPRI(LOG_USER,LOG_INFO),"Send packet!");

	int pktSend = 0;	
	printf("test");
	


	void* sender(){
		do {
	/* Send packet */
		if (sendto(sockfd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr*)&socket_address, sizeof(socket_address)) < 0)
		{	
			return 0;
			perror("Bad Packet\n");
		}else{
			pktSend++;
			printf("TX: %i\n", pktSend);
		}
	
	}while(pktSend> 0);
	}

	void* receive(){
		repeat:	/*printf("listener: Waiting to recvfrom...\n")*/;
		numPkt++;
	printf("RX: %i\n", numPkt);
	numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, &saddr, (socklen_t *)&saddr_len);
	//printf("listener: got packet %lu bytes\n", numbytes);

	struct ethhdr *eth = (struct ethhdr *)(buf);
//printf("\nEthernet Header\n");
//printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
//printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
//printf("\t|-Protocol : %.2X\n",eth->h_proto);
	
	i = sizeof(struct ethhdr);
i++;
i+=3;
i+=3;
i+=3;
i++;
i+=2;
i+=1;

	/*printf("\t\t|Version: %d\n", buf[i]); i++;
	printf("\t\t|PSID: %d\n", buf[i]); i+=3;
	printf("\t\t|Channel: %d\n", buf[i]); i+=3;
	printf("\t\t|Data Rate: %.1f\n", (double)(buf[i]/2)); i+=3;
	printf("\t\t|Tx Power: %d\n", buf[i]); i++;
	printf("\t\t|WAVE Element ID: %d\n", buf[i]); i++;

	printf("\t\t|WSM Length: %d\n", ((int)buf[i])+((int)buf[i+1]));* 
	i+=2;
	
	//i+=9; //Skipping other items...
	
	
	//struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));*/
//print_BSM(buf,numbytes,i);
done:	goto repeat;
	}

	//closelog();
	//return 0;
}

