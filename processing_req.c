














int receive_msg(int client_sock, char* buffer, int buf_size, struct HttpRequest *request) {
    int bytes_read = read(client_sock, buffer, buf_size);
    if (bytes_read == 0) {
        printf("Client disconnected\n");
        close(client_sock);
        return 0;
    }
    if (bytes_read < 0) {
        perror("Read error");
        return -1;
    }
    
    buffer[bytes_read] = '\0';

    printf("Received: %s\n", buffer);
    *request = parse_request(buffer);
    close(client_sock);
    return bytes_read;
}