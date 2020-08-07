#include "tcp.h"

#include <arpa/inet.h>		//inet_pton()
#include <stdarg.h>		//va_list
#include <stdio.h>		//vsnprintf()
#include <netinet/in.h>		//sockaddr_in
#include <string.h>		//memmove()
#include <sys/socket.h>		//socket()
#include <unistd.h>		//close()

struct tcp tcp =
{
	.server = -1,
	.client = -1,
};

ssize_t tcp_server_recv()
{
	ssize_t received = recv(tcp.client, tcp.input.data_end, tcp.input.buffer_end - tcp.input.data_end, MSG_NOSIGNAL);
	if(received > 0)
	{
		tcp.input.data_end += received;
		if(tcp.input.data_end == tcp.input.buffer_end && tcp.input.data != tcp.input.buffer)
		{
			memmove(tcp.input.buffer, tcp.input.data, tcp.input.data_end - tcp.input.data);
			tcp.input.data_end -= tcp.input.data - tcp.input.buffer;
			tcp.input.data = tcp.input.buffer;
		}
	}
	return received;
}

ssize_t tcp_server_send()
{
	ssize_t sent = send(tcp.client, tcp.output.data, tcp.output.data_end - tcp.output.data, MSG_NOSIGNAL);
	tcp.output.data += sent;
	if(tcp.output.data == tcp.output.data_end)
	{
		tcp.output.data = tcp.output.data_end = tcp.output.buffer;
	}
	return sent;
}

int tcp_server_init(char const * address, int port)
{
	struct sockaddr_in server_address = { 0 };
	struct in_addr binary_address;
	
	if(inet_pton(AF_INET, address, &(binary_address)) < 1) return -1;
	if((tcp.server = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;

	int reuse = 1;
	if (setsockopt(tcp.server, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) return -1;

	#ifdef SO_REUSEPORT
	if (setsockopt(tcp.server, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) return -1;
	#endif

	server_address.sin_family = AF_INET;
	server_address.sin_addr = binary_address;
	server_address.sin_port = htons(port);
	
	if(bind(tcp.server, (struct sockaddr *) &server_address, sizeof(server_address))) return -1;
	if(listen(tcp.server, 1)) return -1;

	tcp.input.buffer_end = tcp.input.buffer + sizeof(tcp.input.buffer);
	tcp.input.data = tcp.input.buffer;
	tcp.input.data_end = tcp.input.buffer;
	
	tcp.output.buffer_end = tcp.output.buffer + sizeof(tcp.output.buffer);
	tcp.output.data = tcp.output.buffer;
	tcp.output.data_end = tcp.output.buffer;
	
	return 0;
}

int tcp_server_deinit()
{
	close(tcp.server);
	tcp.server = -1;
	close(tcp.client);
	tcp.client = -1;

	return 0;
}

void tcp_server_start(void (* message_handler)(char *, int))
{
	for(;;)
	{
		tcp.client = accept(tcp.server, (struct sockaddr *) NULL, (socklen_t *) NULL);
		for(;;)
		{
			ssize_t received = tcp_server_recv();
			if(received <= 0) break;
			for(char * scan = tcp.input.data + 2; scan <= tcp.input.data_end; ++scan)
			{
				if(scan[-1] == '\n' && scan[-2] == '\n')
				{
					message_handler(tcp.input.data, scan - tcp.input.data);
					tcp.input.data = scan;
					++scan;
				}
			}
			if(tcp.output.data != tcp.output.data_end)
			{
				ssize_t sent = tcp_server_send();
				if(sent < 0) break;
			}
		}
		close(tcp.client);
		tcp.input.data = tcp.input.buffer;
		tcp.input.data_end = tcp.input.buffer;
		tcp.output.data = tcp.output.buffer;
		tcp.output.data_end = tcp.output.buffer;
	}
}

int tcp_server_printf(char const * format, ...)
{
	va_list args;
	va_start(args, format);
	int printed = vsnprintf(tcp.output.data_end, tcp.output.buffer_end - tcp.output.data_end, format, args);
	tcp.output.data_end += printed;
	va_end(args);
	return printed;
}
