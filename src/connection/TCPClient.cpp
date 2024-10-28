#include "connection/TCPClient.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"

TCPClient::TCPClient(const char* server_ip, uint16_t port)
    : port_(port), connected_(false),recv_length(0) {
    ip4addr_aton(server_ip, &remote_addr_);
    tcp_pcb_ = tcp_new();
}

TCPClient::~TCPClient() {
    disconnect();
}

bool TCPClient::connect() {
    if (!tcp_pcb_) {
        return false;
    }

    tcp_arg(tcp_pcb_, this);
    tcp_err(tcp_pcb_, tcp_client_err);  // Set error callback
 tcp_recv(tcp_pcb_, tcp_client_recv); // Set receive callback
    err_t err = tcp_connect(tcp_pcb_, &remote_addr_, port_, tcp_connected);
    return (err == ERR_OK);
}

void TCPClient::disconnect() {
    if (tcp_pcb_) {
        tcp_close(tcp_pcb_);
        tcp_pcb_ = nullptr;
        connected_ = false;
    }
}

bool TCPClient::send(const char* data, size_t length) {
    if (!connected_ || !tcp_pcb_) {
        return false;
    }

    err_t err = tcp_write(tcp_pcb_, data, length, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        return false;
    }
    
    tcp_output(tcp_pcb_); // Send the data immediately
    return true;
}

bool TCPClient::receive(char* buffer, size_t buffer_length, size_t& received_length) {
    if (!connected_ || !tcp_pcb_) {
        return false; // Not connected or invalid TCP PCB
    }
    printf("Recived %d\n", recv_length);
    // Check if there is data in the internal buffer
    if (recv_length) {
        // Determine how much data to copy to the buffer
        size_t bytes_to_copy = (buffer_length < recv_length) ? buffer_length : recv_length;

        // Copy the data from internal buffer to the provided buffer
        memcpy(buffer, recv_buffer, bytes_to_copy);

        // Update the received length
        received_length = bytes_to_copy;

        // Update the internal buffer by shifting remaining data down
        memmove(recv_buffer, recv_buffer + bytes_to_copy, recv_length - bytes_to_copy);
        recv_length -= bytes_to_copy;

        return true; // Data was successfully copied
    }
    
    received_length = 0; // No data received
    return false; // No data available
}



void TCPClient::poll() {
    if (tcp_pcb_) {
        // Polling logic here (if necessary)
    }
}

err_t TCPClient::tcp_connected(void* arg, struct tcp_pcb* tpcb, err_t err) {
    TCPClient* client = static_cast<TCPClient*>(arg);
    if (err == ERR_OK) {
        client->connected_ = true;
    }
    return err;
}

err_t TCPClient::tcp_client_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err) {
    TCPClient* client = static_cast<TCPClient*>(arg);
    if (!p) {
        // Connection closed
        return tcp_close(tpcb);
    }
    
    // Directly copy data from the pbuf to the internal buffer
    size_t total_len = p->tot_len;
    size_t available_space = sizeof(client->recv_buffer) - client->recv_length;

    // Limit the total length to the available space to avoid overflow
    if (total_len > available_space) {
        total_len = available_space; // Avoid overflow
    }

    // Copy data directly into the internal buffer
    size_t copied_length = 0;
    for (struct pbuf* q = p; q != NULL; q = q->next) {
        size_t copy_len = (total_len < q->len) ? total_len : q->len;
        memcpy(client->recv_buffer + copied_length, q->payload, copy_len);
        copied_length += copy_len;
        total_len -= copy_len;

        if (total_len == 0) break; // Stop if we've copied enough
    }

    // Update the received length
    client->recv_length = copied_length;
    // Acknowledge the received data
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p); // Free the received pbuf
    return ERR_OK;
}

void TCPClient::tcp_client_err(void* arg, err_t err) {
    TCPClient* client = static_cast<TCPClient*>(arg);
    client->connected_ = false; // Mark as disconnected on error
    // Handle error logging or other error processing here if necessary
}

