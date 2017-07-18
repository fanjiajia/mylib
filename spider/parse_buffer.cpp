#include "parse_buffer.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <memory>
using namespace std;

int ParseHttpBuffer::_on_message_begin(http_parser *parser)
{
    //printf("_on_message_begin\n");
    return 0;
}
int ParseHttpBuffer::_on_header_field(http_parser *parser, const char *at, size_t len)
{
    //printf("_on_header_field\n");
    //ParseHttpBuffer *self = (ParseHttpBuffer*)parser->data;
    //std::string header_field = std::string(at).substr(0, len);
    //printf("header_field: %s\n", header_field.c_str());
    return 0;
}
int ParseHttpBuffer::_on_header_value(http_parser *parser, const char *at, size_t len)
{
    //printf("_on_header_value\n");
    return 0;
}

int ParseHttpBuffer::_on_request_url(http_parser *parser, const char *at, size_t len)
{
    //printf("_on_request_url\n");
    return 0;
}

int ParseHttpBuffer::_on_response_status(http_parser *parser, const char *at, size_t len)
{
    //printf("_on_response_status\n");
    response_status = std::string(at).substr(0, len);
    return 0;
}

int ParseHttpBuffer::_on_body(http_parser *parser, const char *at, size_t len)
{
    //printf("_on_body\n");
    http_body = std::string(at).substr(0, len);
    return 0;
}

int ParseHttpBuffer::_on_headers_complete(http_parser *parser)
{
    //printf("_on_headers_complete\n");
    return 0;
}

int ParseHttpBuffer::_on_message_complete(http_parser *parser)
{
    //printf("_on_message_complete\n");
    iscomplete = true;
    return 0;
}


int ParseHttpBuffer::_chunk_header_cb(http_parser *parser)
{
    //printf("_chunk_header_cb\n");
    return 0;
}


int ParseHttpBuffer::_chunk_complete_cb(http_parser *parser)
{
    //printf("_chunk_complete_cb\n");
    return 0;
}

#if 0
int main(void)
{
    string resp("HTTP/1.1 200 OK\r\n"
            "Server: nginx/1.10.3\r\n"
            "Date: Thu, 20 Apr 2017 01:14:12 GMT\r\n"
            "Content-Type: image/jpeg\r\n"
            "Content-Length: 4\r\n"
            "Last-Modified: Mon, 17 Apr 2017 04:29:57 GMT\r\n"
            "ETag: \"58f444c5-4\"\r\n"
            "Accept-Ranges: bytes\r\n"
            "Accept-Ranges: bytes\r\n"
            "Age: 3569\r\n"
            "Connection: keep-alive\r\n"
            "Via: CNC-HBTS-C-213-217 (DLC-3.0)\r\n\r\n"
            "1234");

    ParseHttpBuffer *parse_resp = new ParseHttpBuffer(HTTP_RESPONSE);
    size_t parsed = parse_resp->parser_execute(resp.c_str(), resp.length());
    printf("parsed: %d len: %d\n", parsed, resp.length());
    if (parsed == resp.length())
    {
        printf("parsed ok, status: %s\t%d!\n", parse_resp->response_status.c_str(), parse_resp->parser.status_code);
    }
    return 0;
}
#endif
