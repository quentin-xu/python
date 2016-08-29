#include "Python.h"

#include <stdio.h>

#include "config_client_set.h"

/**
 *@brief 根据ServiceName,set,key获得IP:Port(Mod+L5动态剔除方式)
 *@param Command         命令号值
 *@param cSet            set id
 *@param dwKey           用来取值的模数
 */
static PyObject * confg4set_getServiceAddressByModAndL5(PyObject *self, PyObject *args)
{
  const char* pszServiceName = NULL;
  int iSet = 0;
  int iKey = 0;

  if (!PyArg_ParseTuple(args, "sii", &pszServiceName, &iSet, &iKey))
    return (PyObject *)Py_BuildValue("s", "");

  const CInterAddr& oAddr = CONFIG_SET->getServiceAddressByModAndL5(pszServiceName, iSet, iKey);

  return (PyObject *)Py_BuildValue("(s,i)", oAddr.getHost(), oAddr.getPort());

}


/**
 *@brief 根据CommandId,Set获得IP:Port(Mod+L5动态剔除方式)
 *@param Command         命令号值
 *@param cSet            set id
 *@param dwKey           用来取值的模数
 */
static PyObject * confg4set_getAppAddressByModAndL5(PyObject *self, PyObject *args)
{
  int64_t ddwCommand = 0;
  int iSet = 0;
  int iKey = 0;

  if (!PyArg_ParseTuple(args, "lii", &ddwCommand, &iSet, &iKey))
    return (PyObject *)Py_BuildValue("s", "");

  CInterAddr oAddr;
  if(ddwCommand <= 0xFFFF)
    oAddr = CONFIG_SET->getAppAddressByModAndL5((uint16_t)ddwCommand, iSet, iKey);
  else
    oAddr = CONFIG_SET->getAppAddressByModAndL5((uint32_t)ddwCommand, iSet, iKey);

  return (PyObject *)Py_BuildValue("(s,i)", oAddr.getHost(), oAddr.getPort());
}

/**
 * @brief 根据服务名取到起所有的IP&port列表
 * @param pszServiceName 服务名
 * @param wSet Set索引
 * @return 用vector存储的所有CInterAddr服务IP:Port
 */
static PyObject * confg4set_getAllAddrBySvcNameAndSet(PyObject *self, PyObject *args)
{
  const char* pszServiceName = NULL;
  int iSet = 0;

  if (!PyArg_ParseTuple(args, "si", &pszServiceName, &iSet))
    return (PyObject *)Py_BuildValue("s", "");

  const std::vector<CInterAddr>& vecAddr = CONFIG_SET->getAllAddrBySvcNameAndSet(pszServiceName, iSet);

  PyObject* pResult = PyList_New(vecAddr.size());

  for(std::size_t i = 0; i < vecAddr.size(); ++i)
    PyList_SetItem(pResult, i, Py_BuildValue("(s,i)", vecAddr[i].getHost(), vecAddr[i].getPort()));   

  return PyList_AsTuple(pResult);

}
    
/**
 * @brief 根据服务名取到起所有的IP&port列表
 * @param pszServiceName 服务名
 * @param wSet Set索引
 * @return 用vector存储的所有CInterAddr服务IP:Port
 */
static PyObject * confg4set_getAllAddrByCmdAndSet(PyObject *self, PyObject *args)
{
  int64_t ddwCommand = 0;
  int iSet = 0;

  if (!PyArg_ParseTuple(args, "li", &ddwCommand, &iSet))
    return (PyObject *)Py_BuildValue("s", "");

  std::vector<CInterAddr> vecAddr;
  if(ddwCommand <= 0xFFFF)
    vecAddr = CONFIG_SET->getAllAddrByCmdAndSet((uint16_t)ddwCommand, iSet);
  else
    vecAddr = CONFIG_SET->getAllAddrByCmdAndSet((uint32_t)ddwCommand, iSet);

  PyObject* pResult = PyList_New(vecAddr.size());

  for(std::size_t i = 0; i < vecAddr.size(); ++i)
    PyList_SetItem(pResult, i, Py_BuildValue("(s,i)", vecAddr[i].getHost(), vecAddr[i].getPort()));
   

  return PyList_AsTuple(pResult);

}
    
static PyObject *SpamError;

static PyMethodDef SpamMethods[] = {
    {"getServiceAddressByModAndL5",  confg4set_getServiceAddressByModAndL5, METH_VARARGS, "getServiceAddressByModAndL5(const char* pszServiceName, uint8_t cSet, uint32_t dwKey)"},
    {"getAppAddressByModAndL5",  confg4set_getAppAddressByModAndL5, METH_VARARGS, "getAppAddressByModAndL5(uint32_t dwCommand, uint8_t cSet, uint32_t dwKey)"},
    {"getAllAddrBySvcNameAndSet",  confg4set_getAllAddrBySvcNameAndSet, METH_VARARGS, "getAllAddrBySvcNameAndSet(const char* pszServiceName, uint8_t cSet)"},
    {"getAllAddrByCmdAndSet",  confg4set_getAllAddrByCmdAndSet, METH_VARARGS, "getAllAddrByCmdAndSet(uint32_t dwCommand, uint8_t cSet)"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC initconfg4set(void)
{
  int iRetCode=CONFIG_SET->Initialize();
  if(iRetCode!=0)
  {
    printf("CONFIG_SET->Initialize Failed. ErrCode:%d ErrMsg:%s", iRetCode, CONFIG_SET->getLastErrMsg());
  } 

  PyObject *m;

  m = Py_InitModule("confg4set", SpamMethods);
  if (m == NULL)
    return;

  SpamError = PyErr_NewException("confg4set.error", NULL, NULL);
  Py_INCREF(SpamError);
  PyModule_AddObject(m, "error", SpamError);

}

