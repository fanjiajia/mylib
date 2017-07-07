#ifndef __parse_buffer_h__
#define __parse_buffer_h__
#include "http_bufferparser.h"
#include <string>

class ParseHttpBuffer : public HttpBufferParser {
public:
    ParseHttpBuffer(http_parser_type type): HttpBufferParser(type)
    {
        iscomplete = false;
    }
    ~ParseHttpBuffer(){}

private:
    int _on_message_begin(http_parser *parser);
    int _on_header_field(http_parser *parser, const char *at, size_t len);
    int _on_header_value(http_parser *parser, const char *at, size_t len);
    int _on_request_url(http_parser *parser, const char *at, size_t len);
    int _on_response_status(http_parser *parser, const char *at, size_t len);
    int _on_body(http_parser *parser, const char *at, size_t len);
    int _on_headers_complete(http_parser *parser);
    int _on_message_complete(http_parser *parser);
    int _chunk_header_cb(http_parser *parser);
    int _chunk_complete_cb(http_parser *parser);

public:
    std::string response_status;
    std::string http_body;
    bool iscomplete;
};
#endif
