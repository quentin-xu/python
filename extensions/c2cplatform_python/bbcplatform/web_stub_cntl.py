'''
Created on 2012-5-13

@author: yilin
'''

import socket
class TTransportBase:
    def isOpen(self):
        pass
    def open(self):
        pass
    def close(self):
        pass
    def read(self,sz):
        pass
    def readAll(self,sz):
        buff=''
        have=0
        while(have<sz):
            chunk=self.read(sz-have)
            have+=len(chunk)
            buff+=chunk
            if len(chunk)==0:
                raise EOFError()
        return buff
    
    def write(self,buf):
        pass
    def flush(self):
        pass
 
class TSocketBase(TTransportBase):
    def _resovleAddr(self):
        return socket.getaddrinfo(self.host, self.port,socket.AF_UNSPEC,
                socket.SOCK_STREAM,0,socket.AI_PASSIVE|socket.AI_ADDRCONFIG
                )
             
    def close(self):
        if self.handle:
            self.handle.close()
            self.handle=None
            
class TSocket(TSocketBase):
    def __init__(self,host='localhost',port=53101):
        self.host=host
        self.port=port
        self.handle=None
        self._timeout=None
        
    def setHandle(self,h):
        self.handle=h
        
    def isOpen(self):
        return self.handle is not None
    
    def setTimeout(self,ms):
        if ms is None:
            self._timeout=None
        else:
            self._timeout=ms/1000.0
            
        if self.handle is not None:
            self.handle.settimeout(self._timeout)
            
    def open(self):
        try:
            res0=self._resovleAddr()
            for res in res0:
                self.handle=socket.socket(res[0],res[1])
                self.handle.settimeout(self._timeout)
                try:
                    self.handle.connect(res[4])
                except socket.error,e:
                    if res is not res0[-1]:
                        continue
                    else:
                        raise e
                    break
        except socket.error,e:
            raise e
    def read(self,sz):
        try:
            buff=self.handle.recv(sz)
        except socket.error:
            self.close()
            buff=''
        return buff
    
    def write(self,buff):
        sent=0
        have=len(buff)
        while sent<have:
            plus=self.handle.send(buff)
            if plus==0:
                raise
            sent+=plus
            buff=buff[plus:]
    def flush(self):
        pass


import sys
import random
from c2c_pkg_head import *
from byte_stream import ByteStream

try:
    import confg4set
except Exception,e:
    print e

class WebStubCntl():
    def __init__(self):
        self.ph=BSPkgHead()
        self.callerName='python'
        self.peer_ip=''
        self.peer_ip_port_set=False
        self.peer_port=53101
        self.__STX__=0x55AA
        self.STX=0x02
        self.ETX=0x03
        self.last_err_msg=''
        self.routeKey = random.randint(0,65522) 
        self.cmdSet = 0    

    def setCmdSet(self, dwCmdSet):
        self.cmdSet = dwCmdSet

    def setRouteKey(self, dwRouteKey):
        self.routeKey = dwRouteKey
        
    def setPeerIPPort(self,sIP,iPort):
        self.peer_ip=sIP
        self.peer_port=iPort
        self.peer_ip_port_set=True
        
    def getDwOperatorId(self):
        return self.ph.getDwOperatorId()
    
    def setDwOperatorId(self,dwOperatorId):
        self.ph.setDwOperatorId(dwOperatorId)
        
    def getDwSerialNo(self):
        return self.ph.getDwSerialNo()
    
    def setDwSerialNo(self,dwSerialNo):
        self.ph.setDwSerialNo(dwSerialNo)
        
    def getDwUin(self):
        return self.ph.getDwUin()
    
    def setDwUin(self,dwUin):
        self.ph.setDwUin(dwUin)
        
    def getSPassport(self):
        return self.ph.getSPassport()
    
    def setSPassport(self,passport):
        self.ph.setSPassport(passport)
        
    def getDwCommand(self):
        return self.ph.getDwCommand()
    
    def setDwCommand(self,dwCommand):
        self.ph.setDwCommand(dwCommand)
        
    def getWVersion(self):  
        return  self.ph.getWVersion()
    
    def setWVersion(self,version):
        self.ph.setWVersion(version)  
        
    def getCallerName(self):
        return self.callerName
    
    def setCallerName(self,name):
        self.callerName=name
        
    def getLastErrMsg(self):
        return self.last_err_msg
    
    def invoke(self,req,resp,timeout=2*1000):
        ip = self.peer_ip;
        port = self.peer_port
        if ip == '' and sys.modules.has_key('bbcplatform.confg4set'):
            ip, port = confg4set.getAppAddressByModAndL5(req.getCmdId(), self.cmdSet, self.routeKey)

        client=TSocket(ip, port)
        client.setTimeout(timeout)
        
        #print "client connected %s:%s"% (self.peer_ip, self.peer_port)
        #print req.getCmdId()
        bs=self.serializeToByteStream(req)
        buffer=bs.getWrittenBuffer()
        #return
        client.open()
        client.write(buffer)
        
        recv_buffer=client.readAll(2)
        #print recv_buffer.encode('hex')
        
        if not recv_buffer:
            self.last_err_msg='__STX__ cannot read'
            #print self.last_err_msg
            return -7
        
        bs2=ByteStream()
        bs2.setBuffer4Unserialze(recv_buffer)        
        __STX__2=bs2.popUint16_t()
        recv_buffer=client.readAll(4)
        bs2.setBuffer4Unserialze(recv_buffer)
        dwRecvPkgLen=bs2.popUint32_t()
        #print "dwRecvPkgLen:%d"%dwRecvPkgLen
        if dwRecvPkgLen<0:
            self.last_err_msg="dwRecvPkgLen < 0"
            #print self.last_err_msg
            return -9
        
        recv_buffer=client.readAll(dwRecvPkgLen)
        bs2.setBuffer4Unserialze(recv_buffer)
        if not recv_buffer:
            self.last_err_msg="cannot read reponse body"
            #print self.last_err_msg
            return -10
        bSTX2=bs2.popByte()
        if bSTX2!=self.STX:
            self.last_err_msg="STX not match"
            #print self.last_err_msg
            return -10
        self.ph.unserialize(bs2)
        #print vars(self.ph)
        #print self.ph.getDwCommand()
        #print "ph.unerialize ok"
        resp.unserialize(bs2)
        
        #print "resp.unerialize ok"
        if not bs2.isGood():
            self.last_err_msg="resp.unserialize is not good."
            #print self.last_err_msg
            return -2
        
        bETX=bs2.popByte()
        #print bETX,self.ETX
        if bETX!=self.ETX:
            self.last_err_msg="ETX not match."
            #print self.last_err_msg
            return -2
        client.close()
        return 0    
            
        
        
    def serializeToByteStream(self,obj):

        bsdummy=ByteStream()
        bsdummy.setRealWrite(False)
        obj.serialize(bsdummy)
        dwBodyLen=bsdummy.getWrittenLength()
        dwPkgLength = 1 + self.ph.iPkgHeadLength + dwBodyLen + 1
        self.ph.setDwCommand(obj.getCmdId())
        self.ph.setDwLength(dwPkgLength)
        
        bs=ByteStream()
        bs.pushUint16_t(self.__STX__)
        bs.pushUint32_t(dwPkgLength)
        bs.pushByte(self.STX)
       
        self.ph.serialize(bs)
        obj.serialize(bs)
        bs.pushByte(self.ETX)
        return bs
    
    def unserializeFromByteStream(self,buffer,obj):
        bs=ByteStream()
        result={'retCode':0,'msg':''}
        bs.setBuffer4Unserialze(buffer)
        __STX__=bs.popUint16_t()
        
        if __STX__!=self.__STX__:
            result['retCode']=-5
            result['msg']="__STX__ not match."
            #print result['msg']
            return result
        
        dwRecvPkgLen = bs.popUint32_t()
        if dwRecvPkgLen<0:
            result['retCode']=-6
            result['msg']="dwRecvPkgLen < 0"
            #print result['msg']
            return result
        
        bSTX=bs.popByte()
        if bSTX!=self.STX:
            result['retCode']=-1
            result['msg']="STX not match."
            #print result['msg']
            return result
        self.ph.unserialize(bs)
        
        if obj:
            #print "has obj %s"%obj.__class__
            obj.unserialize(bs)
        else:
            #print "not has obj"
            obj=self.getConfObject(self.ph)
            if obj:
                obj.req.unserialize(bs)
                result['req']=obj['req']
                result['handler']=obj['handler'] 
            else:
                result['retCode']=-1
                result['msg']= "Service configuration error."
                
        if not bs.isGood():
            result['retCode']=-1
            result['msg']="obj.unserialize is not good."
            #print result['msg']
            return result
        bETX=bs.popByte()
        if bETX!=self.ETX:
            result['retCode']=-3
            result['msg']="ETX not match."
            #print result['msg']
            self.last_err_msg=result['msg']
            return result
            
        return result
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
