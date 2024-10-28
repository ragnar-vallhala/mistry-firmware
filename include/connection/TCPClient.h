#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>
#include "pico/stdlib.h"
#include "lwip/tcp.h"

class TCPClient {
public:
    TCPClient(const char* server_ip, uint16_t port);
    ~TCPClient();

    bool connect();
    void disconnect();
    bool send(const char* data, size_t length);
    bool receive(char* buffer, size_t buffer_length, size_t& received_length);
    void poll();

private:
    static err_t tcp_connected(void* arg, struct tcp_pcb* tpcb, err_t err);
    static err_t tcp_client_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err);
    static void tcp_client_err(void* arg, err_t err);

    struct tcp_pcb* tcp_pcb_;
    ip_addr_t remote_addr_;
    uint16_t port_;
    bool connected_;
    char recv_buffer[1024]; // Buffer for incoming data
    uint32_t recv_length;
};

#endif // TCP_CLIENT_H

