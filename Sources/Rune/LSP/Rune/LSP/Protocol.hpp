#pragma once

#include "Rune/Core/String.hpp"

namespace rune::lsp
{
    inline String const methodInitialize = "initialize";
    inline String const methodInitialized = "initialized";
    inline String const methodShutdown = "shutdown";
    inline String const methodExit = "exit";

    inline String const methodDidOpen = "textDocument/didOpen";
    inline String const methodDidChange = "textDocument/didChange";
    inline String const methodDidClose = "textDocument/didClose";
    inline String const methodDefinition = "textDocument/definition";
    inline String const methodDeclaration = "textDocument/declaration";
    inline String const methodDocumentSymbol = "textDocument/documentSymbol";
    inline String const methodHover = "textDocument/hover";
    inline String const methodReferences = "textDocument/references";
    inline String const methodCompletion = "textDocument/completion";
    inline String const methodSignatureHelp = "textDocument/signatureHelp";

    inline String const notificationPublishDiagnostics = "textDocument/publishDiagnostics";
}
