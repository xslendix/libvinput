import libvinput
import signal
import sys

logger = libvinput.listener_create(True)
def signal_handler(sig, frame):
    global logger
    print('You pressed Ctrl+C!')
    libvinput.listener_destroy(logger)
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

word = ''

def commit():
    global word
    if len(word) > 0:
        print("Word:", word)
        word = ''

def cb(evt):
    global word

    if evt.pressed: return
    if evt.keychar.isdigit():
        word = ''
        return
    
    if evt.keychar == '\b':
        word = word[:-1]
        return

    if not evt.keychar.isprintable(): return
    if not evt.keychar.isalpha() or evt.keychar.isspace():
        commit()
    else:
        word += evt.keychar

def cb_mouse(evt):
    commit()

def cb_mouse_move(evt):
    commit()
    pass

libvinput.listener_start(logger, cb, cb_mouse, cb_mouse_move)

