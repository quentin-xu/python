'''
Created on 2012-4-20

@author: yilin
'''

from struct import pack,unpack
from string import strip
from types import *
import byte_stream
import sys

builtin_type = set((IntType, LongType, FloatType, BooleanType, StringType, UnicodeType))



def is_stl_type(name):
    if name.startswith('stl_'):
        i = name.find('<')
        return name[0: i if i > 0 else len(name)] in dir(sys.modules[__name__])
    return False

def get_stl_type(name):
    i = name.find('<')
    return getattr(sys.modules[__name__], name[0: i if i > 0 else len(name)])
    
class uint8_t():
    def __init__(self,value=0):
        self.value=value
        
    def getValue(self):
        return self.value
    
    def setValue(self,value):
        self.value=value
    
    def getSize(self):
        return 1;
    
    def getBytes(self):
        return pack('B',self.value)
    
    @staticmethod
    def s_getBytes(value):
        return pack('B', value)
    
    def setBytes(self,buffer):
        self.value=unpack('B',buffer)[0] 

    @staticmethod   
    def s_fromBytes(buffer):
        return unpack('B',buffer)[0]

class int8_t:
    def __init__(self,value=0):
        self.value=value
        
    def getValue(self):
        return self.value
    
    def setValue(self,value):
        self.value=value
    
    def getSize(self):
        return 1;
    
    def getBytes(self):
        return pack('b',self.value)
    
    @staticmethod
    def s_getBytes(value):
        return pack('b', value)
    
    def setBytes(self,buffer):
        self.value=unpack('b',str(buffer))[0]
        
    @staticmethod   
    def s_fromBytes(buffer):
        return unpack('b',str(buffer))[0]
        
class uint16_t:
    def __init__(self,value=0):
        self.value=value
        
    def getValue(self):
        return self.value
    
    def setValue(self,value):
        self.value=value
    
    def getSize(self):
        return 2;
    
    def getBytes(self):
        return pack('>H',self.value)
    
    @staticmethod
    def s_getBytes(value):
        return pack('>H', value)
    
    def setBytes(self,buffer):
        self.value=unpack('>H',str(buffer))[0]

    @staticmethod   
    def s_fromBytes(buffer):
        return unpack('>H',str(buffer))[0]

class int16_t:
    def __init__(self,value=0):
        self.value=value
        
    def getValue(self):
        return self.value
    
    def setValue(self,value):
        self.value=value
    
    def getSize(self):
        return 2;
    
    def getBytes(self):
        return pack('>h',self.value)
    
    @staticmethod
    def s_getBytes(value):
        return pack('>h', value)
    
    def setBytes(self,buffer):
        self.value=unpack('>h',str(buffer))[0]

    @staticmethod   
    def s_fromBytes(buffer):
        return unpack('>h',str(buffer))[0]        

class uint32_t:
    def __init__(self,value=0):
        self.value=value
        
    def getValue(self):
        return self.value
    
    def setValue(self,value):
        self.value=value
    
    def getSize(self):
        return 4;
    
    def getBytes(self):
        return pack('>I',self.value)
    
    @staticmethod
    def s_getBytes(value):
        return pack('>I', value)
    
    def setBytes(self,buffer):
        self.value=unpack('>I',str(buffer))[0]
    
    @staticmethod   
    def s_fromBytes(buffer):
        return unpack('>I',str(buffer))[0]
        
class int32_t:
    def __init__(self,value=0):
        self.value=value
        
    def getValue(self):
        return self.value
    
    def setValue(self,value):
        self.value=value
    
    def getSize(self):
        return 4;
    
    def getBytes(self):
        return pack('>i',self.value)
    
    @staticmethod
    def s_getBytes(value):
        return pack('>i', value)
    
    def setBytes(self,buffer):
        self.value=unpack('>i',str(buffer))[0]
        
    @staticmethod   
    def s_fromBytes(buffer):
        return unpack('>i',str(buffer))[0]
        
class uint64_t:
    def __init__(self,value=0):
        self.value=value
        
    def getValue(self):
        return self.value
    
    def setValue(self,value):
        self.value=value
    
    def getSize(self):
        return 17;
    
    def getBytes(self):
        if self.value < 0:
            self.value += 0x10000000000000000
        return pack('17s',hex(self.value)[2:18])
    
    @staticmethod
    def s_getBytes(value):
        if value < 0:
            value += 0x10000000000000000
        return pack('17s',hex(value)[2:18])
    
    def setBytes(self,buffer):
        s=unpack('17s',str(buffer))[0]
        s=strip(s,'\0')
        
        self.value=int(s, 16)

    @staticmethod   
    def s_fromBytes(buffer):
        s=unpack('17s',str(buffer))[0]
        s=strip(s,'\0')
        
        return int(s, 16)

class int64_t:
    def __init__(self,value=0):
        self.value=value
        
    def getValue(self):
        return self.value
    
    def setValue(self,value):
        self.value=value
    
    def getSize(self):
        return 17;
    
    def getBytes(self):       
        if self.value < 0:
            self.value += 0x10000000000000000
        return pack('17s',hex(self.value)[2:18]+'\0')

    @staticmethod
    def s_getBytes(value):
        if value < 0:
            value += 0x10000000000000000
        return pack('17s',hex(value)[2:18]+'\0')
    
    def setBytes(self,buffer):
        s=unpack('17s',str(buffer))[0]
        s=strip(s,'\0')
        
        self.value=int(s, 16)
        
        if self.value > 0x8fffffffffffffff:
            self.value -= 0x10000000000000000
        

    @staticmethod   
    def s_fromBytes(buffer):
        s=unpack('17s',str(buffer))[0]
        s=strip(s,'\0')
        
        value=int(s, 16)
        
        if value > 0x8fffffffffffffff:
            value -= 0x10000000000000000
        
        return value

class stl_list(list):
    def __init__(self,element_type=None):
        self.element_type=element_type.strip()

        
    def setType(self,type):
        self.element_type=type
        return self.element_type   
    
    def serialize(self,byteStream):
        byteStream.pushUint32_t(len(self))
        for item in self:
            byteStream.pushObject(item, self.element_type)
    
#     def getClassLen(self):
#         len_bs = byte_stream.ByteStream()
#         len_bs.setRealWrite(False)
#         len_bs.m_iOffset += 4;
#         for item in self:
#             len_bs.pushObject(item, self.element_type)
#         class_len = len_bs.getWrittenLength()
#         return class_len;
    
    def unserialize(self,byteStream):
        size=byteStream.popUint32_t()
        del self[:]
        for i in range(0, size):
            self.append(byteStream.popObject(self.element_type))
        return self
    
    def __set__(self, instance, value):
        
        if not isinstance(value, list) and not isinstance(value, tuple):
            raise AttributeError("object attribute type " + str(value.__class__) + " not set")
        
        stllist = None
        if instance is not None:
            stllist = stl_list(self.element_type)
        else:
            stllist = self
        
        
        for item in value:        
        
            if is_stl_type(self.element_type):
                element_type = get_stl_type(self.element_type)
                element_object = key_type.__new__(element_type)
                element_type.__init__(element_object, self.element_type[self.element_type.find('<') + 1:-1])
                element_object.__set__(None, item)
                item = element_object
            else:
                if not __builtins__.has_key(item.__class__.__name__) and type(item).__module__ + '.' + item.__class__.__name__ != self.element_type:
                    raise AttributeError("object type " + str(item.__class__) + " not the same as " + self.element_type)
        
            stllist.append(item)
        
        
        if instance is not None:
            object.__setattr__(instance, "bbcplatform@" + str(id(self)), stllist)

    def __get__(self, instance, own):
        if instance is not None:
            if not hasattr(instance, "bbcplatform@" + str(id(self))):
                object.__setattr__(instance, "bbcplatform@" + str(id(self)), stl_list(self.element_type))
            return getattr(instance, "bbcplatform@" + str(id(self)))
        else:
            return self
    
class stl_vector(stl_list):
    def __init__(self,element_type=None):
        super(stl_vector,self).__init__(element_type)
        
class stl_set(set):
    def __init__(self,element_type=None):
        self.element_type=element_type.strip()
        
    def setType(self,type):
        self.element_type=type
        return self.element_type   
    
    def serialize(self,byteStream):
        byteStream.pushUint32_t(len(self))
        for item in self:
            byteStream.pushObject(item, self.element_type)
    
#     def getClassLen(self):
#         len_bs = byte_stream.ByteStream()
#         len_bs.setRealWrite(False)
#         len_bs.m_iOffset += 4;
#         for item in self:
#             len_bs.pushObject(item, self.element_type)
#         class_len = len_bs.getWrittenLength()
#         return class_len;
    
    def unserialize(self,byteStream):
        size=byteStream.popUint32_t()
        del self[:]
        for i in range(0, size):
            self.add(byteStream.popObject(self.element_type))
        return self

        
    def __set__(self, instance, value):
        
        if not isinstance(value, set) and not isinstance(value, list):
            raise AttributeError("object attribute type " + str(value.__class__) + " not set or list")
        
        stlset = None
        if instance is not None:
            stlset = stl_set(self.element_type)
        else:
            stlset = self
        
        
        for item in value:
        
            if is_stl_type(self.element_type):
                element_type = get_stl_type(self.element_type)
                element_object = key_type.__new__(element_type)
                element_type.__init__(element_object, self.element_type[self.element_type.find('<') + 1:-1])
                element_object.__set__(None, item)
                item = key_object
            else:
                if not __builtins__.has_key(item.__class__.__name__) and type(item).__module__ + '.' + item.__class__.__name__ != self.element_type:
                    raise AttributeError("object type " + str(item.__class__) + " not the same as " + self.element_type)
        
            stlset.add(item)

        
        if instance is not None:
            object.__setattr__(instance, "bbcplatform@" + str(id(self)), stlset)

    def __get__(self, instance, own):
        if instance is not None:
            if not hasattr(instance, "bbcplatform@" + str(id(self))):
                object.__setattr__(instance, "bbcplatform@" + str(id(self)), stl_set(self.element_type))
            return getattr(instance, "bbcplatform@" + str(id(self)))
        else:
            return self

class stl_bitset(list):
    def __init__(self,size):
        self.size=int(size)
        self.extend([0] * self.size)

    def serialize(self,byteStream):
        byteStream.pushUint32_t(self.size)
        for i in range(0, self.size):
            if i < len(self):
                byteStream.pushUint8_t(self[i])
            else:
                byteStream.pushUint8_t(0)
    
#     def getClassLen(self):
#         len_bs = byte_stream.ByteStream()
#         len_bs.setRealWrite(False)
#         len_bs.m_iOffset += 4
#         len_bs.m_iOffset += self.size
#         return len_bs.m_iOffset;        
            
    def unserialize(self,byteStream):
        self.size=byteStream.popUint32_t()
        del self[:]
        for i in range(0,self.size):
            v=byteStream.popUint8_t()
            self.append(v)  
        return self
    

    
    def __set__(self, instance, value):
        if not isinstance(value, list) and not isinstance(value, basestring): 
            raise AttributeError("object attribute type " + str(value.__class__) + " not list or string")
        
        bitset = None
        if instance is not None:
            bitset = stl_bitset(self.size)
        else:
            bitset = self
        
        for i in range(0, self.size):
            if i < len(value):
                bitset[i] = int(value[i])
            else:
                bitset[i] = 0
        
        if instance is not None:
            object.__setattr__(instance, "bbcplatform@" + str(id(self)), bitset)

    def __get__(self, instance, own):
        if instance is not None:
            if not hasattr(instance, "bbcplatform@" + str(id(self))):
                object.__setattr__(instance, "bbcplatform@" + str(id(self)), stl_bitset(self.size))
            return getattr(instance, "bbcplatform@" + str(id(self)))
        else:
            return self
    
    
class stl_map(dict):
    def __init__(self,element_type=None):
        self.element_type = element_type.strip()
        self.key_type = ''
        self.value_type = ''
        if element_type:
            cnt = 0
            i = 0
            for c in element_type:
                i += 1
                if c == ',' and cnt == 0:
                    break;
                elif c == '<':
                    cnt += 1
                elif c == '>':
                    cnt -= 1
                
            self.key_type = element_type[ : i - 1].strip()
            self.value_type = element_type[i : ].strip()
        
        if len(self.key_type) == 0 or len(self.value_type) == 0:
            raise AttributeError("stl_map must explicit point key and value type")
        
        
    def setType(self,key_type,value_type):
        self.key_type=key_type
        self.value_type=value_type
        self.element_type = key_type + ',' + value_type
        
    def serialize(self,byteStream):
        byteStream.pushUint32_t(len(self))
        for (k,v) in self.items():
            byteStream.pushObject(k,self.key_type)
            byteStream.pushObject(v,self.value_type)
            
#     def getClassLen(self):
#         len_bs = byte_stream.ByteStream()
#         len_bs.setRealWrite(False)
#         len_bs.m_iOffset += 4;
#         for (k,v) in self.items():
#             len_bs.pushObject(k,self.key_type)
#             len_bs.pushObject(v,self.value_type)
#             
#         class_len = len_bs.getWrittenLength()
#         return class_len;    
    
    
    def unserialize(self,byteStream):
        size=byteStream.popUint32_t()
        for i in range(0, size):
            k = byteStream.popObject(self.key_type)
            #print k    
            v = byteStream.popObject(self.value_type)
            
            self[k] = v
            
        return self
    
    def __set__(self, instance, value):
        if not isinstance(value, dict): 
            raise AttributeError("object attribute type " + str(value.__class__) + " not dict")
        
        stlmap = None
        if instance is not None:
            stlmap = stl_map(self.element_type)
        else:
            stlmap = self
        
        for k,v in value.items():
            

            if is_stl_type(self.key_type):
                key_type = get_stl_type(self.key_type)
                key_object = key_type.__new__(key_type)
                key_type.__init__(key_object, self.key_type[self.key_type.find('<') + 1:-1])
                key_object.__set__(None, k)
                k = key_object
                
            if is_stl_type(self.value_type):
                value_type = get_stl_type(self.value_type)
                value_object = value_type.__new__(value_type)
                value_type.__init__(value_object, self.value_type[self.value_type.find('<') + 1:-1])
                value_object.__set__(None, v)
                v = value_object


            if not is_stl_type(self.key_type) and not is_stl_type(self.value_type):
                if type(k) not in builtin_type and type(k).__module__ + '.' + k.__class__.__name__ != self.key_type:             
                    raise AttributeError("object type " + str(k.__class__) + " not the same as " + self.key_type)
            
                if type(v) not in builtin_type and type(v).__module__ + '.' + v.__class__.__name__ != self.value_type:               
                    raise AttributeError("object type " + str(v.__class__) + " not the same as " + self.value_type)

            stlmap[k] = v


        if instance is not None:
            object.__setattr__(instance, "bbcplatform@" + str(id(self)), stlmap)
            


    def __get__(self, instance, own):
        if instance is not None:
            if not hasattr(instance, "bbcplatform@" + str(id(self))):
                object.__setattr__(instance, "bbcplatform@" + str(id(self)), stl_map(self.element_type))
            return getattr(instance, "bbcplatform@" + str(id(self)))
        else:
            return self
    
def bin2hex(binn):
    s=binn.encode('hex')
    return s

if __name__=='__main__':
    pass
#     from binhex import binhex
#     var1=uint8_t(255)   
#     bytes255=var1.getBytes()
#      
#     print bin2hex(bytes255)
#     var2=uint8_t()
#     var2.setBytes(bytes255)
#     print var2.value
#     
#     var3=int8_t(127)   
#     bytes127=var3.getBytes()
#      
#     print bin2hex(bytes127)
#     var4=int8_t()
#     var4.setBytes(bytes127)
#     print var4.value
#     
#     var3=uint16_t(256*256-1)   
#     bytes=var3.getBytes()
#      
#     print bin2hex(bytes)
#     var4=uint16_t()
#     var4.setBytes(bytes)
#     print var4.value
#     
#     
#     var3=int16_t(256*128-1)   
#     bytes=var3.getBytes()
#      
#     print bin2hex(bytes)
#     var4=int16_t()
#     var4.setBytes(bytes)
#     print var4.value
#     
#     var3=uint32_t(256*256*256*256-1)   
#     bytes=var3.getBytes()
#      
#     print bin2hex(bytes)
#     var4=uint32_t()
#     var4.setBytes(bytes)
#     print var4.value
#     
#     
#     var3=int32_t(256*256*256*128-1)   
#     bytes=var3.getBytes()
#      
#     print bin2hex(bytes)
#     var4=int32_t()
#     var4.setBytes(bytes)
#     print var4.value
#     
#     
#     var3=uint64_t(256*256*256*128-1)   
#     bytes=var3.getBytes()
#      
#     print "uint64_t:",bin2hex(bytes)
#     var4=uint64_t()
#     var4.setBytes(bytes)
#     print var4.value
#     
#     vector=stl_vector()
#