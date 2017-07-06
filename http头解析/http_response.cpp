#include "http_response.h"
#include <stdio.h>
#include <string.h>

int HttpResponse::on_message_begin(http_parser* parser)
{
    HttpResponse *self = reinterpret_cast<HttpResponse*>(parser->data);
    self->_on_message_begin(parser);
    return 0;
}
int HttpResponse::on_request_url(http_parser* parser, const char *at, size_t length)
{
    HttpResponse *self = reinterpret_cast<HttpResponse*>(parser->data);
    self->_on_request_url(parser, at, length);
    return 0;
}
int HttpResponse::on_response_status(http_parser* parser, const char *at, size_t length)
{
    HttpResponse *self = reinterpret_cast<HttpResponse*>(parser->data);
    self->_on_response_status(parser, at, length);
    return 0;
}
int HttpResponse::on_header_field(http_parser* parser, const char *at, size_t length)
{
    HttpResponse *self = reinterpret_cast<HttpResponse*>(parser->data);
    self->_on_header_field(parser, at, length);
    return 0;
}
int HttpResponse::on_header_value(http_parser* parser, const char *at, size_t length)
{
    HttpResponse *self = reinterpret_cast<HttpResponse*>(parser->data);
    self->_on_header_value(parser, at, length);
    return 0;
}
int HttpResponse::on_headers_complete(http_parser *parser)
{
    HttpResponse *self = reinterpret_cast<HttpResponse*>(parser->data);
    self->_on_headers_complete(parser);
    return 0;
}
int HttpResponse::on_body(http_parser* parser, const char *at, size_t length)
{
    HttpResponse *self = reinterpret_cast<HttpResponse*>(parser->data);
    self->_on_body(parser, at, length);
    return 0;
}
int HttpResponse::on_message_complete(http_parser *parser)
{
    HttpResponse *self = reinterpret_cast<HttpResponse*>(parser->data);
    self->_on_message_complete(parser);
    return 0;
}

int HttpResponse::chunk_header_cb(http_parser *parser)
{
    HttpResponse *self = reinterpret_cast<HttpResponse*>(parser->data);
    self->_chunk_header_cb(parser);
    return 0;
}

int HttpResponse::chunk_complete_cb(http_parser *parser)
{
    HttpResponse *self = reinterpret_cast<HttpResponse*>(parser->data);
    self->_chunk_complete_cb(parser);
    return 0;
}
