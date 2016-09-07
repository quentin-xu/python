#include "Python.h"
#include <stdio.h>
#include "c2cent/library/log/CWWLog2.h"


using namespace c2cent::library::wwlog;


static PyObject * CWWLog_QuickInitForCGI(PyObject *self, PyObject *args) {
    const char* pszLogName;
    const char* pszBaseLogFilePath=C2CWW_LOG_FILE_PATH;
    bool bIsCanRemote = false;
    uint32_t dwLogSwitch=WLOG_OPEN|WLOG_ERR_OPEN;

    
    if (!PyArg_ParseTuple(args, "ssii", &pszLogName, &pszBaseLogFilePath, &bIsCanRemote, &dwLogSwitch)){
        Py_RETURN_NONE;
    }

    int iRet = C2C_WWLOG->QuickInitForCGI(pszLogName, pszBaseLogFilePath, bIsCanRemote, dwLogSwitch);

    return (PyObject *) Py_BuildValue("i", iRet);

}

static PyObject * CWWLog_LogRunMsg(PyObject *self, PyObject *args) {

        const char* pszLog;
        if (!PyArg_ParseTuple(args, "s:Only accept string", &pszLog)){
                return NULL;
            }
        C2C_WWLOG->LogRunMsg("%s",pszLog);

    
    Py_RETURN_NONE;
}

static PyObject * CWWLog_LogDebugMsg(PyObject *self, PyObject *args) {
        const char* pszLog;
        if (!PyArg_ParseTuple(args, "s:Only accept string", &pszLog)){
                return NULL;
            }
        C2C_WWLOG->LogDebugMsg("%s",pszLog);

    Py_RETURN_NONE;
}

static PyObject * CWWLog_LogErrMsg(PyObject *self, PyObject *args) {
        const char* pszLog;
        if (!PyArg_ParseTuple(args, "s:Only accept string", &pszLog)){
                return NULL;
            }
        C2C_WWLOG->LogErrMsg("%s",pszLog);


    Py_RETURN_NONE;
}

static PyObject * CWWLog_LogPerformMsg(PyObject *self, PyObject *args) {
        const char* pszLog;
        if (!PyArg_ParseTuple(args, "s:Only accept string", &pszLog)){
	    return NULL;
        }
        C2C_WWLOG->LogPerformMsg("%s",pszLog);
    Py_RETURN_NONE;
}

static PyObject * CWWLog_LogKeyMsg(PyObject *self, PyObject *args) {
        const char* pszLog;
        if (!PyArg_ParseTuple(args, "s:Only accept string", &pszLog)){
	    return NULL;
        }
        C2C_WWLOG->LogKeyMsg("%s",pszLog);
    Py_RETURN_NONE;
}


static PyObject *SpamError;

static PyMethodDef SpamMethods[] =
        {
                { "LogRunMsg", CWWLog_LogRunMsg, METH_VARARGS, "Dont use directly" },
                { "LogDebugMsg", CWWLog_LogDebugMsg, METH_VARARGS, "Dont use directly" },
                { "LogErrMsg", CWWLog_LogErrMsg, METH_VARARGS, "Dont use directly" },
                { "LogPerformMsg", CWWLog_LogPerformMsg, METH_VARARGS, "Dont use directly" },
		{ "QuickInitForCGI", CWWLog_QuickInitForCGI, METH_VARARGS, "Dont use directly" },
		{ "LogKeyMsg", CWWLog_LogKeyMsg, METH_VARARGS, "Dont use directly"},
                { NULL, NULL, 0, NULL } /* Sentinel */
        };

PyMODINIT_FUNC initCWWLog(void) {

    PyObject *m = Py_InitModule("CWWLog", SpamMethods);
    if (m == NULL)
        return;

    /* Add some symbolic constants to the module */
    PyObject *d = PyModule_GetDict(m);

    PyObject *close = PyInt_FromLong((long) WLOG_CLOSE);
    PyObject *open = PyInt_FromLong((long) WLOG_OPEN);
    PyObject *key = PyInt_FromLong((long) WLOG_KEY_OPEN);
    PyObject *err = PyInt_FromLong((long) WLOG_ERR_OPEN);
    PyObject *per = PyInt_FromLong((long) WLOG_PER_OPEN);
    PyObject *run = PyInt_FromLong((long) WLOG_RUN_OPEN);
    PyObject *deg = PyInt_FromLong((long) WLOG_DEG_OPEN);
    PyObject *all = PyInt_FromLong((long) WLOG_ALL_OPEN);

    PyDict_SetItemString(d, "WLOG_CLOSE", close);
    PyDict_SetItemString(d, "WLOG_OPEN", open);
    PyDict_SetItemString(d, "WLOG_KEY_OPEN", key);
    PyDict_SetItemString(d, "WLOG_ERR_OPEN", err);
    PyDict_SetItemString(d, "WLOG_PER_OPEN", per);
    PyDict_SetItemString(d, "WLOG_RUN_OPEN", run);
    PyDict_SetItemString(d, "WLOG_DEG_OPEN", deg);
    PyDict_SetItemString(d, "WLOG_ALL_OPEN", all);

    
    SpamError = PyErr_NewException("CWWLog.error", NULL, NULL);
    Py_INCREF(SpamError);
    PyModule_AddObject(m, "error", SpamError);
}

