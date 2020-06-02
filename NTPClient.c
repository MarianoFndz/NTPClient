#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define  NTP_TIMESTAMP_DELTA 2208988800ul

#define LI(packet) (uint8_t) ((packet.li_vn_mode & 0xC0) >> 6)
#define  VN(packet) (uint8_t) ((packet.li_vn_mode & 0x38) >> 3)
#define MODE(packet) (uint8_t) ((packet.li_vn_mode & 0x07) >> 0)

void error (char* msg)
{
	perror(msg);

	exit (0);
}

int main (int argc, char* argv[ ]) {
	int sockfd;//Variable socket
	int portno = 123;//Puerto UDP

	struct ntp_packet{
		uint8_t li_vn_mode;
		uint8_t stratum;
		uint8_t poll;
		uint8_t precision;
		uint32_t rootDelay;
		uint32_t rootDispersion;
		uint32_t refId;
		uint32_t refTm_s;
		uint32_t refTm_f;
		uint32_t origTm_s;
		uint32_t origTm_f;
		uint32_t rxTm_s;
		uint32_t rxTm_f;
		uint32_t txTm_s;
		uint32_t txTm_f;
	} packet; //Creacion del paquete NTP

	memset(&packet, 0, sizeof(packet)); //Inicializa las variables de packet en 0

	packet.li_vn_mode = 27;//Leap, Version y Modos, en este caso cliente. 27 es la suma

	struct sockaddr_in serv_addr;//Creacion de la estructura sockaddr_in que contiene la informacion del servidor

	char ip[]= "127.0.0.1"; //IP del servidor, en este caso es Loopback

	sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );//Creacion del socket

	if( sockfd <0)
	{	error("ERROR abriendo el socket");}

	bzero( (char*) &serv_addr, sizeof(serv_addr));//Completa con 0 la estructura

	serv_addr.sin_addr.s_addr = inet_addr(ip);//Asigna la IP con formato inet_addr

	serv_addr.sin_family = AF_INET; //Asignamos el protocolo de la conexion en este caso IPV4

	serv_addr.sin_port = htons(portno);//Asignamos el puerto con formato htons

	time_t t= time(NULL); //time(NULL) devuelve horario actual

	packet.txTm_s = htonl( NTP_TIMESTAMP_DELTA +  t ); //Sumamos nuetro horario el horario de unix

 	sendto(sockfd, (struct ntp_packet*)&packet, sizeof(packet), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));//Enviamos el packet

	recvfrom(sockfd, (struct ntp_packet*)&packet, sizeof(packet), 0, (struct sockaddr*)&serv_addr, (socklen_t *)sizeof(serv_addr));//Recibimos el packet

	printf("Tiempo de envio de el cliente: %d \n", packet.origTm_s );
	printf("Tiempo cuando recibio el paquete el servidor: %d \n", packet.rxTm_s );
	printf("Tiempo cuando envio el paquete el servidor: %d \n", packet.txTm_s );

	packet.txTm_s = ntohl(packet.txTm_s);	//Convertimos a ntohl
	packet.rxTm_s = ntohl(packet.rxTm_s);	//""
	time_t recibido = (time_t)(packet.rxTm_s - NTP_TIMESTAMP_DELTA);//Le restamos el horario unix que le sumamos antes y mostramos el horario que recibimos el packet

	time_t time = (time_t)(packet.txTm_s -  NTP_TIMESTAMP_DELTA);//""
	printf("Tiempo: %s\n", ctime((const time_t*)&time));//Mostramos nuestro horario devuelto
	printf("Tiempo cuando recibio el paquete: %s\n", ctime((const time_t*)&recibido));//Mostramos el horario cuando el servidor recibio el packet
	return 0;
}

//Fernandez, Mariano
//Farfaglia, Bautista	
