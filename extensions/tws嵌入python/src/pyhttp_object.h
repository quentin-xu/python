#include <Python.h>
#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>
#include <cxxabi.h>

#include "webframework3/http_request.h"
#include "webframework3/http_response.h"


using namespace web_solution::web_framework;

namespace pyhttp{

template<typename Type>
class CPyObjContainer
{
public:
  PyObject_HEAD
  Type* ptr; 
};

template<typename _Type, typename _Args>
class GenMethod
{
  //using namespace std;
public:
  typedef _Type Type;
  typedef _Args Args;
  template<Args(Type::*func)(void)const>
  void Register(std::vector<PyMethodDef>& PyMethods, const char* name){
    std::size_t i = std::string(name).find_last_of(":&");
    i = (i == std::string::npos ? 0 : i + 1);
    PyMethodDef PyMethod = {(char *)name + i, &GetMethod<func>, METH_VARARGS|METH_NOARGS, name + i};
    PyMethods.push_back(PyMethod);
  }

  template<Args(Type::*func)(void)const>
  static PyObject* GetMethod(PyObject* self, PyObject *args){
    //const std::string typeName(typeid(CPyObjContainer<Type>).name());
    const std::string typeName = abi::__cxa_demangle(typeid(Type).name(), NULL, NULL, NULL);
    if (typeName != self->ob_type->tp_name){
      PyErr_SetString(PyExc_TypeError, (typeName + " can use " + self->ob_type->tp_name + " method").c_str());
      return NULL;
    }
    
    Type* arg1 = reinterpret_cast<CPyObjContainer<Type> *>(self)->ptr;
    const Args& result = (arg1->*func)();
    PyObject* resultobj = PyInt_FromLong(result);
    return resultobj;
  }


  template<void(Type::*func)(Args)>
  void Register(std::vector<PyMethodDef>& PyMethods, const char* name){
    std::size_t i = std::string(name).find_last_of(":&");
    i = (i == std::string::npos ? 0 : i + 1);
    PyMethodDef PyMethod = {(char *)name + i, &SetMethod<func>, METH_VARARGS|METH_O, name + i};
    PyMethods.push_back(PyMethod);
  }

  template<void(Type::*func)(Args)>
  static PyObject* SetMethod(PyObject* self, PyObject *args){
    //const std::string typeName(typeid(CPyObjContainer<Type>).name());
    const std::string typeName = abi::__cxa_demangle(typeid(Type).name(), NULL, NULL, NULL);
    if (typeName != self->ob_type->tp_name){
      PyErr_SetString(PyExc_TypeError, std::string("Only " + typeName + " can use this method!").c_str());
      return NULL;
    }

    long v = 0;

    if (PyInt_Check(args)){
      v = PyInt_AsLong(args);
    }else if (PyLong_Check(args)){
      v = PyLong_AsLong(args);
    }
    else {
      PyErr_SetString(PyExc_TypeError, std::string("Error Type!").c_str());
      return NULL;
    }

    Args cv = v;
    if (cv != v){
      PyErr_SetString(PyExc_TypeError, std::string("Value overflow!").c_str());
      return NULL;
    }
    
    Type* arg1 = reinterpret_cast<CPyObjContainer<Type> *>(self)->ptr;
    (arg1->*func)(cv); 

    Py_RETURN_NONE;
  }

};

template<typename _Type>
class GenMethod<_Type, const std::string&>
{
  //using namespace std;
public:
  typedef _Type Type;
  typedef const std::string& Args;

  template<Args(Type::*func)(void)const>
  void Register(std::vector<PyMethodDef>& PyMethods, const char* name){
    std::size_t i = std::string(name).find_last_of(":&");
    i = (i == std::string::npos ? 0 : i + 1);
    PyMethodDef PyMethod = {(char *)name + i, &GetMethod<func>, METH_VARARGS|METH_NOARGS, name + i};
    PyMethods.push_back(PyMethod);
  }

  template<Args(Type::*func)(void)const>
  static PyObject* GetMethod(PyObject* self, PyObject *args){
    //const std::string typeName(typeid(CPyObjContainer<Type>).name());
    const std::string typeName = abi::__cxa_demangle(typeid(Type).name(), NULL, NULL, NULL);
    if (typeName != self->ob_type->tp_name){
      PyErr_SetString(PyExc_TypeError, std::string("Only " + typeName + " can use this method!").c_str());
      return NULL;
    }
    
    Type* arg1 = reinterpret_cast<CPyObjContainer<Type> *>(self)->ptr;
    Args result = (arg1->*func)();
  //PyObject* resultobj = PyInt_FromLong(result);
    PyObject* resultobj = PyString_FromStringAndSize(result.c_str(), Py_ssize_t(result.length()));
    return resultobj;
  }


  template<void(Type::*func)(Args)>
  void Register(std::vector<PyMethodDef>& PyMethods, const char* name){
    std::size_t i = std::string(name).find_last_of(":&");
    i = (i == std::string::npos ? 0 : i + 1);
    PyMethodDef PyMethod = {(char *)name + i, &SetMethod<func>, METH_VARARGS|METH_O, name + i};
    PyMethods.push_back(PyMethod);
  }

  template<void(Type::*func)(Args)>
  static PyObject* SetMethod(PyObject* self, PyObject *args){
    //const std::string typeName(typeid(CPyObjContainer<Type>).name());
    const std::string typeName = abi::__cxa_demangle(typeid(Type).name(), NULL, NULL, NULL);
    if (typeName != self->ob_type->tp_name){
      PyErr_SetString(PyExc_TypeError, std::string("Only " + typeName + " can use this method!").c_str());
      return NULL;
    }

    if (!PyString_Check(args)){
      PyErr_SetString(PyExc_TypeError, std::string("Error Type!").c_str());
      return NULL;
    }

    char* str = NULL;
    Py_ssize_t i = 0;
    PyString_AsStringAndSize(args, &str, &i);

    Type* arg1 = reinterpret_cast<CPyObjContainer<Type> *>(self)->ptr;
    (arg1->*func)(std::string(str, i)); 

    Py_RETURN_NONE;

  }

};



//template<typename Type, typename In1, void(Type::*func)(In1)>
template<typename Type, typename In1>
inline GenMethod<Type, In1> FetchArgs(void(Type::*func)(In1))
{
  return GenMethod<Type, In1>();
}


template<typename Type, typename Out1>
inline GenMethod<Type, Out1> FetchArgs(Out1(Type::*func)(void)const)
{
  return GenMethod<Type, Out1>();
}

#define REGISTER(method) do{\
    FetchArgs(method).Register<method>(pyMethods, #method); \
}while(0)

void PyHttp_dealloc(PyObject *v)
{

}

int PyHttp_compare(PyObject *v, PyObject *w)
{
  return (long)v < (long)w ? -1 : ((long)v > (long)w ? 1 : 0);
}

PyObject * PyHttp_repr(PyObject *v)
{
  return PyString_FromFormat("<TWS Build-in Object at %p>", (void *)v);
}



PyObject* GetVal(PyObject* self, PyObject *args){
    
    const std::string typeName = abi::__cxa_demangle(typeid(CActionParams).name(), NULL, NULL, NULL);

    if (typeName != self->ob_type->tp_name){
        PyErr_SetString(PyExc_TypeError, (typeName + " can use " + self->ob_type->tp_name + " method").c_str());
        return NULL;
    }

    if (!PyString_Check(args)){
        PyErr_SetString(PyExc_TypeError, std::string("Error Type!").c_str());
        return NULL;
    }

    CActionParams* arg1 = reinterpret_cast<CPyObjContainer<CActionParams> *>(self)->ptr;
    char* str = NULL;
    Py_ssize_t i = 0;
    PyString_AsStringAndSize(args, &str, &i);


    std::vector<std::string>  vecParam;
    if(arg1->GetVal(str, vecParam) != 0 || vecParam.empty()){
      Py_RETURN_NONE;
    }

    PyObject* resultobj = NULL;

    if(vecParam.size() == 1)
    {
        resultobj = PyString_FromString(vecParam.front().c_str());
    }else {
        resultobj = PyList_New(vecParam.size());
        for (size_t i = 0; i < vecParam.size(); ++i){
	  PyObject* obj = PyString_FromString(vecParam[i].c_str());
            PyList_SET_ITEM(resultobj, i, obj);
        }
    }


    return resultobj;
}

PyObject* GetCookieVal(PyObject* self, PyObject *args){
    
    const std::string typeName = abi::__cxa_demangle(typeid(CActionParams).name(), NULL, NULL, NULL);

    if (typeName != self->ob_type->tp_name){
        PyErr_SetString(PyExc_TypeError, (typeName + " can use " + self->ob_type->tp_name + " method").c_str());
        return NULL;
    }

    if (!PyString_Check(args)){
        PyErr_SetString(PyExc_TypeError, std::string("Error Type!").c_str());
        return NULL;
    }

    CActionParams* arg1 = reinterpret_cast<CPyObjContainer<CActionParams> *>(self)->ptr;
    char* str = NULL;
    Py_ssize_t i = 0;
    PyString_AsStringAndSize(args, &str, &i);


    std::string strParam;
    if(arg1->GetCookieVal(str, strParam) != 0){
      Py_RETURN_NONE;
    }

    PyObject* resultobj = PyString_FromString(strParam.c_str());
   
    return resultobj;
}


PyTypeObject* GetActionParamsType()
{
  using namespace web_solution::comm;
  
  static bool isRegister = false;

  //const char* name = abi::__cxa_demangle(typeid(CPyObjContainer<CHttpRequest2>).name(), NULL, NULL, NULL);
  //const char* name = typeid(CPyObjContainer<CHttpRequest2>).name();
  const char* name = abi::__cxa_demangle(typeid(CActionParams).name(), NULL, NULL, NULL);

  std::cout << name << std::endl;


  static PyTypeObject pyType = {
    PyObject_HEAD_INIT(NULL)
    0,                                    /* ob_size */
    (char *)name,                         /* tp_name */
    sizeof(CPyObjContainer<CActionParams>),/* tp_basicsize */
    0,                                    /* tp_itemsize */
    (destructor)PyHttp_dealloc,     /* tp_dealloc */
    0,    /* tp_print */
    (getattrfunc)0,                       /* tp_getattr */
    (setattrfunc)0,                       /* tp_setattr */
    (cmpfunc)PyHttp_compare,        /* tp_compare */
    (reprfunc)PyHttp_repr,          /* tp_repr */
    0,              /* tp_as_number */
    0,                                    /* tp_as_sequence */
    0,                                    /* tp_as_mapping */
    (hashfunc)0,                          /* tp_hash */
    (ternaryfunc)0,                       /* tp_call */
    0,    /* tp_str */
    PyObject_GenericGetAttr,              /* tp_getattro */
    0,                                    /* tp_setattro */
    0,                                    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                   /* tp_flags */
    0,                       /* tp_doc */
    0,                                    /* tp_traverse */
    0,                                    /* tp_clear */
    (richcmpfunc)0,/* tp_richcompare */
    0,                                    /* tp_weaklistoffset */
#if PY_VERSION_HEX >= 0x02020000
    0,                                    /* tp_iter */
    0,                                    /* tp_iternext */
    0,                   /* tp_methods */
    0,                                    /* tp_members */
    0,                                    /* tp_getset */
    0,                                    /* tp_base */
    0,                                    /* tp_dict */
    0,                                    /* tp_descr_get */
    0,                                    /* tp_descr_set */
    0,                                    /* tp_dictoffset */
    0,                                    /* tp_init */
    0,                                    /* tp_alloc */
    0,                                    /* tp_new */
    0,                                    /* tp_free */
    0,                                    /* tp_is_gc */
    0,                                    /* tp_bases */
    0,                                    /* tp_mro */
    0,                                    /* tp_cache */
    0,                                    /* tp_subclasses */
    0,                                    /* tp_weaklist */
#endif
#if PY_VERSION_HEX >= 0x02030000
    0,                                    /* tp_del */
#endif
#if PY_VERSION_HEX >= 0x02060000
    0,                                    /* tp_version_tag */
#endif
#if PY_VERSION_HEX >= 0x03040000
    0,                                    /* tp_finalize */
#endif
#ifdef COUNT_ALLOCS
    0,                                    /* tp_allocs */
    0,                                    /* tp_frees */
    0,                                    /* tp_maxalloc */
#if PY_VERSION_HEX >= 0x02050000
    0,                                    /* tp_prev */
#endif
    0                                     /* tp_next */
#endif
  };

  
  if(!isRegister){
 
      static PyMethodDef PyMethods[] = {{(char *)"GetVal", &GetVal, METH_VARARGS|METH_O, "GetVal"},
					{(char *)"GetCookieVal", &GetCookieVal, METH_VARARGS|METH_O, "GetCookieVal"},
					{NULL, NULL}
      };
      
      isRegister = true;

      pyType.tp_methods = PyMethods;
      PyType_Ready(&pyType);
  }

  return &pyType;

}


PyObject* UpdateCookieVal(PyObject* self, PyObject *args)
{
    const std::string typeName = abi::__cxa_demangle(typeid(CWebViewData).name(), NULL, NULL, NULL);

    if (typeName != self->ob_type->tp_name){
        PyErr_SetString(PyExc_TypeError, std::string("Only " + typeName + " can use this method!").c_str());
        return NULL;
    }
    
    char *sName, *sVal, *sTime, *sDomain;
    if (!PyArg_ParseTuple(args, "ssss:UpdateCookieVal", &sName, &sVal, &sTime, &sDomain)){
        PyErr_SetString(PyExc_TypeError, std::string("Cookie Name value time domain all must be string").c_str());
        return NULL;
    }
    
    CWebViewData* arg1 = reinterpret_cast<CPyObjContainer<CWebViewData> *>(self)->ptr;


    arg1->UpdateCookieVal(sName, sVal, sTime, sDomain, false);
   
    Py_RETURN_NONE;
}


PyObject* DelCookieVal(PyObject* self, PyObject *args)
{
    const std::string typeName = abi::__cxa_demangle(typeid(CWebViewData).name(), NULL, NULL, NULL);

    if (typeName != self->ob_type->tp_name){
        PyErr_SetString(PyExc_TypeError, std::string("Only " + typeName + " can use this method!").c_str());
        return NULL;
    }
    
    if (!PyString_Check(args)){
        PyErr_SetString(PyExc_TypeError, std::string("Error Type!").c_str());
        return NULL;
    }
    
    CWebViewData* arg1 = reinterpret_cast<CPyObjContainer<CWebViewData> *>(self)->ptr;
    char* str = NULL;
    Py_ssize_t i = 0;
    PyString_AsStringAndSize(args, &str, &i);

    std::string strParam;
    arg1->DelCookieVal(str);
   
    Py_RETURN_NONE;
}


PyObject* RemoveCookieVal(PyObject* self, PyObject *args)
{
    const std::string typeName = abi::__cxa_demangle(typeid(CWebViewData).name(), NULL, NULL, NULL);

    if (typeName != self->ob_type->tp_name){
        PyErr_SetString(PyExc_TypeError, std::string("Only " + typeName + " can use this method!").c_str());
        return NULL;
    }
    
    if (!PyString_Check(args)){
        PyErr_SetString(PyExc_TypeError, std::string("Error Type!").c_str());
        return NULL;
    }
    
    CWebViewData* arg1 = reinterpret_cast<CPyObjContainer<CWebViewData> *>(self)->ptr;
    char* str = NULL;
    Py_ssize_t i = 0;
    PyString_AsStringAndSize(args, &str, &i);

    std::string strParam;
    arg1->RemoveCookieVal(str);
   
    Py_RETURN_NONE;
}

PyTypeObject* GetWebViewDataType()
{
  using namespace web_solution::comm;
  
  static bool isRegister = false;

  //const char* name = abi::__cxa_demangle(typeid(CPyObjContainer<CHttpRequest2>).name(), NULL, NULL, NULL);
  //const char* name = typeid(CPyObjContainer<CHttpRequest2>).name();
  const char* name = abi::__cxa_demangle(typeid(CWebViewData).name(), NULL, NULL, NULL);
  std::cout << name << std::endl;


  static PyTypeObject pyType = {
    PyObject_HEAD_INIT(NULL)
    0,                                    /* ob_size */
    (char *)name,                         /* tp_name */
    sizeof(CPyObjContainer<CWebViewData>),/* tp_basicsize */
    0,                                    /* tp_itemsize */
    (destructor)PyHttp_dealloc,     /* tp_dealloc */
    0,    /* tp_print */
    (getattrfunc)0,                       /* tp_getattr */
    (setattrfunc)0,                       /* tp_setattr */
    (cmpfunc)PyHttp_compare,        /* tp_compare */
    (reprfunc)PyHttp_repr,          /* tp_repr */
    0,              /* tp_as_number */
    0,                                    /* tp_as_sequence */
    0,                                    /* tp_as_mapping */
    (hashfunc)0,                          /* tp_hash */
    (ternaryfunc)0,                       /* tp_call */
    0,    /* tp_str */
    PyObject_GenericGetAttr,              /* tp_getattro */
    0,                                    /* tp_setattro */
    0,                                    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                   /* tp_flags */
    0,                       /* tp_doc */
    0,                                    /* tp_traverse */
    0,                                    /* tp_clear */
    (richcmpfunc)0,/* tp_richcompare */
    0,                                    /* tp_weaklistoffset */
#if PY_VERSION_HEX >= 0x02020000
    0,                                    /* tp_iter */
    0,                                    /* tp_iternext */
    0,                   /* tp_methods */
    0,                                    /* tp_members */
    0,                                    /* tp_getset */
    0,                                    /* tp_base */
    0,                                    /* tp_dict */
    0,                                    /* tp_descr_get */
    0,                                    /* tp_descr_set */
    0,                                    /* tp_dictoffset */
    0,                                    /* tp_init */
    0,                                    /* tp_alloc */
    0,                                    /* tp_new */
    0,                                    /* tp_free */
    0,                                    /* tp_is_gc */
    0,                                    /* tp_bases */
    0,                                    /* tp_mro */
    0,                                    /* tp_cache */
    0,                                    /* tp_subclasses */
    0,                                    /* tp_weaklist */
#endif
#if PY_VERSION_HEX >= 0x02030000
    0,                                    /* tp_del */
#endif
#if PY_VERSION_HEX >= 0x02060000
    0,                                    /* tp_version_tag */
#endif
#if PY_VERSION_HEX >= 0x03040000
    0,                                    /* tp_finalize */
#endif
#ifdef COUNT_ALLOCS
    0,                                    /* tp_allocs */
    0,                                    /* tp_frees */
    0,                                    /* tp_maxalloc */
#if PY_VERSION_HEX >= 0x02050000
    0,                                    /* tp_prev */
#endif
    0                                     /* tp_next */
#endif
  };

  
  if(!isRegister){

      isRegister = true;
      static PyMethodDef PyMethods[] = {{(char *)"UpdateCookieVal", &UpdateCookieVal, METH_VARARGS, "UpdateCookieVal(const std::string& sName, const std::string& sVal, const std::string& sTime, const std::string& sDomain)"},
              {(char *)"DelCookieVal", &DelCookieVal, METH_VARARGS|METH_O, "DelCookieVal(const std::string& sName)"},
              {(char *)"RemoveCookieVal", &RemoveCookieVal, METH_VARARGS|METH_O, "RemoveCookieVal(const std::string& sName)"},
				 {NULL, NULL}
      };
      
      pyType.tp_methods = PyMethods;
      PyType_Ready(&pyType);
  }

  return &pyType;

}


PyTypeObject* GetWebSvcCntlType()
{
  using namespace web_solution::comm;
  
  static bool isRegister = false;
  static std::vector<PyMethodDef> pyMethods;

  //const char* name = abi::__cxa_demangle(typeid(CPyObjContainer<CHttpRequest2>).name(), NULL, NULL, NULL);
  //const char* name = typeid(CPyObjContainer<CHttpRequest2>).name();
  const char* name = abi::__cxa_demangle(typeid(CWebSvcCntl).name(), NULL, NULL, NULL);

  std::cout << name << std::endl;


  static PyTypeObject pyType = {
    PyObject_HEAD_INIT(NULL)
    0,                                    /* ob_size */
    (char *)name,                         /* tp_name */
    sizeof(CPyObjContainer<CWebSvcCntl>),/* tp_basicsize */
    0,                                    /* tp_itemsize */
    (destructor)PyHttp_dealloc,     /* tp_dealloc */
    0,    /* tp_print */
    (getattrfunc)0,                       /* tp_getattr */
    (setattrfunc)0,                       /* tp_setattr */
    (cmpfunc)PyHttp_compare,        /* tp_compare */
    (reprfunc)PyHttp_repr,          /* tp_repr */
    0,              /* tp_as_number */
    0,                                    /* tp_as_sequence */
    0,                                    /* tp_as_mapping */
    (hashfunc)0,                          /* tp_hash */
    (ternaryfunc)0,                       /* tp_call */
    0,    /* tp_str */
    PyObject_GenericGetAttr,              /* tp_getattro */
    0,                                    /* tp_setattro */
    0,                                    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                   /* tp_flags */
    0,                       /* tp_doc */
    0,                                    /* tp_traverse */
    0,                                    /* tp_clear */
    (richcmpfunc)0,/* tp_richcompare */
    0,                                    /* tp_weaklistoffset */
#if PY_VERSION_HEX >= 0x02020000
    0,                                    /* tp_iter */
    0,                                    /* tp_iternext */
    0,                   /* tp_methods */
    0,                                    /* tp_members */
    0,                                    /* tp_getset */
    0,                                    /* tp_base */
    0,                                    /* tp_dict */
    0,                                    /* tp_descr_get */
    0,                                    /* tp_descr_set */
    0,                                    /* tp_dictoffset */
    0,                                    /* tp_init */
    0,                                    /* tp_alloc */
    0,                                    /* tp_new */
    0,                                    /* tp_free */
    0,                                    /* tp_is_gc */
    0,                                    /* tp_bases */
    0,                                    /* tp_mro */
    0,                                    /* tp_cache */
    0,                                    /* tp_subclasses */
    0,                                    /* tp_weaklist */
#endif
#if PY_VERSION_HEX >= 0x02030000
    0,                                    /* tp_del */
#endif
#if PY_VERSION_HEX >= 0x02060000
    0,                                    /* tp_version_tag */
#endif
#if PY_VERSION_HEX >= 0x03040000
    0,                                    /* tp_finalize */
#endif
#ifdef COUNT_ALLOCS
    0,                                    /* tp_allocs */
    0,                                    /* tp_frees */
    0,                                    /* tp_maxalloc */
#if PY_VERSION_HEX >= 0x02050000
    0,                                    /* tp_prev */
#endif
    0                                     /* tp_next */
#endif
  };

  
  if(!isRegister){
 
      REGISTER(&CWebSvcCntl::SetResult);
      REGISTER(&CWebSvcCntl::GetResult);

      REGISTER(&CWebSvcCntl::SetUpdateTime);
      REGISTER(&CWebSvcCntl::GetUpdateTime);

      REGISTER(&CWebSvcCntl::SetWebCacheExpireTime);
      REGISTER(&CWebSvcCntl::GetWebCacheExpireTime);

      REGISTER(&CWebSvcCntl::SetBrowserCacheExpireTime);
      REGISTER(&CWebSvcCntl::GetBrowserCacheExpireTime);

      REGISTER(&CWebSvcCntl::SetEtags);
      REGISTER(&CWebSvcCntl::GetEtags);

      REGISTER(&CWebSvcCntl::SetMimeType);
      REGISTER(&CWebSvcCntl::GetMimeType);

      REGISTER(&CWebSvcCntl::SetMimeEncoding);
      REGISTER(&CWebSvcCntl::GetMimeEncoding);

      REGISTER(&CWebSvcCntl::SetUseWebCache);
      REGISTER(&CWebSvcCntl::GetUseWebCache);

      REGISTER(&CWebSvcCntl::SetContentEncoding);
      REGISTER(&CWebSvcCntl::GetContentEncoding);

      REGISTER(&CWebSvcCntl::SetLocation);
      REGISTER(&CWebSvcCntl::GetLocation);

      REGISTER(&CWebSvcCntl::SetOutputHead);
      REGISTER(&CWebSvcCntl::GetOutputHead);

      isRegister = true;

      PyMethodDef PyMethod = {NULL, NULL};
      pyMethods.push_back(PyMethod);

      pyType.tp_methods = &pyMethods[0];
      PyType_Ready(&pyType);
  }

  return &pyType;

}

}

