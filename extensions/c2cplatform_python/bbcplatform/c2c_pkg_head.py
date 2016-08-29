'''
Created on 2012-5-11

@author: yilin
'''
class BSPkgHead:
    def __init__(self):    
        self.dwLength = 0
        self.dwSerialNo = 0
        self.wVersion = 2
        self.wCommand = 0
        self.dwUin = 0
        self.dwFlag = 0
        self.dwResult = 0
        self.dwClientIP = 0
        self.wClientPort = 0
        self.dwAccessServerIP = 0
        self.wAccessServerPort = 0
        self.dwAppInterfaceIP = 0
        self.wAppInterfacePort = 0
        self.dwAppServerIP = 0
        self.wAppServerPort = 0
    
        self.dwOperatorId = 0
        self.sPassport = "1234567890"
    
        self.wSeconds = 0
        self.dwUSeconds = 0
    
        self.wCookie2Length = 35
        self.cLegacy = 0
        self.iSockfd = 0
        self.dwSockChannel = 0
        self.wPeerPort = 0
        self.dwCommand = 0
        self.wSvrLevelIndex = 0
        self.bCookie2 = "123456789012345678"
    
        self.PASSPORT_LEN = 10
        self.COOKIE2_LEN = 18
    
        self.iPkgHeadLength = 105


    def getDwSerialNo(self):
        return self.dwSerialNo

    def setDwSerialNo(self,dwSerialNo):
        self.dwSerialNo = dwSerialNo
        
    def getDwResult(self):
        return self.dwResult   
    
    def setDwResult(self,dwResult):
        self.dwResult = dwResult


    def serialize(self,bs):
       

        #print "head serialize start:"+bs.m_byPackage.encode('hex') 
        bs.pushUint32_t(self.dwLength)                
        bs.pushUint32_t(self.dwSerialNo)        
        bs.pushUint16_t(self.wVersion)
        bs.pushUint16_t(self.wCommand)
        bs.pushUint32_t(self.dwUin)
        bs.pushUint32_t(self.dwFlag)
        bs.pushUint32_t(self.dwResult)
        bs.pushUint32_t(self.dwClientIP)
        bs.pushUint16_t(self.wClientPort)
        bs.pushUint32_t(self.dwAccessServerIP)
        bs.pushUint16_t(self.wAccessServerPort)
        bs.pushUint32_t(self.dwAppInterfaceIP)
        bs.pushUint16_t(self.wAppInterfacePort)
        bs.pushUint32_t(self.dwAppServerIP)
        bs.pushUint16_t(self.wAppServerPort)
        bs.pushUint32_t(self.dwOperatorId)
        bs.pushBytes(self.sPassport, self.PASSPORT_LEN)
        bs.pushUint16_t(self.wSeconds)
        bs.pushUint32_t(self.dwUSeconds)
        bs.pushUint16_t(self.wCookie2Length)
        bs.pushByte(self.cLegacy)
        bs.pushUint32_t(self.iSockfd)
        bs.pushUint32_t(self.dwSockChannel)
        bs.pushUint16_t(self.wPeerPort)
        bs.pushUint32_t(self.dwCommand)
        bs.pushUint16_t(self.wSvrLevelIndex)
        bs.pushBytes(self.bCookie2, self.COOKIE2_LEN)
        
        return bs.isGood()


    def unserialize(self,bs):
        self.dwLength = bs.popUint32_t()
        self.dwSerialNo = bs.popUint32_t()
        self.wVersion = bs.popUint16_t()
        self.wCommand = bs.popUint16_t()
        self.dwUin = bs.popUint32_t()
        self.dwFlag = bs.popUint32_t()
        self.dwResult = bs.popUint32_t()
        self.dwClientIP = bs.popUint32_t()
        self.wClientPort = bs.popUint16_t()
        self.dwAccessServerIP = bs.popUint32_t()
        self.wAccessServerPort = bs.popUint16_t()
        self.dwAppInterfaceIP = bs.popUint32_t()
        self.wAppInterfacePort = bs.popUint16_t()
        self.dwAppServerIP = bs.popUint32_t()
        self.wAppServerPort = bs.popUint16_t()
        self.dwOperatorId = bs.popUint32_t()
        self.sPassport = bs.popBytes(self.PASSPORT_LEN)
        self.wSeconds = bs.popUint16_t()
        self.dwUSeconds = bs.popUint32_t()
        self.wCookie2Length = bs.popUint16_t()
        self.cLegacy = bs.popByte()
        self.iSockfd = bs.popUint32_t()
        self.dwSockChannel = bs.popUint32_t()
        self.wPeerPort = bs.popUint16_t()
        self.dwCommand = bs.popUint32_t()
        self.wSvrLevelIndex = bs.popUint16_t()
        self.bCookie2 = bs.popBytes(self.COOKIE2_LEN)
    
        return bs.isGood()


    def getDwClientIP(self): 
        return self.dwClientIP

    def setDwClientIP(self,dwClientIP):
        self.dwClientIP = dwClientIP

    def getDwCommand(self):
        return self.dwCommand

    def setDwCommand(self,dwCommand):
        self.dwCommand = dwCommand
        self.wCommand = dwCommand / 0x10000


    def getDwOperatorId(self): 
        return self.dwOperatorId
    
    
    def setDwOperatorId(self,dwOperatorId):
        self.dwOperatorId = dwOperatorId

    def getSPassport(self):
        return self.sPassport

    def setSPassport(self,passport):
        self.sPassport = passport

    def getWClientPort(self):
        return self.wClientPort

    def setWClientPort(self,clientPort):
        self.wClientPort = clientPort

    def getDwLength(self):
        return self.dwLength

    def setDwLength(self,dwLength):
        self.dwLength=dwLength

    def dump(self):
        pass

    def getDwUin(self):
        return self.dwUin

    def setDwUin(self,dwUin):
        self.dwUin=dwUin

    def getWVersion(self):
        return self.wVersion

    def setWVersion(self,version):
        self.wVersion=version



