#include "libvinput.h"

#include <ApplicationServices/ApplicationServices.h>
#include <stdlib.h>

typedef struct _EventListenerInternal
{
	CFMachPortRef eventTap;
	CFRunLoopSourceRef runLoopSource;
} EventListenerInternal;

char key_code_to_char(uint16_t keyCode, bool shift, bool caps)
{
	if ((keyCode >= 0 && keyCode <= 40) || (keyCode == 45) || (keyCode == 46)) {
		char key = 0;
		switch (keyCode) {
		case 0: key = 'a'; break;
		case 1: key = 's'; break;
		case 2: key = 'd'; break;
		case 3: key = 'f'; break;
		case 4: key = 'h'; break;
		case 5: key = 'g'; break;
		case 6: key = 'z'; break;
		case 7: key = 'x'; break;
		case 8: key = 'c'; break;
		case 9: key = 'v'; break;
		case 11: key = 'b'; break;
		case 12: key = 'q'; break;
		case 13: key = 'w'; break;
		case 14: key = 'e'; break;
		case 15: key = 'r'; break;
		case 16: key = 'y'; break;
		case 17: key = 't'; break;
		case 31: key = 'o'; break;
		case 32: key = 'u'; break;
		case 34: key = 'i'; break;
		case 35: key = 'p'; break;
		case 37: key = 'l'; break;
		case 38: key = 'j'; break;
		case 40: key = 'k'; break;
		case 45: key = 'n'; break;
		case 46: key = 'm'; break;
		}

		if (shift != caps) key = toupper(key);

		return key;
	}

	switch (keyCode) {
	case 18: return shift ? '!' : '1';
	case 19: return shift ? '@' : '2';
	case 20: return shift ? '#' : '3';
	case 21: return shift ? '$' : '4';
	case 22: return shift ? '^' : '6';
	case 23: return shift ? '%' : '5';
	case 24: return shift ? '+' : '=';
	case 25: return shift ? '(' : '9';
	case 26: return shift ? '&' : '7';
	case 27: return shift ? '_' : '-';
	case 28: return shift ? '*' : '8';
	case 29: return shift ? ')' : '0';
	case 30: return shift ? '}' : ']';
	case 33: return shift ? '{' : '[';
	case 39: return shift ? '\"' : '\'';
	case 41: return shift ? ':' : ';';
	case 42: return shift ? '|' : '\\';
	case 43: return shift ? '<' : ',';
	case 44: return shift ? '?' : '/';
	case 47: return shift ? '>' : '.';
	case 50: return shift ? '~' : '`';
	case 51: return '\b';  // Backspace
	case 48: return '\t';  // Tab
	case 36: return '\n';  // Enter (Return)
	case 49: return ' ';   // Spacebar
	case 53: return 0x1B;  // Escape (ESC)
	case 117: return 0x7F; // Delete (Forward Delete)
	}

	return 0;
}

CGEventRef CGEventCallback(
    CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon)
{
	EventListener *listener = (EventListener *)refcon;
	if (!listener->initialized) return event;

	KeyboardEvent kevent = { 0 };
	kevent.modifiers = (KeyboardModifiers) { 0 };
	kevent.keycode = (uint16_t)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
	kevent.timestamp = (size_t)(CGEventGetTimestamp(event) / 1000000);

	if (type == kCGEventKeyDown || type == kCGEventKeyUp) {
		kevent.pressed = (type == kCGEventKeyDown);

		CGEventFlags flags = CGEventGetFlags(event);
		bool shift = (flags & kCGEventFlagMaskShift) != 0;
		bool caps = (flags & kCGEventFlagMaskAlphaShift) != 0;

		kevent.keychar = key_code_to_char(kevent.keycode, shift, caps);
		kevent.keysym = kevent.keycode;

		if (listener->data) {
			KeyboardCallback callback = (KeyboardCallback)listener->data;
			callback(kevent);
		}
	}

	return event;
}

VInputError _EventListener_init(EventListener *listener)
{
	if (!listener) return VINPUT_UNINITIALIZED;
	listener->initialized = true;

	CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventKeyUp);
	CFMachPortRef eventTap = CGEventTapCreate(
	    kCGSessionEventTap, kCGHeadInsertEventTap, 0, eventMask, CGEventCallback, listener);

	if (!eventTap) {
		fprintf(stderr, "Failed to create event tap\n");
		return VINPUT_MAC_TAP;
	}

	CFRunLoopSourceRef runLoopSource
	    = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
	CGEventTapEnable(eventTap, true);

	EventListenerInternal *internal = malloc(sizeof(EventListenerInternal));
	if (!internal) {
		CFRelease(eventTap);
		CFRelease(runLoopSource);
		return VINPUT_MALLOC;
	}
	internal->runLoopSource = runLoopSource;

	listener->data = internal;
	listener->initialized = true;
	return VINPUT_OK;
}

VInputError EventListener_start(EventListener *listener, KeyboardCallback callback)
{
	if (!listener->listen_keyboard || !callback) return VINPUT_UNINITIALIZED;
	listener->data = callback;
	CFRunLoopRun();
	return VINPUT_OK;
}

VInputError EventListener_free(EventListener *listener)
{
	if (!listener->initialized) return VINPUT_UNINITIALIZED;
	EventListenerInternal *internal = (EventListenerInternal *)listener->data;
	if (internal) {
		CFMachPortInvalidate(internal->eventTap);
		CFRunLoopSourceInvalidate(internal->runLoopSource);
		CFRelease(internal->eventTap);
		CFRelease(internal->runLoopSource);
		free(internal);
	}
	listener->initialized = false;
	return VINPUT_OK;
}
