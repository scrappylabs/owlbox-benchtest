/* WAVE Short Message Protocol Recv - IEEE 1609
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

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

#define DEST_MAC0	0xFF
#define DEST_MAC1	0xFF
#define DEST_MAC2	0xFF
#define DEST_MAC3	0xFF
#define DEST_MAC4	0xFF
#define DEST_MAC5	0xFF

#define ETHER_TYPE	0x88DC

#define DEFAULT_IF	"wlp1s0"
#define BUF_SIZ		65536
int numPkt = -1;

void print_BSM(uint8_t *buf, ssize_t length, int i);

int main(int argc, char *argv[])
{
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

void print_BSM(uint8_t *buf, ssize_t numbytes, int i) {
	//Need to figure out the blob start and end we print out the tx_VID portion hoping to find it in the stack. Once we have a reference, we can decode the rest of the data in the blob.

	
	/* Print packet */
	//printf("\tData:");
	//for (int i=sizeof(struct ethhdr); i<numbytes; i++) printf("%02x:", buf[i]);
	//printf("\n");

	
	uint32_t txtemp_var, txtemp_var2;

	printf("BSM Packet \n");
	printf("______________________________\n\n");

	//MsgCount
	uint8_t msgCnt;
	i+=9;
	printf("\t msgCnt: %d \n", buf[i]); i++;

	//TemporaryID
	uint8_t temp_id_1, temp_id_2, temp_id_3, temp_id_4;
	printf("\t tempID: %.2X-%.2X-%.2X-%.2X \n", buf[i],buf[i+1],buf[i+2],buf[i+3]); i+=4;

	//DSecond
	int secMark;
	secMark += buf[i] << 8;
	secMark += buf[i+1];
	printf("\t secMark: %d seconds \n\n", secMark/1000); i+=2;

	printf("____PositionLocal3D____\n\n");

	//Latitude
	long int txlatitude_val = 0;
        txlatitude_val += buf[i] << 24;
	txlatitude_val += buf[i+1] << 16;
	txlatitude_val += buf[i+2] << 8;
	txlatitude_val += buf[i+3];
	//printf("\t lat hex: %.2X-%.2X-%.2X-%.2X \n", buf[i],buf[i+1],buf[i+2],buf[i+3]);
	printf("\t lat: %ld deg %c \n", txlatitude_val, (txlatitude_val < 0)? 'S' : 'N'); i+=4;

	//Longitude
	long int txlongitude_val;
	txlongitude_val += buf[i] << 24;
	txlongitude_val += buf[i+1] << 16;
	txlongitude_val += buf[i+2] << 8;
	txlongitude_val += buf[i+3];
	printf("\t long: %ld deg %c \n", txlongitude_val, (txlongitude_val < 0)? 'W' : 'E'); i+=4;

	//Elevation
	uint8_t elevation_val_1, elevation_val_2;
	elevation_val_1 = buf[i]; i++;
	elevation_val_2 = buf[i]; i++;
	printf("\t elev: %d meters\n", (elevation_val_2 & 0x00FF)/10);

	//PositionalAccuracy
	long int accuracy;
	accuracy += buf[i] << 24;
	accuracy += buf[i+1] << 16;
	accuracy += buf[i+2] << 8;
	accuracy += buf[i+3];
	printf("\t accuracy: %ld \n\n", accuracy); i+=4;
	

	printf("____Motion____\n\n");

	//TransmissionAndSpeed
	uint8_t transmission_speed_1, transmission_speed_2;
	transmission_speed_1 = buf[i]; i++;
	transmission_speed_2 = buf[i]; i++;

	/*transmission_speed[0] = (uint8_t)(((uint32_t)(wsmgps.speed*50) & 0xFF00) >> 8);
	transmission_speed[1] = (uint8_t)(((uint32_t)(wsmgps.speed*50) & 0x00FF));
	transmission_speed[0] = transmission_speed[0] | 0xE0;*/

	printf("\t speed: %d m/s \n", (transmission_speed_2&0x00FF)/50);

	//Heading
	int heading_val;
	heading_val += buf[i] << 8;
	heading_val += buf[i+1];
	printf("\t heading: %d \n", heading_val); i+=2;

	//SteeringWheelAngle
	uint8_t stw;
	printf("\t angle: %d \n", buf[i]); i++;

	//AccelerationSet4Way
	int accelsetlong, accelsetlat, accelsetyaw;
	uint8_t accelsetvert;
	accelsetlong += buf[i] << 8;
	accelsetlong += buf[i+1]; i+=2;

	accelsetlat += buf[i] << 8;
	accelsetlat += buf[i+1]; i+=2;

	accelsetvert = buf[i]; i++;

	accelsetyaw += buf[i] << 8;
	accelsetyaw += buf[i+1]; i+=2;

	printf("\t accelSet\n");
	printf("\t\t longitude acceleration: %d \n", accelsetlong);
	printf("\t\t latitude acceleration: %d \n", accelsetlat);
	printf("\t\t vertical acceleration: %d \n", accelsetvert);
	printf("\t\t yaw: %d \n\n", accelsetyaw);

	printf("____Control____\n\n");

	//BrakeSystemStatus
	int brakes;
	brakes += buf[i] << 8;
	brakes += buf[i+1]; i+=2;
	printf("\t brakes: %d \n\n", brakes);

	printf("____VehicleBasic____\n\n");

	//TX_SIZE
	uint8_t txvsize[3];
	txvsize[0] = buf[i];
	txvsize[1] = buf[i+1];
	txvsize[2] = buf[i+2];
	printf("\t size\n");
	printf("\t\t width: %d cm \n",txvsize[0]);
	printf("\t\t length: %d cm \n\n",txvsize[2]); i+=3;

}
