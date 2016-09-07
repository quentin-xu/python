import sys
import lang_util
from struct import pack,unpack

def load_object(path):
	try:
		dot = path.rindex('.')
	except ValueError:
		raise ValueError, "Error loading object '%s': not a full path" % path

	module, name = path[:dot], path[dot+1:]
	try:
		mod = __import__(module, {}, {}, [''])
	except ImportError, e:
		raise ImportError, "Error loading object '%s': %s" % (path, e)

	try:
		obj = getattr(mod, name)
	except AttributeError:
		raise NameError, "Module '%s' doesn't define any object named '%s'" % (module, name)

	return obj


class ByteStream:
	def __init__(self): 
		self.m_byPackage = bytearray()
		self.m_iBufLen = 0
		self.m_iOffset = 0
		self.m_bGood = True
		self.m_bRealWrite = True

	def setBuffer4Unserialze(self,buffer): 
		self.m_byPackage = bytearray(buffer)
		self.m_iBufLen = len(self.m_byPackage)
		self.m_iOffset = 0
		self.m_bGood = True
		self.m_bRealWrite = True


	def setRealWrite(self,bRealWrite): 
		self.m_bRealWrite = bRealWrite

	def getWrittenBuffer(self): 
		return self.m_byPackage

	def getWrittenLength(self): 
		return self.m_iOffset

	def getReadLength(self): 
		return self.m_iOffset

	def isGood(self): 
		return self.m_bGood

	
	
	def pushBool(self,b):
		v = 0
		if(b):
			v = 1
		self.pushByte(v)
		
	
	def popBool(self): 
		v = self.popByte()
		if(v != 0):
			return True
		return False


	def pushUint8_t(self,v):
# 		if not self.m_bRealWrite:
# 			self.m_iOffset+=1
# 			return
		if isinstance(v, lang_util.uint8_t):
			self.m_byPackage += v.getBytes(v)
		else:
			self.m_byPackage += lang_util.uint8_t.s_getBytes(v)
		self.m_iOffset+=1

	def popUint8_t(self): 
		if  (not self.m_bGood) or (self.m_iOffset + 1) > self.m_iBufLen: 
			self.m_bGood = False
			return
	
		#bytes = self.m_byPackage[self.m_iOffset:self.m_iOffset + 1]
		#bytes = self.m_byPackage[self.m_iOffset:self.m_iOffset + 1]
		v = self.m_byPackage[self.m_iOffset]
		self.m_iOffset+=1
		return v

	def pushUByte(self,v): 
		return self.pushUint8_t(v)

	def popUByte(self): 
		return self.popUint8_t()


	def pushInt8_t(self,v): 
# 		if not self.m_bRealWrite: 
# 			self.m_iOffset+=1
# 			return
		if isinstance(v, lang_util.int8_t):
			self.m_byPackage += v.getBytes(v)
		else:
			self.m_byPackage += lang_util.int8_t.s_getBytes(v)
		self.m_iOffset+=1

	def popInt8_t(self): 
		if (not self.m_bGood) or (self.m_iOffset + 1) > self.m_iBufLen: 
			self.m_bGood = False
			return
		bytes=self.m_byPackage[self.m_iOffset:self.m_iOffset + 1]
		self.m_iOffset+=1
		return lang_util.int8_t.s_fromBytes(bytes)

	def pushByte(self,v):
# 		if not self.m_bRealWrite:
# 			self.m_iOffset+=1
# 			return
		if not isinstance(v, int):
			b=pack('1c',v)
		else:
			b=pack('b',v)
		self.m_byPackage+=b
		self.m_iOffset+=1
		
	def popByte(self):
		if (not self.m_bGood) or (self.m_iOffset+1)>self.m_iBufLen:
			self.m_bGood=False
			return
		#b=self.m_byPackage[self.m_iOffset:self.m_iOffset+1]
		v=self.m_byPackage[self.m_iOffset]
		self.m_iOffset+=1
		#print "popByte:"+b.encode('hex')
		#print unpack('B',b)
		#print "popByte v:"+str(v)
		return v

	def pushUint16_t(self,v): 
# 		if not self.m_bRealWrite: 
# 			self.m_iOffset+=2
# 			return
		if isinstance(v, lang_util.uint16_t):
			self.m_byPackage += v.getBytes(v)
		else:
			self.m_byPackage += lang_util.uint16_t.s_getBytes(v)
		self.m_iOffset+=2

	def popUint16_t(self): 
		if (not self.m_bGood) or (self.m_iOffset + 2) > self.m_iBufLen: 
			self.m_bGood = False
			return
		bytes=self.m_byPackage[self.m_iOffset:self.m_iOffset + 2]
		self.m_iOffset+=2
		return lang_util.uint16_t.s_fromBytes(bytes)

	def pushInt16_t(self,v): 
# 		if not self.m_bRealWrite: 
# 			self.m_iOffset+=2
# 			return
		if isinstance(v, lang_util.int16_t):
			self.m_byPackage += v.getBytes(v)
		else:
			self.m_byPackage += lang_util.int16_t.s_getBytes(v)
		self.m_iOffset+=2

	def popInt16_t(self): 
		if (not self.m_bGood) or (self.m_iOffset + 2) > self.m_iBufLen: 
			self.m_bGood = False
			return
		bytes=self.m_byPackage[self.m_iOffset:self.m_iOffset + 2]
		self.m_iOffset+=2
		return lang_util.int16_t.s_fromBytes(bytes)


	def pushShort(self,v):
		return self.pushInt16_t(v)
	
	def popShort(self):
		return self.popInt16_t()
	
	def pushUint32_t(self,v): 
# 		if not self.m_bRealWrite: 
# 			self.m_iOffset+=4
# 			return
		if isinstance(v, lang_util.uint32_t):
			self.m_byPackage += v.getBytes(v)
		else:
			self.m_byPackage += lang_util.uint32_t.s_getBytes(v)
		self.m_iOffset+=4

	def popUint32_t(self): 
		if (not self.m_bGood or (self.m_iOffset + 4) > self.m_iBufLen): 
			self.m_bGood = False
			return
		#bytes=self.m_byPackage[self.m_iOffset:self.m_iOffset + 4]
		v = (self.m_byPackage[self.m_iOffset] << 24) + (self.m_byPackage[self.m_iOffset + 1] << 16) +  (self.m_byPackage[self.m_iOffset + 2] << 8) + self.m_byPackage[self.m_iOffset + 3]
		self.m_iOffset+=4
		return v

	
	def pushInt32_t(self,v): 
# 		if not self.m_bRealWrite: 
# 			self.m_iOffset+=4
# 			return
		if isinstance(v, lang_util.int32_t):
			self.m_byPackage += v.getBytes(v)
		else:
			self.m_byPackage += lang_util.int32_t.s_getBytes(v)
	
		self.m_iOffset+=4

	def popInt32_t(self): 
		if (not self.m_bGood or (self.m_iOffset + 4) > self.m_iBufLen): 
			self.m_bGood = False
			return
		bytes=self.m_byPackage[self.m_iOffset:self.m_iOffset + 4]
		self.m_iOffset+=4
		return lang_util.int32_t.s_fromBytes(bytes)
		

	def pushInt(self,v):
		return self.pushInt32_t(v)
	
	def popInt(self):
		return self.popInt32_t()


	def pushUint64_t(self,v):
# 		if not self.m_bRealWrite:
# 			self.m_iOffset+=17
# 			return
		if isinstance(v, lang_util.uint64_t):
			self.m_byPackage += v.getBytes(v)
		else:
			self.m_byPackage += lang_util.uint64_t.s_getBytes(v)
			
		self.m_iOffset+=17
		
	def popUint64_t(self): 
		if (not self.m_bGood or (self.m_iOffset + 17) > self.m_iBufLen): 
			self.m_bGood = False
			return
		bytes=self.m_byPackage[self.m_iOffset:self.m_iOffset + 17]
		self.m_iOffset+=17
		return lang_util.uint64_t.s_fromBytes(bytes)
	
	def pushInt64_t(self,v):
# 		if not self.m_bRealWrite:
# 			self.m_iOffset+=17
# 			return
		if isinstance(v, lang_util.int64_t):
			self.m_byPackage += v.getBytes(v)
		else:
			self.m_byPackage += lang_util.int64_t.s_getBytes(v)
			
		self.m_iOffset+=17
		
	def popInt64_t(self): 
		if (not self.m_bGood or (self.m_iOffset + 17) > self.m_iBufLen): 
			self.m_bGood = False
			return
		bytes=self.m_byPackage[self.m_iOffset:self.m_iOffset + 17]
		self.m_iOffset+=17
		return lang_util.int64_t.s_fromBytes(bytes)

	def pushLong(self,v):
		return self.pushInt64_t(v)
	
	def popLong(self):
		return self.popInt64_t()


	def pushString(self,v):
# 		if not self.m_bRealWrite:
# 			self.m_iOffset+=4
# 			if v:
# 				self.m_iOffset+=len(v)
# 			return
		strlen=len(v)
		self.pushUint32_t(strlen)
		str=pack("%ds"%strlen,v)
		#print "pushString:"+str.encode('hex')
		self.m_byPackage+=str
		self.m_iOffset+=strlen
		
	def popString(self):
		
		strlen=self.popUint32_t()
		#print "self.m_iBufLen:%d"%self.m_iBufLen
		#print "self.m_iOffset:%d"%self.m_iOffset
		#print "strlen:%d"%strlen
		if not self.m_bGood or (self.m_iOffset+strlen)>self.m_iBufLen:
			self.m_bGood=False
			return
		v=self.m_byPackage[self.m_iOffset:self.m_iOffset+strlen]
		v=str(v)
		self.m_iOffset+=strlen
		return v

	def pushBytes(self,v,blen):
# 		if not self.m_bRealWrite:
# 			self.m_iOffset+=blen
# 			return
		for i in range(0,blen):
			self.pushByte(v[i])
			

	def popBytes(self,blen):
		if not self.m_bGood or (self.m_iOffset+blen)>self.m_iBufLen:
			self.m_bGood=False
			return
		result=''
		for i in range(0,blen):
			result+=chr(self.popByte())		
		return result

	def pushObject(self,o,type="uint32_t"):
		base_types = ['uint8_t','uint16_t','uint32_t','uint64_t','int8_t','int16_t','int32_t','int64_t']
		#stl_types = ['stl_list','stl_vector','stl_set','stl_bitset','stl_map']
		if type in base_types:
			obj=lang_util.__dict__[type]()
			obj.setValue(o)
			blen=obj.getSize()
# 			if not self.m_bRealWrite:
# 				self.m_iOffset+=blen
# 				return True
			b=obj.getBytes()
			self.m_byPackage+=b
			self.m_iOffset+=blen
			return True
		elif hasattr(o,'serialize'):
# 			if not self.m_bRealWrite:
# 				self.m_iOffset += o.getClassLen()
# 				return True
# 			else:
			return o.serialize(self)
			
# 			bs = ByteStream()
# 			o.serialize(bs)
# 			self.m_byPackage+=bs.m_byPackage
# 			self.m_iOffset+=bs.m_iOffset
# 			return True

		elif isinstance(o, int) and type == 'int':
			return self.pushUint32_t(o)
		elif isinstance(o,str) and type == 'stl_string':
			return  self.pushString(o)
		else:
			raise AttributeError("Can't pushObject attribute type " + str(o.__class__) + " as " + type)
		
		
	def popObject(self,type="uint32_t"):
		type=type.strip()
		if type[0:4]=='uint':
			obj=lang_util.__dict__[type]()
			blen=obj.getSize()
			bytes=self.m_byPackage[self.m_iOffset:self.m_iOffset+blen]
			obj.setBytes(bytes)
			object_value=obj.getValue()
			self.m_iOffset+=blen
			return object_value
		elif type[0:7]=='stl_map':	
			map=lang_util.stl_map(type[8:-1])
# 			map.setType(key_type,value_type)
			object_value=map.unserialize(self)
			return object_value
		elif type[0:10]=='stl_bitset':
			vec=lang_util.stl_bitset(type[11:-1])
			object_value=vec.unserialize(self)
			return object_value
		#stl_vector, stl_list, stl_set, stl_bitset
		elif type[0:4]=='stl_' and type!='stl_string':
			lt_pos=-1
			if '<' in type:
				lt_pos=type.index('<')
			if lt_pos==-1:
				return False
			value_type=type[lt_pos+1:-1]
			vec=lang_util.stl_list(value_type)
			object_value=vec.unserialize(self)
			return object_value
		elif type.find('.')>0 and type.rindex('.')!=len(type)-1:
			o=load_object(type)
			if hasattr(o,'unserialize'):
				o=o()
				object_value=o.unserialize(self)
				return object_value
			return False
		elif type=='int':
			return self.popInt()
		elif type=='string' or type=='stl_string':
			object_value=self.popString()
			return object_value			
		else:
			raise AttributeError("Can't popObject attribute type " + type)
			return False
			
			
			
		


