"use strict";

importScripts("rune_playground.js");

function postRuntimeError(error) {
    self.postMessage({
        type: "error",
        message: error?.message ?? String(error),
        stack: error?.stack ?? null,
    });
}

self.onerror = (message, source, lineno, colno, error) => {
    self.postMessage({
        type: "error",
        message: error?.message ?? String(message),
        stack: error?.stack ?? `${source ?? "worker"}:${lineno ?? 0}:${colno ?? 0}`,
    });
    return true;
};

self.onunhandledrejection = (event) => {
    postRuntimeError(event.reason);
};

globalThis.window = globalThis;
globalThis.devicePixelRatio = globalThis.devicePixelRatio ?? 1;
globalThis.screen = globalThis.screen ?? { orientation: null };
let userHasActivatedPage = false;
globalThis.navigator ??= {};
if (!globalThis.navigator.userActivation) {
    Object.defineProperty(globalThis.navigator, "userActivation", {
        configurable: true,
        value: {
            get hasBeenActive() { return userHasActivatedPage; },
            get isActive() { return userHasActivatedPage; },
        },
    });
}
globalThis.matchMedia = globalThis.matchMedia ?? (() => ({
    matches: false,
    addEventListener: () => {},
    removeEventListener: () => {},
}));

function emptyElement(tagName = "div") {
    const element = {
        tagName: tagName.toUpperCase(),
        style: {},
        children: [],
        append: (...children) => element.children.push(...children),
        appendChild: (child) => {
            element.children.push(child);
            return child;
        },
        insertBefore: (child) => {
            element.children.push(child);
            return child;
        },
        remove: () => {},
        addEventListener: () => {},
        removeEventListener: () => {},
        getBoundingClientRect: () => ({ left: 0, top: 0, width: globalThis.innerWidth, height: globalThis.innerHeight }),
        classList: { add: () => {}, remove: () => {} },
    };
    return element;
}

function installDocumentShim(canvas) {
    canvas.id = "canvas";
    canvas.style ??= {};
    canvas.addEventListener ??= () => {};
    canvas.removeEventListener ??= () => {};
    canvas.onwheel ??= null;
    canvas.getBoundingClientRect ??= () => ({ left: 0, top: 0, width: canvas.width, height: canvas.height });

    const body = emptyElement("body");
    body.clientWidth = canvas.width;
    body.clientHeight = canvas.height;
    body.requestPointerLock = () => {};

    globalThis.document = {
        body,
        documentElement: emptyElement("html"),
        head: emptyElement("head"),
        title: "Rune Playground",
        hidden: false,
        visibilityState: "visible",
        activeElement: canvas,
        currentScript: null,
        fullscreenEnabled: false,
        webkitFullscreenEnabled: false,
        pointerLockElement: null,
        querySelector: (selector) => selector === "#canvas" || selector === "canvas" ? canvas : null,
        getElementById: (id) => id === "canvas" ? canvas : null,
        createElement: (tagName) => tagName === "canvas" ? new OffscreenCanvas(1, 1) : emptyElement(tagName),
        // SDL3's Emscripten input backend registers keyboard listeners on
        // "#window"/"#document", which resolve to `self` and this shim
        // respectively. Delegate to `self` (a real EventTarget) so both
        // targets end up on the same listener set we can dispatch to below.
        addEventListener: (...args) => self.addEventListener(...args),
        removeEventListener: (...args) => self.removeEventListener(...args),
        exitPointerLock: () => {},
        hasFocus: () => true,
    };
}

function writeCode(module, code) {
    const utf8 = new TextEncoder().encode(code);
    const bufferPtr = module._malloc(utf8.length + 1);
    if (!bufferPtr) {
        throw new Error("Failed to allocate WASM source buffer");
    }

    module.HEAPU8.set(utf8, bufferPtr);
    module.HEAPU8[bufferPtr + utf8.length] = 0;
    return bufferPtr;
}

// The transferred OffscreenCanvas the running program renders into, and the
// element input events (mouse/pointer) get dispatched on. Set once "run"
// starts; forwarded input before that (or after the run ends) is dropped.
let activeCanvas = null;

// KeyboardEvent/MouseEvent/PointerEvent constructors don't exist in a worker
// global scope (only plain Event does), so build a bare Event and bolt the
// fields SDL3's compiled input backend actually reads onto it. It only ever
// touches plain properties (e.key, e.clientX, e.preventDefault(), ...), so a
// duck-typed Event works exactly as well as a real KeyboardEvent/PointerEvent.
function makeSyntheticEvent(type, fields) {
    const event = new Event(type, { bubbles: true, cancelable: true });
    Object.assign(event, fields);
    return event;
}

function dispatchForwardedInput(kind, detail) {
    userHasActivatedPage = true;

    if (kind === "keydown" || kind === "keyup") {
        const keyEvent = () => makeSyntheticEvent(kind, {
            ...detail,
            // Real KeyboardEvents expose modifier-key state via a method,
            // not a plain property; we don't track lock-key state, so report
            // them as off rather than leaving the method missing entirely.
            getModifierState: () => false,
        });
        // SDL3's HTML5 backend can register keyboard listeners on the
        // window/document *or* the canvas element depending on build
        // config; dispatch on all of them rather than guessing which.
        self.dispatchEvent(keyEvent());
        activeCanvas?.dispatchEvent?.(keyEvent());
        return;
    }

    if (!activeCanvas) return;
    if (kind === "wheel") {
        activeCanvas.dispatchEvent?.(makeSyntheticEvent("wheel", detail));
        return;
    }
    const pointerKind = kind === "mousemove" ? "pointermove" : kind === "mousedown" ? "pointerdown" : "pointerup";
    activeCanvas.dispatchEvent?.(makeSyntheticEvent(pointerKind, {
        ...detail,
        pointerId: 1,
        pointerType: "mouse",
        isPrimary: true,
    }));
    // The simple mouse_x/mouse_y tracker SDL3 installs listens on "document"
    // (aliased to `self` above) using plain mouse events, not pointer events.
    self.dispatchEvent(makeSyntheticEvent(kind, detail));
}

self.onmessage = async (event) => {
    if (event.data?.type === "input") {
        dispatchForwardedInput(event.data.kind, event.data.detail);
        return;
    }

    if (event.data?.type !== "run") return;

    const { code, packageData, canvas } = event.data;
    activeCanvas = canvas;
    globalThis.innerWidth = canvas?.width ?? 0;
    globalThis.innerHeight = canvas?.height ?? 0;
    globalThis.outerWidth = globalThis.innerWidth;
    globalThis.outerHeight = globalThis.innerHeight;
    globalThis.pageXOffset = 0;
    globalThis.pageYOffset = 0;
    installDocumentShim(canvas);
    let module = null;
    let bufferPtr = 0;

    try {
        module = await RunePlayground({
            canvas,
            print: (text) => self.postMessage({ type: "stdout", text }),
            printErr: (text) => self.postMessage({ type: "stderr", text }),
        });

        bufferPtr = writeCode(module, packageData ? JSON.stringify(packageData) : code);
        const exitCode = packageData
            ? module._wasm_run_package(bufferPtr)
            : module._wasm_run_code(bufferPtr);
        module._free(bufferPtr);
        self.postMessage({ type: "done", exitCode });
    } catch (error) {
        if (error === "unwind") {
            if (module && bufferPtr) {
                module._free(bufferPtr);
            }
            return;
        }
        if (module && bufferPtr) {
            module._free(bufferPtr);
        }
        postRuntimeError(error);
    }
};
