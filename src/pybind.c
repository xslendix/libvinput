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

static PyTypeObject PyKeyboardModifiersType = {
	PyVarObject_HEAD_INIT(NULL, 0).tp_name = "libvinput.KeyboardModifiers",
	.tp_basicsize = sizeof(PyKeyboardModifiers),
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_new = PyType_GenericNew,
};

typedef struct
{
	PyObject_HEAD KeyboardEvent event;
	PyKeyboardModifiers *modifiers;
} PyKeyboardEvent;

static PyTypeObject PyKeyboardEventType = {
	PyVarObject_HEAD_INIT(NULL, 0).tp_name = "libvinput.KeyboardEvent",
	.tp_basicsize = sizeof(PyKeyboardEvent),
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_new = PyType_GenericNew,
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
