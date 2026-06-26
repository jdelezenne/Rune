"use strict";

// Examples

const examplesById = new Map();
const CUSTOM_PROGRAM_ID = "custom.singleFile";
const CUSTOM_PROGRAM_PATH = "Main.rune";
const CUSTOM_PROGRAM_TEMPLATE = `func main() {
}
`;

function loadExamples() {
    for (const example of RUNE_PLAYGROUND_EXAMPLES.examples ?? []) {
        examplesById.set(example.id, example);
        const option = document.createElement("option");
        option.value = example.id;
        option.textContent = example.label;
        exampleSel.appendChild(option);
    }

    if (loadProgramFromLocation()) {
        return;
    }

    const defaultExample = examplesById.get(RUNE_PLAYGROUND_EXAMPLES.default) ?? examplesById.values().next().value;
    if (defaultExample) selectProgram(defaultExample);
}

function sourceFilesForProgram(program) {
    if (program?.files?.length) {
        return program.files;
    }
    return [{
        path: CUSTOM_PROGRAM_PATH,
        content: program?.content ?? "",
    }];
}

function defaultSourcePath(files) {
    return files.find((file) => file.path.split("/").pop() === "Main.rune")?.path ?? files[0]?.path ?? CUSTOM_PROGRAM_PATH;
}

function createCustomProgram() {
    return {
        id: CUSTOM_PROGRAM_ID,
        label: "Untitled",
        content: CUSTOM_PROGRAM_TEMPLATE,
        files: [{
            path: CUSTOM_PROGRAM_PATH,
            content: CUSTOM_PROGRAM_TEMPLATE,
        }],
        view: "output",
    };
}

function createCustomProgramWithCode(code) {
    return {
        id: CUSTOM_PROGRAM_ID,
        label: "Shared",
        content: code,
        files: [{
            path: CUSTOM_PROGRAM_PATH,
            content: code,
        }],
        view: "output",
    };
}

function base64UrlEncode(text) {
    const bytes = new TextEncoder().encode(text);
    let binary = "";
    for (const byte of bytes) {
        binary += String.fromCharCode(byte);
    }
    return btoa(binary).replace(/\+/g, "-").replace(/\//g, "_").replace(/=+$/g, "");
}

function base64UrlDecode(text) {
    const padded = text.replace(/-/g, "+").replace(/_/g, "/").padEnd(Math.ceil(text.length / 4) * 4, "=");
    const binary = atob(padded);
    const bytes = new Uint8Array(binary.length);
    for (let index = 0; index < binary.length; ++index) {
        bytes[index] = binary.charCodeAt(index);
    }
    return new TextDecoder().decode(bytes);
}

function loadProgramFromLocation() {
    const params = new URLSearchParams(window.location.search);
    const code = params.get("code");
    if (code != null) {
        selectProgram(createCustomProgramWithCode(base64UrlDecode(code)));
        return true;
    }

    const exampleId = params.get("example");
    const example = exampleId ? examplesById.get(exampleId) : null;
    if (!example) {
        return false;
    }

    selectProgram(example);
    const file = params.get("file");
    if (file && sourceEdits.has(file)) {
        selectedSourcePath = file;
        renderSourceSelector();
        loadSelectedSource();
    }
    return true;
}

function saveCurrentSourceEdit() {
    if (!selectedSourcePath) return;
    sourceEdits.set(selectedSourcePath, editor.value);
}

function syncProgramControls() {
    const isCustomProgram = selectedExample?.id === CUSTOM_PROGRAM_ID;
    exampleSel.value = isCustomProgram ? "" : selectedExample?.id ?? "";
    shareBtn.disabled = isCustomProgram;
}

function selectProgram(program) {
    selectedExample = program;
    sourceEdits = new Map(sourceFilesForProgram(program).map((file) => [file.path, file.content]));
    selectedSourcePath = defaultSourcePath(sourceFilesForProgram(program));
    renderSourceSelector();
    loadSelectedSource();
    syncProgramControls();
    recreateDisplayCanvas();
    showResultView(program.view === "display" ? "display" : "output");
}

function renderSourceSelector() {
    const files = sourceFilesForProgram(selectedExample);
    sourceFileSel.innerHTML = "";
    for (const file of files) {
        const option = document.createElement("option");
        option.value = file.path;
        option.textContent = file.path;
        sourceFileSel.appendChild(option);
    }
    sourceFileSel.value = selectedSourcePath ?? "";
    sourceFileSel.disabled = files.length <= 1;
}

function loadSelectedSource() {
    editor.value = sourceEdits.get(selectedSourcePath) ?? "";
}

function currentShareUrl() {
    saveCurrentSourceEdit();
    const url = new URL(window.location.href);
    url.search = "";
    url.hash = "";

    if (selectedExample?.id && selectedExample.id !== CUSTOM_PROGRAM_ID) {
        url.searchParams.set("example", selectedExample.id);
        return url.toString();
    }

    url.searchParams.set("code", base64UrlEncode(editor.value));
    return url.toString();
}

async function copyText(text) {
    if (navigator.clipboard?.writeText) {
        await navigator.clipboard.writeText(text);
        return;
    }

    const scratch = document.createElement("textarea");
    scratch.value = text;
    scratch.setAttribute("readonly", "");
    scratch.style.position = "fixed";
    scratch.style.left = "-9999px";
    document.body.appendChild(scratch);
    scratch.select();
    document.execCommand("copy");
    scratch.remove();
}

function currentRunPackage() {
    saveCurrentSourceEdit();
    const files = sourceFilesForProgram(selectedExample);
    if (!selectedExample?.manifest && files.length <= 1) {
        return null;
    }

    const packageData = {
        files: files.map((file) => ({
            path: file.path,
            content: sourceEdits.get(file.path) ?? file.content,
        })),
    };
    if (selectedExample.manifest) {
        packageData.manifest = selectedExample.manifest;
    }
    if (selectedExample.resources?.length) {
        packageData.resources = selectedExample.resources;
    }

    return packageData;
}

// DOM

const editor = /** @type {HTMLTextAreaElement} */ (document.getElementById("editor"));
const runBtn = document.getElementById("run-btn");
const stopBtn = document.getElementById("stop-btn");
const newBtn = document.getElementById("new-btn");
const shareBtn = document.getElementById("share-btn");
const clearBtn = document.getElementById("clear-btn");
const exampleSel = document.getElementById("examples");
const sourceFileSel = document.getElementById("source-files");
const outputEl = document.getElementById("output");
const statusBadge = document.getElementById("status-badge");
const elapsedEl = document.getElementById("elapsed");
const dividerEl = document.getElementById("divider");
const outputTab = document.getElementById("output-tab");
const displayTab = document.getElementById("display-tab");
const displayEl = document.getElementById("display");
let displayCanvas = /** @type {HTMLCanvasElement} */ (document.getElementById("canvas"));

// WASM module

let defaultExampleLoaded = false;
let defaultExampleError = null;
let selectedExample = null;
let capturedOut = [];
let capturedErr = [];
let activeWorker = null;
let activeMainThreadRun = null;
let retainedDisplayWorker = null;
let retainedMainThreadRun = null;
let runStartedAt = 0;
let selectedSourcePath = null;
let sourceEdits = new Map();
let shareLabelTimeout = 0;

try {
    loadExamples();
    defaultExampleLoaded = true;
} catch (err) {
    defaultExampleLoaded = true;
    defaultExampleError = err;
}
setReady();

// State

function setReady() {
    if (!defaultExampleLoaded) return;
    if (defaultExampleError) {
        runBtn.disabled = true;
        showError(defaultExampleError.message);
        return;
    }
    runBtn.disabled = false;
    outputEl.innerHTML = '<span class="placeholder">Run your code to see output here.</span>';
}

function showError(msg) {
    outputEl.innerHTML = `<span class="out-stderr">${escHtml(msg)}</span>`;
}

// Example picker

exampleSel.addEventListener("change", async () => {
    const key = exampleSel.value;
    const example = examplesById.get(key);
    if (example) {
        try {
            stopProgram();
            releaseRetainedDisplayWorker();
            releaseRetainedMainThreadRun();
            selectProgram(example);
            editor.focus();
            clearOutput();
        } catch (err) {
            showError(err.message);
        }
    }
});

sourceFileSel.addEventListener("change", () => {
    saveCurrentSourceEdit();
    selectedSourcePath = sourceFileSel.value;
    loadSelectedSource();
    editor.focus();
});

newBtn.addEventListener("click", () => {
    stopProgram();
    releaseRetainedDisplayWorker();
    releaseRetainedMainThreadRun();
    selectProgram(createCustomProgram());
    clearOutput();
    editor.focus();
});

shareBtn.addEventListener("click", async () => {
    if (shareBtn.disabled) return;

    const originalLabel = "Share";
    try {
        await copyText(currentShareUrl());
        shareBtn.textContent = "Copied";
    } catch (err) {
        shareBtn.textContent = "Failed";
        showError(err?.message ?? String(err));
    }

    window.clearTimeout(shareLabelTimeout);
    shareLabelTimeout = window.setTimeout(() => {
        shareBtn.textContent = originalLabel;
    }, 1200);
});

// Input forwarding (keyboard/mouse → worker)
//
// The running program executes inside a Worker against a transferred
// OffscreenCanvas, so it never sees real browser input events on its own.
// We capture them here on the main thread and forward the relevant fields;
// runner-worker.js reconstructs real KeyboardEvent/PointerEvent/MouseEvent
// objects and dispatches them so the compiled SDL3 input backend picks them
// up exactly as it would in a normal (non-worker) page.

const GAME_KEYS_PREVENT_DEFAULT = new Set([
    "ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight", " ", "Spacebar",
]);

function forwardKeyEvent(kind, e) {
    const worker = activeWorker ?? retainedDisplayWorker;
    if (!worker || e.target === editor) return;
    if (GAME_KEYS_PREVENT_DEFAULT.has(e.key)) {
        e.preventDefault();
    }
    worker.postMessage({
        type: "input",
        kind,
        detail: {
            key: e.key,
            code: e.code,
            keyCode: e.keyCode,
            which: e.which,
            charCode: e.charCode,
            location: e.location,
            ctrlKey: e.ctrlKey,
            shiftKey: e.shiftKey,
            altKey: e.altKey,
            metaKey: e.metaKey,
            repeat: e.repeat,
        },
    });
}

window.addEventListener("keydown", (e) => forwardKeyEvent("keydown", e));
window.addEventListener("keyup", (e) => forwardKeyEvent("keyup", e));

function attachCanvasInputForwarding(canvas) {
    const scaledPosition = (e) => {
        const rect = canvas.getBoundingClientRect();
        const scaleX = rect.width ? canvas.width / rect.width : 1;
        const scaleY = rect.height ? canvas.height / rect.height : 1;
        return {
            clientX: (e.clientX - rect.left) * scaleX,
            clientY: (e.clientY - rect.top) * scaleY,
        };
    };
    const forward = (kind) => (e) => {
        const worker = activeWorker ?? retainedDisplayWorker;
        if (!worker) return;
        worker.postMessage({
            type: "input",
            kind,
            detail: {
                ...scaledPosition(e),
                button: e.button,
                buttons: e.buttons,
            },
        });
    };
    const forwardWheel = (e) => {
        e.preventDefault();
        const worker = activeWorker ?? retainedDisplayWorker;
        if (!worker) return;
        worker.postMessage({
            type: "input",
            kind: "wheel",
            detail: {
                ...scaledPosition(e),
                deltaX: e.deltaX,
                deltaY: e.deltaY,
                deltaZ: e.deltaZ,
                deltaMode: e.deltaMode,
            },
        });
    };
    canvas.addEventListener("mousemove", forward("mousemove"));
    canvas.addEventListener("mousedown", forward("mousedown"));
    canvas.addEventListener("mouseup", forward("mouseup"));
    canvas.addEventListener("wheel", forwardWheel, { passive: false });
}

function sourceUsesAudio(code) {
    return /\bInitFlags\s*\.\s*audio\b|\bSDL3\s*\.\s*(openAudioDevice|openAudioDeviceStream|createAudioStream|resumeAudioDevice|putAudioStreamData|setAudioStream(Get|Put)Callback)\b/.test(code);
}

function runSourceText() {
    const packageData = currentRunPackage();
    if (packageData) {
        return packageData.files.map((file) => file.content).join("\n");
    }

    return editor.value;
}

// Run

function runCode() {
    if (!editor.value.trim() || activeWorker || activeMainThreadRun) return;

    releaseRetainedDisplayWorker();
    releaseRetainedMainThreadRun();
    setRunning(true);
    clearOutput();

    capturedOut = [];
    capturedErr = [];
    runStartedAt = performance.now();

    const canvas = recreateDisplayCanvas();
    canvas.addEventListener("contextmenu", (e) => e.preventDefault());
    const packageData = currentRunPackage();
    if (sourceUsesAudio(runSourceText())) {
        runCodeOnMainThread(canvas);
        return;
    }

    attachCanvasInputForwarding(canvas);
    canvas.dataset.transferred = "true";
    const offscreenCanvas = canvas.transferControlToOffscreen();
    activeWorker = new Worker("runner-worker.js");
    activeWorker.onmessage = handleWorkerMessage;
    activeWorker.onerror = (event) => {
        capturedErr.push(formatRuntimeError(event.message, {
            filename: event.filename,
            lineno: event.lineno,
            colno: event.colno,
        }));
        finishRun(1, { preserveDisplay: false });
    };
    activeWorker.postMessage({
        type: "run",
        code: editor.value,
        packageData,
        canvas: offscreenCanvas,
    }, [offscreenCanvas]);
}

async function runCodeOnMainThread(canvas) {
    activeMainThreadRun = { module: null };

    try {
        const module = await RunePlayground({
            canvas,
            print: (text) => {
                capturedOut.push(text);
                renderLiveOutput();
            },
            printErr: (text) => {
                capturedErr.push(text);
                renderLiveOutput();
            },
        });
        if (!activeMainThreadRun) {
            return;
        }

        activeMainThreadRun.module = module;
        const packageData = currentRunPackage();
        const bufferPtr = packageData
            ? writeCodeToModule(module, JSON.stringify(packageData))
            : writeCodeToModule(module, editor.value);
        let exitCode = 1;
        try {
            exitCode = packageData
                ? module._wasm_run_package(bufferPtr)
                : module._wasm_run_code(bufferPtr);
        } finally {
            module._free(bufferPtr);
        }

        finishRun(exitCode, {
            preserveDisplay: exitCode === 0 && selectedExample?.view === "display",
            mainThread: true,
        });
    } catch (error) {
        if (String(error) === "unwind") {
            renderLiveOutput();
            return;
        }

        capturedErr.push(formatRuntimeError(error?.message ?? String(error), { stack: error?.stack }));
        finishRun(1, { preserveDisplay: false, mainThread: true });
    }
}

function writeCodeToModule(module, code) {
    const utf8 = new TextEncoder().encode(code);
    const bufferPtr = module._malloc(utf8.length + 1);
    if (!bufferPtr) {
        throw new Error("Failed to allocate WASM source buffer");
    }

    module.HEAPU8.set(utf8, bufferPtr);
    module.HEAPU8[bufferPtr + utf8.length] = 0;
    return bufferPtr;
}

function handleWorkerMessage(event) {
    const message = event.data;
    if (message.type === "stdout") {
        capturedOut.push(message.text);
        renderLiveOutput();
        return;
    }
    if (message.type === "stderr") {
        capturedErr.push(message.text);
        renderLiveOutput();
        return;
    }
    if (message.type === "error") {
        capturedErr.push(formatRuntimeError(message.message, { stack: message.stack }));
        finishRun(1, { preserveDisplay: false });
        return;
    }
    if (message.type === "done") {
        finishRun(message.exitCode, {
            preserveDisplay: message.exitCode === 0 && selectedExample?.view === "display",
        });
    }
}

function handleRetainedDisplayWorkerMessage(event) {
    const message = event.data;
    if (message.type === "stdout") {
        capturedOut.push(message.text);
        renderLiveOutput();
        return;
    }
    if (message.type === "stderr") {
        capturedErr.push(message.text);
        renderLiveOutput();
        return;
    }
    if (message.type === "error") {
        capturedErr.push(formatRuntimeError(message.message, { stack: message.stack }));
        failRetainedDisplayWorker();
    }
}

function failRetainedDisplayWorker() {
    retainedDisplayWorker?.terminate();
    retainedDisplayWorker = null;
    const elapsed = ((performance.now() - runStartedAt) / 1000).toFixed(3);
    renderResult(
        capturedOut.join("\n"),
        capturedErr.join("\n"),
        1,
        parseFloat(elapsed));
}

function finishRun(exitCode, options = {}) {
    if (activeWorker) {
        if (options.preserveDisplay) {
            retainedDisplayWorker = activeWorker;
            retainedDisplayWorker.onmessage = handleRetainedDisplayWorkerMessage;
            retainedDisplayWorker.onerror = (event) => {
                capturedErr.push(formatRuntimeError(event.message, {
                    filename: event.filename,
                    lineno: event.lineno,
                    colno: event.colno,
                }));
                failRetainedDisplayWorker();
            };
        } else {
            activeWorker.terminate();
        }
        activeWorker = null;
    }
    if (activeMainThreadRun) {
        if (!options.preserveDisplay) {
            activeMainThreadRun.module?._wasm_cancel_main_loop?.();
        }
        if (options.preserveDisplay && options.mainThread) {
            retainedMainThreadRun = activeMainThreadRun;
        }
        activeMainThreadRun = null;
    }
    const elapsed = ((performance.now() - runStartedAt) / 1000).toFixed(3);
    renderResult(capturedOut.join("\n"), capturedErr.join("\n"), exitCode, parseFloat(elapsed), options.statusKind);
    setRunning(false);
}

function renderLiveOutput() {
    if (selectedExample?.view === "display") {
        outputEl.textContent = [capturedOut.join("\n"), capturedErr.join("\n")]
            .filter(Boolean)
            .join("\n");
    }
}

function formatRuntimeError(message, detail = {}) {
    let text = `Runtime error: ${message}`;
    if (detail.stack) {
        text += `\n${detail.stack}`;
    } else if (detail.filename) {
        text += `\n${detail.filename}:${detail.lineno ?? 0}:${detail.colno ?? 0}`;
    }
    return text;
}

function releaseRetainedDisplayWorker() {
    if (!retainedDisplayWorker) return;
    retainedDisplayWorker.terminate();
    retainedDisplayWorker = null;
}

function releaseRetainedMainThreadRun() {
    retainedMainThreadRun?.module?._wasm_cancel_main_loop?.();
    retainedMainThreadRun = null;
}

runBtn.addEventListener("click", runCode);
stopBtn.addEventListener("click", stopProgram);

editor.addEventListener("keydown", (e) => {
    if ((e.ctrlKey || e.metaKey) && e.key === "Enter") {
        e.preventDefault();
        runCode();
    }
    if (e.key === "Tab") {
        e.preventDefault();
        const s = editor.selectionStart;
        editor.value = editor.value.slice(0, s) + "    " + editor.value.slice(editor.selectionEnd);
        editor.selectionStart = editor.selectionEnd = s + 4;
    }
});

clearBtn.addEventListener("click", clearOutput);

outputTab.addEventListener("click", () => showResultView("output"));
displayTab.addEventListener("click", () => showResultView("display"));

function showResultView(view) {
    const showDisplay = view === "display";
    outputEl.hidden = showDisplay;
    displayEl.hidden = !showDisplay;
    outputEl.classList.toggle("active", !showDisplay);
    displayEl.classList.toggle("active", showDisplay);
    outputTab.classList.toggle("active", !showDisplay);
    displayTab.classList.toggle("active", showDisplay);
    outputTab.setAttribute("aria-selected", String(!showDisplay));
    displayTab.setAttribute("aria-selected", String(showDisplay));
}

function recreateDisplayCanvas() {
    const replacement = document.createElement("canvas");
    replacement.id = "canvas";
    replacement.className = "display-canvas";
    displayCanvas.replaceWith(replacement);
    displayCanvas = replacement;
    return displayCanvas;
}

// Output rendering

function clearOutput() {
    outputEl.innerHTML = "";
    statusBadge.className = "status-badge hidden";
    elapsedEl.classList.add("hidden");
}

function renderResult(stdout, stderr, exitCode, elapsed, statusKind = null) {
    outputEl.innerHTML = "";

    if (stdout) {
        const span = document.createElement("span");
        span.className = "out-stdout";
        span.textContent = stdout;
        outputEl.appendChild(span);
    }
    if (stderr) {
        if (stdout) outputEl.appendChild(document.createTextNode("\n"));
        const span = document.createElement("span");
        span.className = "out-stderr";
        span.textContent = stderr;
        outputEl.appendChild(span);
    }
    if (!stdout && !stderr) {
        const span = document.createElement("span");
        span.className = "placeholder";
        span.textContent = "(no output)";
        outputEl.appendChild(span);
    }

    setStatus(statusKind ?? (exitCode === 0 ? "ok" : "fail"), elapsed);
}

function setStatus(kind, elapsed) {
    statusBadge.className = `status-badge ${kind}`;
    statusBadge.textContent =
        kind === "ok" ? "ok" :
        kind === "fail" ? "error" :
        kind === "stopped" ? "stopped" :
        "running";
    if (elapsed != null) {
        elapsedEl.textContent = `${elapsed} s`;
        elapsedEl.classList.remove("hidden");
    } else {
        elapsedEl.classList.add("hidden");
    }
}

function setRunning(on) {
    runBtn.disabled = on;
    stopBtn.disabled = !on;
    if (on) setStatus("busy", null);
}

function stopProgram() {
    if (!activeWorker && !activeMainThreadRun) {
        return;
    }

    finishRun(0, {
        mainThread: Boolean(activeMainThreadRun),
        statusKind: "stopped",
    });
}

function escHtml(s) {
    return s.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
}

// Drag-to-resize

(function () {
    const panes = document.querySelector(".panes");
    const editorP = document.querySelector(".editor-pane");
    const outputP = document.querySelector(".output-pane");
    let drag = false;

    dividerEl.addEventListener("mousedown", (e) => {
        drag = true;
        dividerEl.classList.add("dragging");
        document.body.style.cssText += ";cursor:col-resize;user-select:none";
        e.preventDefault();
    });
    document.addEventListener("mousemove", (e) => {
        if (!drag) return;
        const rect = panes.getBoundingClientRect();
        const total = panes.clientWidth - dividerEl.offsetWidth;
        const left = Math.max(200, Math.min(e.clientX - rect.left, total - 200));
        editorP.style.cssText = `flex:none;width:${left}px`;
        outputP.style.cssText = `flex:none;width:${total - left}px`;
    });
    document.addEventListener("mouseup", () => {
        if (!drag) return;
        drag = false;
        dividerEl.classList.remove("dragging");
        document.body.style.cursor = "";
        document.body.style.userSelect = "";
    });
})();
