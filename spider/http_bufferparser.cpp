#include "http_bufferparser.h"
#include <stdio.h>
#include <string.h>

int HttpBufferParser::on_message_begin(http_parser* parser)
{
    HttpBufferParser *self = reinterpret_cast<HttpBufferParser*>(parser->data);
    self->_on_message_begin(parser);
    return 0;
}
int HttpBufferParser::on_request_url(http_parser* parser, const char *at, size_t length)
{
    HttpBufferParser *self = reinterpret_cast<HttpBufferParser*>(parser->data);
    self->_on_request_url(parser, at, length);
    return 0;
}
int HttpBufferParser::on_response_status(http_parser* parser, const char *at, size_t length)
{
    HttpBufferParser *self = reinterpret_cast<HttpBufferParser*>(parser->data);
    self->_on_response_status(parser, at, length);
    return 0;
}
int HttpBufferParser::on_header_field(http_parser* parser, const char *at, size_t length)
{
    HttpBufferParser *self = reinterpret_cast<HttpBufferParser*>(parser->data);
    self->_on_header_field(parser, at, length);
    return 0;
}
int HttpBufferParser::on_header_value(http_parser* parser, const char *at, size_t length)
{
    HttpBufferParser *self = reinterpret_cast<HttpBufferParser*>(parser->data);
    self->_on_header_value(parser, at, length);
    return 0;
}
int HttpBufferParser::on_headers_complete(http_parser *parser)
{
    HttpBufferParser *self = reinterpret_cast<HttpBufferParser*>(parser->data);
    self->_on_headers_complete(parser);
    return 0;
}
int HttpBufferParser::on_body(http_parser* parser, const char *at, size_t length)
{
    HttpBufferParser *self = reinterpret_cast<HttpBufferParser*>(parser->data);
    self->_on_body(parser, at, length);
    return 0;
}
int HttpBufferParser::on_message_complete(http_parser *parser)
{
    HttpBufferParser *self = reinterpret_cast<HttpBufferParser*>(parser->data);
    self->_on_message_complete(parser);
    return 0;
}

int HttpBufferParser::chunk_header_cb(http_parser *parser)
{
    HttpBufferParser *self = reinterpret_cast<HttpBufferParser*>(parser->data);
    self->_chunk_header_cb(parser);
    return 0;
}

int HttpBufferParser::chunk_complete_cb(http_parser *parser)
{
    HttpBufferParser *self = reinterpret_cast<HttpBufferParser*>(parser->data);
    self->_chunk_complete_cb(parser);
    return 0;
}
