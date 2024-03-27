#include "client_com.h"

void read_cb(struct bufferevent *bev, void *ctx) {
    char buf[1024];
    int n;
    while ((n = bufferevent_read(bev, buf, sizeof(buf))) > 0) {
        fwrite(buf, 1, n, stdout);
    }
}

void event_cb(struct bufferevent *bev, short events, void *ctx) {
    if (events & BEV_EVENT_CONNECTED) {
        printf("Connected to server.\n");
    } else if (events & (BEV_EVENT_ERROR | BEV_EVENT_EOF)) {
        struct event_base *base = (struct event_base *)ctx;
        if (events & BEV_EVENT_ERROR) {
            int err = bufferevent_socket_get_dns_error(bev);
            if (err)
                printf("DNS error: %s\n", evutil_gai_strerror(err));
            printf("Connection closed.\n");
        }
        bufferevent_free(bev);
        event_base_loopexit(base, NULL);
    }
}