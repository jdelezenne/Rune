#pragma once

#include "Rune/Core/Optional.hpp"
#include "Rune/Core/String.hpp"

#include <nlohmann/json.hpp>

namespace rune::lsp
{
    using Json = nlohmann::json;

    struct JsonRpcMessage final
    {
        Optional<int64> id;
        String method;
        Json params = Json::object();
        bool isRequest = false;
        bool isNotification = false;
        bool isResponse = false;
        bool success = true;
        Json result = Json::object();
        Json error = Json::object();
    };

    class JsonRpcTransport final
    {
    public:
        bool readMessage(JsonRpcMessage& message);
        void writeResponse(Optional<int64> id, Json result);
        void writeError(Optional<int64> id, int code, String const& message);
        void writeNotification(String const& method, Json params);
    };
}
