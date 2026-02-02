#pragma once
#include <string>
#include <functional>

struct TransferProgress
{
    uint64_t total = 0;
    uint64_t now   = 0;
};

using TransferProgressCallback = std::function<void(const TransferProgress&, bool& cancel)>;
using TransferCompleteCallback = std::function<void(const std::string& body, unsigned status)>;
using TransferErrorCallback    = std::function<void(const std::string& body, const std::string& error, unsigned status)>;

struct TransferCallback
{
    TransferProgressCallback onProgress;
    TransferCompleteCallback onComplete;
    TransferErrorCallback    onError;
};
