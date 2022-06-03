#include <stdio.h>

#include <memory.h>

#include <winsock.h>

#pragma pack(push)
#pragma pack(1)

struct CoreService_Init{
    char            ubytStartCode;
    long long       IngSeqNo;
    unsigned int    unitFCode;
    char            ubytMarketNo;
    char            arychrReserve[6];
    unsigned int    uintLen;
    char            arychrIdentifier[10];
    char            ubytCheckSum;
};

#pragma pack(pop)

int PrintBinaryString( const char* pBinary, int dataLength, char* pString, int stringLength, bool isSplitBySpace ){
    static char mappingCode[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    int dstIndex = 0;
    int iMap = 0;
    for( int i = 0; i < dataLength; ++i ){
        iMap = (pBinary[i] & 0xF0) >> 4;
        pString[dstIndex++] = mappingCode[iMap];
        iMap = pBinary[i] & 0x0F;
        pString[dstIndex++] = mappingCode[iMap];
        if( isSplitBySpace ){
            pString[dstIndex++] = 0x20;
        }
    }
    return dstIndex;
}

char CalcCheckSum( const char* pData, int len ){
    if( len <= 0 )
        return 0;

    char checkSum = pData[0];
    for( int i = 1; i < len; ++i ){
        checkSum ^= pData[i];
    }

    return checkSum;
}

int main(){

    CoreService_Init init;
    memset( &init, 0, sizeof(init) );

    init.ubytStartCode = 0x1B;
    init.unitFCode = 0x03000000;
    init.IngSeqNo = 1;
    memcpy( init.arychrIdentifier, "KGICORE", 7 );
    

    //init.unitFCode = htonl( init.unitFCode );

    // calculate check sum
    int length = sizeof( init ) - 1;
    char* pInit = (char *)&init;    

    init.ubytCheckSum = CalcCheckSum(pInit, length);    

    char buffer[256];
    int len = PrintBinaryString( reinterpret_cast<const char *>(&init),  length + 1, buffer, 256, true );
    buffer[len] = '\0';
    printf( "Binary:%s\r\nCheckSum=%d\r\n", buffer, init.ubytCheckSum );

    memcpy( init.arychrIdentifier, "COREKGI", 7 );
    len = PrintBinaryString( reinterpret_cast<const char *>(&init),  length + 1, buffer, 256, true );
    buffer[len] = '\0';
    init.ubytCheckSum = CalcCheckSum(pInit, length);
    printf( "Binary:%s\r\nCheckSum=%d\r\n", buffer, init.ubytCheckSum );

    init.unitFCode = htonl( init.unitFCode );
    len = PrintBinaryString( reinterpret_cast<const char *>(&init),  length + 1, buffer, 256, true );
    buffer[len] = '\0';
    init.ubytCheckSum = CalcCheckSum(pInit, length);
    printf( "Binary:%s\r\nCheckSum=%d\r\n", buffer, init.ubytCheckSum );

    return 0;
}