#include <sstream>

#include <json/json.h>

#include "Utils.hpp"
#include "Response.hpp"

using namespace std;
using namespace pion;
using namespace pion::net;

namespace lite_comet {

const string Response::INVALID_REQUEST("{\"error\": \"Invalid request\"}");
const string Response::NOT_ACTIVE_CHANNEL(
    "{\"error\": \"Not active channel (no listeners)\"}");
const string Response::OK("{\"status\": \"ok\"}");

void Response::ok(
    HTTPResponseWriterPtr writer, 
    const string& text, 
    bool with_headers
) {
    if(with_headers) {
        // TODO
    }
    writer->write(text);
}

void Response::error(
    HTTPResponseWriterPtr writer, 
    const string& text, 
    bool with_headers
) {
    if(with_headers) {
        // TODO
    }
    writer->write(text);
}

void Response::data(
    HTTPResponseWriterPtr writer, 
    const string& data, 
    const string& callback, 
    bool with_headers
) {
    if(!callback.empty()) {
        Response::ok(writer, callback + "(" + data + ");", with_headers);
    } else {
        Response::ok(writer, data, with_headers);
    }
}

void Response::empty(
    HTTPResponseWriterPtr writer, 
    long new_offset,
    const string& callback, 
    bool with_headers
) {
    Json::Value root;
    root["new_offset"] = static_cast<Json::Int>(new_offset);
    Json::FastWriter json_writer;
    Response::data(writer, json_writer.write(root), callback, with_headers);
}

}   // end namespace lite_comet 

