#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <string>
#include <queue>
#include <tuple>
#include <regex>
#include <map>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/dns.h>

#include "parse_buffer.h"

std::queue<std::tuple<std::string, int>> url_queue;

std::map<std::string, int> url_history;

static void get_something(struct event_base *base, std::string url, int deep_size);

std::regex m_re("\"[a-zA-z0-9]+://[^\\s]*/\"");

typedef struct _connect_ctx_
{
    std::string uri;
    int port;
    std::string host;
    std::string url;
    std::string m_readbuffer;
    std::string m_sendbuffer;
    struct event_base *base;
    struct evdns_base *dns_base;
    int deep_size;
}connect_ctx_t;

static std::tuple<std::string, std::string, int> pasrse_url(std::string url)
{
    std::string host;
    std::string uri = "/";
    int port = 80;

    std::size_t found = url.find("://");
    if (found == std::string::npos)
    {
        host = url;
    }
    else
    {
        if (found + 3 >= url.length()) return std::make_tuple(host, uri, port);
        host = url.substr(found + 3);
    }

    std::size_t m_pos = host.find("/");
    if (m_pos == std::string::npos)
    {
        m_pos = host.find("?");
        if (m_pos != std::string::npos)
        {
            uri += host.substr(m_pos);
            host = host.substr(0, m_pos);
        }
    }
    else
    {
        uri = host.substr(m_pos);
        host = host.substr(0, m_pos);

        m_pos = host.find(":");
        if (m_pos != std::string::npos)
        {
            port = stoi(host.substr(m_pos + 1));
            host = host.substr(0, m_pos);
        }
    }
    return std::make_tuple(host, uri, port);
}

static void analysis_web(std::string &body, int deep_size, std::string m_url)
{
    std::smatch result;
    auto pos = body.cbegin();
    auto end = body.cend();

    for (; std::regex_search(pos, end, result, std::regex("href=\"(https?:)?//\\S+\"")); pos = result.suffix().first)
    {
        std::string url = std::regex_replace(result[0].str(), std::regex("href=\"(https?:)?//(\\S+)\""), "$2");
        if (url.find("css") != std::string::npos) continue;
        if (url.find(".ico") != std::string::npos) continue;
        auto it = url_history.find(url);
        if (it == url_history.end())
        {
            url_queue.push(std::make_tuple(url, deep_size++));
            url_history.insert(std::make_pair(url, deep_size));
        }
    }

    std::smatch res;
    pos = body.cbegin();
    end = body.cend();
    for (; std::regex_search(pos, end, res, m_re); pos = res.suffix().first)
    {
        std::string tmp = res[0].str();
        if (tmp.find("ed2k") == std::string::npos || tmp.find("Game.of.Thrones.S0") == std::string::npos) continue; 
        printf("match:%s\n", tmp.c_str());
    }
}

static void deal_once(struct event_base *base)
{
    url_queue.pop();
    if (url_queue.empty())
    {
        printf("url_queue is empty()!\n");
        return;
    }
    auto once_cycle = url_queue.front();
    std::string url;
    int deep_size;
    std::tie(url, deep_size) = once_cycle;
    get_something(base, url, deep_size);
}

static void do_read_cb_(struct bufferevent *bev, void *ctx)
{
    auto pctx = (connect_ctx_t *)ctx;
    while (true)
    {
        char buff[1024] = { '\0' };
        size_t ncount = bufferevent_read(bev, buff, sizeof(buff));
        if (ncount <= 0) break;
        pctx->m_readbuffer.append(buff, ncount);
    }

    std::shared_ptr<ParseHttpBuffer> parser_resp = std::make_shared<ParseHttpBuffer>(HTTP_RESPONSE);
    parser_resp->parser_execute(pctx->m_readbuffer.c_str(), pctx->m_readbuffer.length());
    if (parser_resp->iscomplete)
    {
        bufferevent_free(bev);
        evdns_base_free(pctx->dns_base, 0);

        analysis_web(parser_resp->http_body, pctx->deep_size, pctx->url);

        deal_once(pctx->base);
        delete pctx;
    }
}

static void do_event_cb_(struct bufferevent *bev, short what, void *ptr)
{
    auto pctx = (connect_ctx_t *)ptr;

    if (what & BEV_EVENT_CONNECTED)
    {
        pctx->m_sendbuffer += std::string("GET ") + pctx->uri + std::string(" HTTP/1.1\r\n");
        if (pctx->port != 80)
            pctx->m_sendbuffer += std::string("Host: ") + pctx->host + std::string(":") + std::to_string(pctx->port) + std::string("\r\n");
        else
            pctx->m_sendbuffer += std::string("Host: ") + pctx->host + std::string("\r\n");
        pctx->m_sendbuffer += "Accept: */*\r\n";
        pctx->m_sendbuffer += "Connection: close\r\n\r\n";
        auto output = bufferevent_get_output(bev);
        evbuffer_add(output, pctx->m_sendbuffer.c_str(), pctx->m_sendbuffer.length());
    }
    else if (what & BEV_EVENT_EOF)
    {
        //printf("do_event_cb_: BEV_EVENT_EOF\n%s%s", pctx->m_sendbuffer.c_str(), pctx->m_readbuffer.c_str());
        bufferevent_free(bev);
        evdns_base_free(pctx->dns_base, 0);
        deal_once(pctx->base);
        delete pctx;
    }
    else if (what & BEV_EVENT_ERROR)
    {
        //printf("DNS error: %s host[%s]\n", evutil_gai_strerror(bufferevent_socket_get_dns_error(bev)), pctx->host.c_str());
        bufferevent_free(bev);
        evdns_base_free(pctx->dns_base, 0);
        deal_once(pctx->base);
        delete pctx;
    }
    else
    {
        bufferevent_free(bev);
        evdns_base_free(pctx->dns_base, 0);
        deal_once(pctx->base);
        delete pctx;
    }
}

static void get_something(struct event_base *base, std::string url, int deep_size)
{
    if (deep_size > 20)
    {
        deal_once(base);
        return;
    }

    printf("url: %s deep: %d size: %ld\n", url.c_str(), deep_size, url_queue.size());
    connect_ctx_t *m_ctx = new connect_ctx_t;
    std::tie(m_ctx->host, m_ctx->uri, m_ctx->port) = pasrse_url(url);
    m_ctx->base = base;
    m_ctx->deep_size = deep_size;
    m_ctx->url = url;

    if (!m_ctx->host.empty())
    {
        auto dns_base = evdns_base_new(base, 1);
        m_ctx->dns_base = dns_base;

        auto bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
        struct timeval timeout = { 10, 0 };
        bufferevent_set_timeouts(bev, &timeout, &timeout);
        bufferevent_setcb(bev, do_read_cb_, NULL, do_event_cb_, m_ctx);
        bufferevent_enable(bev, EV_READ | EV_WRITE);

        bufferevent_socket_connect_hostname(bev, dns_base, AF_UNSPEC, m_ctx->host.c_str(), m_ctx->port);
    }
    else
    {
        deal_once(m_ctx->base);
        delete m_ctx;
    }
}

int main(int argc, char **argv)
{
    auto base = event_base_new();
    
    std::string start = "www.meijutt.com";
    url_queue.push(std::make_tuple(start, 1));
    url_history.insert(std::make_pair(start, 1));

    auto once_cycle = url_queue.front();
    std::string url;
    int deep_size;
    std::tie(url, deep_size) = once_cycle;
    get_something(base, url, deep_size);

    event_base_dispatch(base);
    event_base_free(base);
    return 1;
}
