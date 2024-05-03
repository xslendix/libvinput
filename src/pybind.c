#include <Python.h>

#include "libvinput.h"

static PyObject *py_EventListener_create(PyObject *self, PyObject *args)
{
	static EventListener listener;
	bool listen_keyboard;

	if (!PyArg_ParseTuple(args, "p", &listen_keyboard)) return NULL;

	VInputError result = EventListener_create(&listener, listen_keyboard);
	if (result != VINPUT_OK) {
		PyErr_SetString(PyExc_RuntimeError, VInput_error_get_message(result));
		return NULL;
	}

	PyObject *listener_obj = PyCapsule_New(&listener, "EventListener", NULL);
	return listener_obj;
}

static PyObject *py_EventListener_free(PyObject *self, PyObject *args)
{
	EventListener *listener;
	PyObject *listener_obj;

	if (!PyArg_ParseTuple(args, "O", &listener_obj)) return NULL;
	listener = (EventListener *)PyCapsule_GetPointer(listener_obj, "EventListener");

	VInputError result = EventListener_free(listener);
	if (result != VINPUT_OK) {
		PyErr_SetString(PyExc_RuntimeError, VInput_error_get_message(result));
		return NULL;
	}
	Py_RETURN_NONE;
}

typedef struct
{
	PyObject_HEAD KeyboardModifiers modifiers;
} PyKeyboardModifiers;

static PyObject *KeyboardModifiers_get_left_control(
    PyKeyboardModifiers *self, void *closure)
{
	return PyBool_FromLong(self->modifiers.left_control);
}
static PyObject *KeyboardModifiers_get_right_control(
    PyKeyboardModifiers *self, void *closure)
{
	return PyBool_FromLong(self->modifiers.right_control);
}
static PyObject *KeyboardModifiers_get_left_shift(
    PyKeyboardModifiers *self, void *closure)
{
	return PyBool_FromLong(self->modifiers.left_shift);
}
static PyObject *KeyboardModifiers_get_right_shift(
    PyKeyboardModifiers *self, void *closure)
{
	return PyBool_FromLong(self->modifiers.right_shift);
}
static PyObject *KeyboardModifiers_get_left_alt(PyKeyboardModifiers *self, void *closure)
{
	return PyBool_FromLong(self->modifiers.left_alt);
}
static PyObject *KeyboardModifiers_get_right_alt(PyKeyboardModifiers *self, void *closure)
{
	return PyBool_FromLong(self->modifiers.right_alt);
}
static PyObject *KeyboardModifiers_get_left_meta(PyKeyboardModifiers *self, void *closure)
{
	return PyBool_FromLong(self->modifiers.left_meta);
}
static PyObject *KeyboardModifiers_get_right_meta(
    PyKeyboardModifiers *self, void *closure)
{
	return PyBool_FromLong(self->modifiers.right_meta);
}
static PyObject *KeyboardModifiers_get_left_super(
    PyKeyboardModifiers *self, void *closure)
{
	return PyBool_FromLong(self->modifiers.left_super);
}
static PyObject *KeyboardModifiers_get_right_super(
    PyKeyboardModifiers *self, void *closure)
{
	return PyBool_FromLong(self->modifiers.right_super);
}
static PyObject *KeyboardModifiers_get_left_hyper(
    PyKeyboardModifiers *self, void *closure)
{
	return PyBool_FromLong(self->modifiers.left_hyper);
}
static PyObject *KeyboardModifiers_get_right_hyper(
    PyKeyboardModifiers *self, void *closure)
{
	return PyBool_FromLong(self->modifiers.right_hyper);
}

static PyGetSetDef KeyboardModifiers_getsetters[] = {
	{ "left_control", (getter)KeyboardModifiers_get_left_control, NULL,
	    "left_control modifier status", NULL },
	{ "right_control", (getter)KeyboardModifiers_get_right_control, NULL,
	    "right_control modifier status", NULL },
	{ "left_shift", (getter)KeyboardModifiers_get_left_shift, NULL,
	    "left_shift modifier status", NULL },
	{ "right_shift", (getter)KeyboardModifiers_get_right_shift, NULL,
	    "right_shift modifier status", NULL },
	{ "left_alt", (getter)KeyboardModifiers_get_left_alt, NULL, "left_alt modifier status",
	    NULL },
	{ "right_alt", (getter)KeyboardModifiers_get_right_alt, NULL,
	    "right_alt modifier status", NULL },
	{ "left_meta", (getter)KeyboardModifiers_get_left_meta, NULL,
	    "left_meta modifier status", NULL },
	{ "right_meta", (getter)KeyboardModifiers_get_right_meta, NULL,
	    "right_meta modifier status", NULL },
	{ "left_super", (getter)KeyboardModifiers_get_left_super, NULL,
	    "left_super modifier status", NULL },
	{ "right_super", (getter)KeyboardModifiers_get_right_super, NULL,
	    "right_super modifier status", NULL },
	{ "left_hyper", (getter)KeyboardModifiers_get_left_hyper, NULL,
	    "left_hyper modifier status", NULL },
	{ "right_hyper", (getter)KeyboardModifiers_get_right_hyper, NULL,
	    "right_hyper modifier status", NULL },
	{ NULL } /* Sentinel */
};

static PyTypeObject PyKeyboardModifiersType = {
	PyVarObject_HEAD_INIT(NULL, 0).tp_name = "libvinput.KeyboardModifiers",
	.tp_basicsize = sizeof(PyKeyboardModifiers),
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_new = PyType_GenericNew,
	.tp_getset = KeyboardModifiers_getsetters,
};

typedef struct
{
	PyObject_HEAD KeyboardEvent event;
	PyKeyboardModifiers *modifiers;
} PyKeyboardEvent;

static PyObject *KeyboardEvent_get_pressed(PyKeyboardEvent *self, void *closure)
{
	return PyBool_FromLong(self->event.pressed);
}
static PyObject *KeyboardEvent_get_keychar(PyKeyboardEvent *self, void *closure)
{
	return PyUnicode_FromStringAndSize(&self->event.keychar, 1);
}
static PyObject *KeyboardEvent_get_keycode(PyKeyboardEvent *self, void *closure)
{
	return PyLong_FromUnsignedLong(self->event.keycode);
}
static PyObject *KeyboardEvent_get_keysym(PyKeyboardEvent *self, void *closure)
{
	return PyLong_FromUnsignedLong(self->event.keysym);
}
static PyObject *KeyboardEvent_get_modifiers(PyKeyboardEvent *self, void *closure)
{
	Py_INCREF(self->modifiers);
	return (PyObject *)self->modifiers;
}
static PyObject *KeyboardEvent_get_timestamp(PyKeyboardEvent *self, void *closure)
{
	return PyLong_FromSize_t(self->event.timestamp);
}

static PyGetSetDef KeyboardEvent_getsetters[] = {
	{ "pressed", (getter)KeyboardEvent_get_pressed, NULL, "Key pressed status", NULL },
	{ "keychar", (getter)KeyboardEvent_get_keychar, NULL, "ASCII character of the key",
	    NULL },
	{ "keycode", (getter)KeyboardEvent_get_keycode, NULL, "Key code value", NULL },
	{ "keysym", (getter)KeyboardEvent_get_keysym, NULL,
	    "Key symbol on X11 or virtual key code on Windows", NULL },
	{ "modifiers", (getter)KeyboardEvent_get_modifiers, NULL,
	    "Keyboard modifiers active during the event", NULL },
	{ "timestamp", (getter)KeyboardEvent_get_timestamp, NULL,
	    "Timestamp of the event in milliseconds", NULL },
	{ NULL } // Sentinel
};

static PyTypeObject PyKeyboardEventType = {
	PyVarObject_HEAD_INIT(NULL, 0).tp_name = "libvinput.KeyboardEvent",
	.tp_basicsize = sizeof(PyKeyboardEvent),
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_new = PyType_GenericNew,
	.tp_getset = KeyboardEvent_getsetters,
};

PyObject *KeyboardEvent_to_PyObject(KeyboardEvent *event)
{
	PyKeyboardModifiers *py_mods
	    = PyObject_New(PyKeyboardModifiers, &PyKeyboardModifiersType);
	if (!py_mods) {
		PyErr_SetString(PyExc_MemoryError, "Failed to create PyKeyboardModifiers object");
		return NULL;
	}
	py_mods->modifiers = event->modifiers;

	PyKeyboardEvent *py_event = PyObject_New(PyKeyboardEvent, &PyKeyboardEventType);
	if (!py_event) {
		PyErr_SetString(PyExc_MemoryError, "Failed to create PyKeyboardEvent object");
		return NULL;
	}
	py_event->event = *event;
	py_event->modifiers = py_mods;
	return (PyObject *)py_event;
}

typedef struct
{
	PyObject *py_callback;
} CallbackContext;

CallbackContext callback_context;

void python_keyboard_callback(KeyboardEvent event)
{
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();

	PyObject *py_event = KeyboardEvent_to_PyObject(&event);
	if (!py_event) {
		PyErr_Print();
		PyGILState_Release(gstate);
		return;
	}

	PyObject *result
	    = PyObject_CallFunctionObjArgs(callback_context.py_callback, py_event, NULL);
	if (!result)
		PyErr_Print();
	else
		Py_DECREF(result);

	Py_DECREF(py_event);
	PyGILState_Release(gstate);
}

static PyObject *py_EventListener_start(PyObject *self, PyObject *args)
{
	PyObject *py_listener_capsule;
	PyObject *py_callback;

	if (!PyArg_ParseTuple(args, "OO", &py_listener_capsule, &py_callback)) return NULL;

	if (!PyCallable_Check(py_callback)) {
		PyErr_SetString(PyExc_TypeError, "Parameter must be callable");
		return NULL;
	}

	EventListener *listener
	    = (EventListener *)PyCapsule_GetPointer(py_listener_capsule, "EventListener");
	if (!listener) return NULL;

	Py_INCREF(py_callback);
	callback_context.py_callback = py_callback;

	VInputError err = EventListener_start(listener, python_keyboard_callback);
	if (err != VINPUT_OK) {
		Py_DECREF(py_callback);
		PyErr_SetString(PyExc_RuntimeError, VInput_error_get_message(err));
		return NULL;
	}

	Py_RETURN_NONE;
}

static PyMethodDef VInputMethods[] = {
	{ "listener_create", py_EventListener_create, METH_VARARGS,
	    "Create an event listener." },
	{ "listener_destroy", py_EventListener_free, METH_VARARGS,
	    "Destroy an event listener." },
	{ "listener_start", py_EventListener_start, METH_VARARGS,
	    "Make an EventListener start listening. This is a blocking call." },
	{ NULL, NULL, 0, NULL } // Sentinel
};

static struct PyModuleDef vinputmodule = { PyModuleDef_HEAD_INIT,
	"libvinput", // name of the module
	NULL,        // module documentation, may be NULL
	-1,          // size of per-interpreter state of the module, or -1 if the
	             // module keeps state in global variables.
	VInputMethods };

PyMODINIT_FUNC PyInit_libvinput(void)
{
	PyObject *m = PyModule_Create(&vinputmodule);
	if (PyType_Ready(&PyKeyboardModifiersType) < 0) return NULL;
	if (PyType_Ready(&PyKeyboardEventType) < 0) return NULL;

	Py_INCREF(&PyKeyboardModifiersType);
	Py_INCREF(&PyKeyboardEventType);
	PyModule_AddObject(m, "KeyboardModifiers", (PyObject *)&PyKeyboardModifiersType);
	PyModule_AddObject(m, "KeyboardEvent", (PyObject *)&PyKeyboardEventType);
	return m;
}
