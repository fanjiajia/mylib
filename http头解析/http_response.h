#ifndef __http_response_h__
#define __http_response_h__
#include "http_parser.h"
#include <stdio.h>
#include <memory>

class HttpResponse {
public:
    HttpResponse() {
        parser.data = this;
        http_parser_init(&parser, HTTP_RESPONSE);
        settings.on_message_begin = on_message_begin;
        settings.on_header_field = on_header_field;
        settings.on_header_value = on_header_value;
        settings.on_url = on_request_url;
        settings.on_status = on_response_status;
        settings.on_body = on_body;
        settings.on_headers_complete = on_headers_complete;
        settings.on_message_complete = on_message_complete;
        settings.on_chunk_header = chunk_header_cb;
        settings.on_chunk_complete = chunk_complete_cb;
    }
    virtual ~HttpResponse() {
    }
    int parser_execute(const char *data, size_t len) {
        return http_parser_execute(&parser, &settings, data, len);
    }
private:
    static int on_message_begin(http_parser *parser);
    virtual int _on_message_begin(http_parser *) {
        return 0;
    }

    static int on_request_url(http_parser *parser, const char *at, size_t length);
    virtual int _on_request_url(http_parser *, const char *, size_t) {
        return 0;
    }

    static int on_response_status(http_parser *parser, const char *at, size_t length);
    virtual int _on_response_status(http_parser *, const char *, size_t) {
        return 0;
    }

    static int on_header_field(http_parser *parser, const char *at, size_t length);
    virtual int _on_header_field(http_parser *, const char *, size_t) {
        return 0;
    }
    
    static int on_header_value(http_parser *parser, const char *at, size_t length);
    virtual int _on_header_value(http_parser *, const char *, size_t) {
        return 0;
    }

    static int on_headers_complete(http_parser *parser);
    virtual int _on_headers_complete(http_parser *) {
        return 0;
    }

    static int on_body(http_parser *parser, const char *at, size_t length);
    virtual int _on_body(http_parser *, const char *, size_t) {
        return 0;
    }

    static int on_message_complete(http_parser *parser);
    virtual int _on_message_complete(http_parser *) {
        return 0;
    }

    static int chunk_header_cb(http_parser *parser);
    virtual int _chunk_header_cb(http_parser *)
    {
        return 0;
    }

    static int chunk_complete_cb(http_parser *parser);
    virtual int _chunk_complete_cb(http_parser *)
    {
        return 0;
    }

public:
    http_parser parser;
    http_parser_settings settings;
};

#endif
