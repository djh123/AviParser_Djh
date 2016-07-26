#include <assert.h>
#include <errno.h>

#include "avifileparser.h"
#include "byte_write.h"



AVIFileParser::AVIFileParser(void)
    :_aviFd(NULL),
      _hasAudio(false),
      _hasVideo(false),
      _isValid(false),
      _fLen(0),
      _moviOff(0),
      _maxFrameSize(0),
      _currVideoIndex(0)
{
}

AVIFileParser::AVIFileParser( const char* file )
    :_hasAudio(false),
      _hasVideo(false),
      _isValid(false),
      _fLen(0),
      _moviOff(0),
      _maxFrameSize(0),
      _currVideoIndex(0)
{
    openAVI(file);
}


AVIFileParser::~AVIFileParser(void)
{
    if (_aviFd != NULL)
        fclose(_aviFd);
}


void AVIFileParser::openAVI( const char* file )
{
    _aviFd = fopen(file, "rb");
    if (_aviFd == NULL)
    {
#ifndef NPRINT
        printf("open avi error:\n");
#endif
        // 抛出错误号
        throw ERROR_OPEN_AVI;
    }

    // 开始解析
    _parseFile();

    _isValid = true;
}

void AVIFileParser::_parseFile()
{
    // 第一次读12个字节
    DWORD fourcc = 0;
    DWORD fourcc2 = 0;
    //    bool flag = true;
    int index =1;
    bool hasIndex = false;

    readFileTagSize(fourcc, _fLen, fourcc2);
    if (fourcc != FOURCC_RIFF || fourcc2 != FOURCC_AVI)
        throw ERROR_INVALID_AVI;

    while (true)
    {
        uint32_t size = 0;
        //        bool isEof = readFileDW(fourcc);
        //        if (isEof)
        //            return;
//        char tmpBuf[4] = {0};
//        bool ret = readFile(tmpBuf, 4);
//        printf("hello world aaaa %s \n",tmpBuf);
//        fourcc = read_le_dw(tmpBuf);
//        printf("hello world aaaa %u \n",fourcc);
//        readFileDW(size);
        //        if (isEof)
        //            return;
//        if(index = 2) index = 3;
        bool isEof = readFileDW(fourcc);
        if (isEof)
            return;
        isEof = readFileDW(size);
        if (isEof)
            return;


        if (fourcc == FOURCC_LIST)
        {
            printf("hello world aaaa %x \n",FOURCC_LIST);
            char tmpBuf[4] = {0};
            readFile(tmpBuf, 4);
            printf("hello world exec %s \n",tmpBuf);
            fourcc2 = read_le_dw(tmpBuf);

            //            fourcc2 = readFileDW();
            if(fourcc2 == FOURCC_hdrl){
                {
                    if (size > MAX_ALLOWED_AVI_HEADER_SIZE)
                        throw ERROR_INVALID_AVI;

                    // 跳过hdrl
                    uint32_t off = 4;
                    printf("hello world size %d \n",size);
                    while (off < size)
                    {
                        printf("hello world off %d \n",off);
                        fourcc = readFileDW();
                        switch (fourcc)
                        {
                        case FOURCC_avih:
                        {
                            _aviMainHeader.fcc = FOURCC_avih;
                            _aviMainHeader.cb = readFileDW();
                            _aviMainHeader.dwMicroSecPerFrame = readFileDW();
                            _aviMainHeader.dwMaxBytesPerSec = readFileDW();
                            _aviMainHeader.dwPaddingGranularity = readFileDW();
                            _aviMainHeader.dwFlags = readFileDW();
                            _aviMainHeader.dwTotalFrames = readFileDW();
                            _aviMainHeader.dwInitialFrames = readFileDW();
                            _aviMainHeader.dwStreams = readFileDW();
                            _aviMainHeader.dwSuggestedBufferSize = readFileDW();
                            _aviMainHeader.dwWidth = readFileDW();
                            _aviMainHeader.dwHeight = readFileDW();

                            if ((AVIFILEINFO_HASINDEX & _aviMainHeader.dwFlags) == AVIFILEINFO_HASINDEX)
                                hasIndex = true;

                            // 跳过保留字段
                            fseek(_aviFd, 16, SEEK_CUR);
                            // 跳过avih以及长度各四个字节
                            off += _aviMainHeader.cb + 8;
                        }
                            break;
                        case FOURCC_LIST:
                        {
                            int avListLen = readFileDW();
                            if (readFileDW() != FOURCC_strl)
                                throw ERROR_INVALID_AVI;

                            // 跳过strl
                            int tmpOff = 4;
                            AVIStreamHeader aviStreamHeader = {0};
                            printf("hello world tmpOff %d \n",tmpOff);
                            printf("hello world avListLen %d \n",avListLen);
                            while (tmpOff < avListLen)
                            {
                                //                                fourcc = readFileDW();
                                char tmpBufq[4] = {0};
                                readFile(tmpBufq, 4);
                                fourcc = read_le_dw(tmpBufq);
                                printf("hello world bbb  %s \n",tmpBufq);
                                tmpOff += 4;

                                if (fourcc == FOURCC_strh)
                                {
                                    printf("hello world hdrl strh \n ");
                                    aviStreamHeader.fcc = FOURCC_strh;
                                    aviStreamHeader.cb = readFileDW();
                                    aviStreamHeader.fccType = readFileDW();

                                    aviStreamHeader.fccHandler = readFileDW();
                                    aviStreamHeader.dwFlags = readFileDW();
                                    aviStreamHeader.wPriority = readFileW();
                                    aviStreamHeader.wLanguage = readFileW();
                                    aviStreamHeader.dwInitialFrames = readFileDW();
                                    aviStreamHeader.dwScale = readFileDW();
                                    aviStreamHeader.dwRate = readFileDW();
                                    aviStreamHeader.dwStart = readFileDW();
                                    aviStreamHeader.dwLength = readFileDW();
                                    aviStreamHeader.dwSuggestedBufferSize = readFileDW();
                                    aviStreamHeader.dwQuality = readFileDW();
                                    aviStreamHeader.dwSampleSize = readFileDW();
                                    aviStreamHeader.rcFrame.left = readFileW();
                                    aviStreamHeader.rcFrame.top = readFileW();
                                    aviStreamHeader.rcFrame.right = readFileW();
                                    aviStreamHeader.rcFrame.bottom = readFileW();

                                    // 跳过长度
                                    tmpOff += 4;
                                    tmpOff += aviStreamHeader.cb;
                                }
                                else if (fourcc == FOURCC_strf)
                                {
                                    printf("hello world hdrl strf  5555 \n");
                                    int tmpLen = readFileDW();
                                    if (aviStreamHeader.fccType == FOURCC_vids)
                                    {
                                        printf("hello world hdrl strf  5555 9999 \n");
                                        _hasVideo = true;
                                        _aviVideoStreamHeader = aviStreamHeader;
                                        _bitmapInfo.bmiHeader.biSize = readFileDW();
                                        _bitmapInfo.bmiHeader.biWidth = readFileDW();
                                        _bitmapInfo.bmiHeader.biHeight = readFileDW();
                                        _bitmapInfo.bmiHeader.biPlanes = readFileW();
                                        _bitmapInfo.bmiHeader.biBitCount = readFileW();
                                        _bitmapInfo.bmiHeader.biCompression = readFileDW();
                                        _bitmapInfo.bmiHeader.biSizeImage = readFileDW();
                                        _bitmapInfo.bmiHeader.biXPelsPerMeter = readFileDW();
                                        _bitmapInfo.bmiHeader.biYPelsPerMeter = readFileDW();
                                        _bitmapInfo.bmiHeader.biClrUsed = readFileDW();
                                        _bitmapInfo.bmiHeader.biClrImportant = readFileDW();
                                        if (tmpLen > _bitmapInfo.bmiHeader.biSize){
                                            printf("hello world hdrl strf  5555 9999  tmpLen %u  \n",tmpLen);
                                            printf("hello world hdrl strf  5555 9999  _bitmapInfo.bmiHeader.biSize %u  \n",_bitmapInfo.bmiHeader.biSize);
                                            fseek(_aviFd, tmpLen - _bitmapInfo.bmiHeader.biSize, SEEK_CUR);
                                        }

                                    }
                                    else if (aviStreamHeader.fccType == FOURCC_auds)
                                    {
                                        printf("hello world hdrl strf  5555 11111 \n");
                                        _hasAudio = true;
                                        _aviAudioStreamHeader = aviStreamHeader;
                                        _waveInfo.wFormatTag = readFileW();
                                        printf("hello world _waveInfo.wFormatTag  %u ... \n",_waveInfo.wFormatTag);
                                        _waveInfo.nChannels = readFileW();
                                        printf("hello world    _waveInfo.nChannels  %u ... \n",   _waveInfo.nChannels);
                                        _waveInfo.nSamplesPerSec = readFileDW();
                                        printf("hello world   _waveInfo.nSamplesPerSec  %u ... \n",   _waveInfo.nSamplesPerSec );
                                        _waveInfo.nAvgBytesPerSec = readFileDW();
                                        printf("hello world     _waveInfo.nAvgBytesPerSec  %u ... \n",    _waveInfo.nAvgBytesPerSec );
                                        _waveInfo.nBlockAlign = readFileW();
                                        printf("hello world      _waveInfo.nBlockAlign   %u ... \n",      _waveInfo.nBlockAlign );
                                        _waveInfo.wBitsPerSample = readFileW();
                                        printf("hello world         _waveInfo.wBitsPerSample   %u ... \n",       _waveInfo.wBitsPerSample );
                                        _waveInfo.cbSize = readFileW();
                                        printf("hello world _waveInfo.cbSize  %u ... \n",_waveInfo.cbSize);
                                              strnOff = ftell(_aviFd);
                                        char tmpBufq[4] = {0};
                                        readFile(tmpBufq, 4);

                                        printf("hello world  strf 1  %s \n",tmpBufq);

                                        //                                        fseek(_aviFd, _waveInfo.cbSize, SEEK_CUR);

                                        /*
                                                #include <mmreg.h>
                                                WAVE_FORMAT_PCM
                                                WAVE_FORMAT_ALAW
                                                */
                                    }
                                    printf("hello world hdrl strf 1 \n");
                                    // 跳过长度
                                    tmpOff += 4;
                                    tmpOff += tmpLen;
                                }
                                else if (fourcc == FOURCC_strn)
                                {
                                    printf("hello world hdrl FOURCC_strn 1 \n");
                                    int tmpLen = readFileDW();
                                    fseek(_aviFd, tmpLen, SEEK_CUR);

                                    // 跳过长度
                                    tmpOff += 4;
                                    tmpOff += tmpLen;
                                }
                                else if (fourcc == FOURCC_JUNK)
                                {
                                    printf("hello world hdrl FOURCC_JUNK 1 \n");
                                    int tmpLen = readFileDW();
                                    fseek(_aviFd, tmpLen, SEEK_CUR);

                                    // 跳过长度
                                    tmpOff += 4;
                                    tmpOff += tmpLen;
                                }
                                else if (fourcc == FOURCC_vprp)
                                {
                                    int tmpLen = readFileDW();
                                    fseek(_aviFd, tmpLen, SEEK_CUR);

                                    // 跳过长度
                                    tmpOff += 4;
                                    tmpOff += tmpLen;
                                }
                            }
                            printf("hello world hdrl strf  off 2 %d  \n",off);
                            printf("hello world hdrl strf avListLen 2 %d  \n",avListLen);
                            off += avListLen + 8;
                            //tiao guo  30 zijie   strn
                            //                            fseek(_aviFd,30,SEEK_CUR);
                            printf("hello world hdrl off %d  \n",off);
                        }
                            break;
                        case FOURCC_JUNK:
                        {
                            // 跳过JUNK
                            printf("hello world hdrl FOURCC_JUNK 2 \n");
                            off += 4;
                            int tmpLen = readFileDW();
                            fseek(_aviFd, tmpLen, SEEK_CUR);

                            // 跳过长度
                            off += 4;
                            off += tmpLen;
                        }
                            break;
                        }
                    }
                    printf("hello world hdrl ............ \n");
                }
                printf("hello world hdrl ............1 \n");

            }else if (fourcc2 == FOURCC_INFO) {
                fourcc = readFileDW();
                if (fourcc == FOURCC_ISFT)
                {
                    int tmpLen = readFileDW();
                    _soft.resize(tmpLen);
                    readFile(&_soft[0], tmpLen);
                }
            }else if(fourcc2 ==FOURCC_movi){
                _moviOff = ftell(_aviFd);
                if (hasIndex)
                {
                    // 跳过movi,直接到idx处
                    fseek(_aviFd, size - 4, SEEK_CUR);
                }
            }

            //            switch (fourcc2)
            //            {
            //            case FOURCC_hdrl:

            //                break;
            //            case FOURCC_INFO:
            //            {

            //            }
            //                break;
            //            case FOURCC_movi:
            //            {


            //            }
            //                break;
            //            }
            printf("hello world hdrl ............2 \n");
            index =2;
        }
        else if (fourcc == FOURCC_idx1)
        {
            printf("hello world idx1 %x \n",FOURCC_LIST);
            _aviIndex.fcc = FOURCC_idx1;
            _aviIndex.cb = size;

            int tmpOff = 0;
            while (tmpOff < _aviIndex.cb)
            {
                char tmpBuf[4] = {0};
                readFile(tmpBuf, 4);
                int index = read_le_dw(tmpBuf);
                AVIIndexEntry tmpEntry;
                tmpEntry.dwFlags = readFileDW();
                tmpEntry.dwOffset = readFileDW();
                tmpEntry.dwSize = readFileDW();
                if (tmpEntry.dwSize > _maxFrameSize)
                    _maxFrameSize = tmpEntry.dwSize;
                // 视频数据
                if (tmpBuf[2] == 'd')
                {
                    _aviIndex.videoIndexMap[index].push_back(tmpEntry);
                }
                else if (tmpBuf[2] == 'w')
                {
                    _aviIndex.audioIndexMap[index].push_back(tmpEntry);
                }
                // 一个索引信息的长度
                tmpOff += 16;
            }
        }
        else if (fourcc ==  FOURCC_JUNK)
        {
            // 跳过
            printf("hello world hdrl FOURCC_JUNK 3 \n");
            fseek(_aviFd, size, SEEK_CUR);
        }

    }
    printf("hello world hdrl whlie  mowei  3 \n");

}

bool AVIFileParser::readFile( char* buf, uint32_t len )
{
    uint32_t ret = fread(buf, 1, len, _aviFd);
    if (ret != len)
    {
        if (feof(_aviFd) != 0)
            return true;

#ifndef NPRINT
        printf("fread avi error: 1 \n");
#endif
        throw ERROR_INVALID_AVI;
    }

    return false;
}

bool AVIFileParser::readFileTagSize( DWORD& fourcc, uint32_t& size, DWORD& fourcc2 )
{
    char tmpBuf[12] = {0};
    bool ret = readFile(tmpBuf, 12);
    fourcc = read_le_dw(tmpBuf);
    size = read_le_dw(tmpBuf + 4);
    fourcc2 = read_le_dw(tmpBuf + 8);
    return ret;
}

uint32_t AVIFileParser::readFileDW()
{
    char tmpBuf[4] = {0};
    readFile(tmpBuf, 4);
    return read_le_dw(tmpBuf);
}

bool AVIFileParser::readFileDW( uint32_t& dw )
{
    char tmpBuf[4] = {0};
    bool ret = readFile(tmpBuf, 4);
    dw = read_le_dw(tmpBuf);
    return ret;
}

uint16_t AVIFileParser::readFileW()
{
    char tmpBuf[2] = {0};
    readFile(tmpBuf, 2);
    return read_le_w(tmpBuf);
}

bool AVIFileParser::readFileW( uint16_t& w )
{
    char tmpBuf[2] = {0};
    bool ret = readFile(tmpBuf, 2);
    w = read_le_w(tmpBuf);
    return ret;
}

int32_t AVIFileParser::getVideoFrame( char* buf, int32_t index /*= -1*/ )
{
    if (!_hasVideo)
        return -1;

    // 只取第一个
    std::vector<AVIIndexEntry>& videoVec = _aviIndex.videoIndexMap.begin()->second;
    int32_t tmpIndex = 0;
    if (index == -1)
    {
        if (_currVideoIndex < videoVec.size())
            tmpIndex = _currVideoIndex++;
        else
            return 0;
    }
    else
        tmpIndex = index;

    AVIIndexEntry& videoEntry = videoVec[tmpIndex];
    // 从movi结尾加上偏移后是dc db的数据,然后再跳过4字节的一个长度值
    fseek(_aviFd, _moviOff + videoEntry.dwOffset + 4, SEEK_SET);
    int32_t ret = fread(buf, 1, videoEntry.dwSize, _aviFd);
    assert(ret == videoEntry.dwSize);
    return ret;
}

bool AVIFileParser::seekVideoFrame( int32_t index )
{
    std::vector<AVIIndexEntry>& videoVec = _aviIndex.videoIndexMap.begin()->second;
    if (index < videoVec.size())
    {
        _currVideoIndex = index;
        return true;
    }

    return false;
}

void AVIFileParser::writeAVI(int index)
{
    FILE * fp = fopen("/home/djh/QtProject/AviParser_Djh/aab", "w+");
    unsigned int totalSIze = 639695360;
    char * riff ="RIFF";
    //    fseek(_aviFd,0L,SEEK_SET);
    printf("hello world _moviOff %u \n",_moviOff);
    //        char *copybuff ={0};
    //        copybuff = (char *) malloc(sizeof(char)*totalSIze);
    //    fread(copybuff,1,totalSIze,_aviFd);*******
    //        fwrite(copybuff,totalSIze,1,fp);
    fseek(_aviFd, _moviOff, SEEK_SET);
    //    char buff[4] = {0};
    //    int32_t ret = fread(buff, 1,4, _aviFd);
    //    printf("hello world exec %s \n",buff);
    //    printf("hello world exec %u \n",read_le_w(buff));

    //    ret = fread(buff, 1,4, _aviFd);
    //    printf("hello world exec %s \n",buff);
    //    unsigned int size = read_le_w(buff);
    //    printf("hello world exec %u \n",size);

    //        fseek(_aviFd, size, SEEK_CUR);


    //    char *copybuff ={0};
    //    copybuff = (char *) malloc(sizeof(char)*size);
    //    memset( copybuff, 0, size*sizeof(char) );
    //    fread(copybuff, 1,size, _aviFd);
    char buff[4] = {0};

    int  movisize=0;
    int i=0;
    while(i<1000){
        i++;
        int32_t ret = fread(buff, 1,4, _aviFd);
        printf("hello world exec %s \n",buff);
        unsigned int size = 0;
        ret = fread(buff, 1,4, _aviFd);
        size = read_le_w(buff);
        printf("hello world exec %u \n",size);
        fseek(_aviFd, size, SEEK_CUR);
        movisize =8+size;
    }


    //    ret = fread(buff, 1,4, _aviFd);
    //    printf("hello world exec %s \n",buff);

    ////    fwrite(riff,4,1,fp);
    ////    fwrite(riff,4,1,fp);
    //    fseek(fp,2,SEEK_SET);
    //      char * riff1="d";
    fwrite(riff,4,1,fp);
    int aa =123;
    //   uint32_t  refLen = _fLen -
    uint32_t  refLen = movisize+8;


    refLen += _aviMainHeader.cb;
    _aviMainHeader.dwTotalFrames =1000;
    _aviAudioStreamHeader.dwFlags=_aviAudioStreamHeader.dwFlags&AVIFILEINFO_HASINDEX_F;
    refLen +=4;



    refLen +=  _aviVideoStreamHeader.cb;
    refLen +=4;

    refLen +=_bitmapInfo.bmiHeader.biSize;
    refLen +=4;

    refLen +=8;


    refLen +=  _aviAudioStreamHeader.cb;
    refLen +=4;

    refLen +=16;
    refLen +=4;







    fwrite("RIFF",4,1,fp);
      fwrite(&refLen,4,1,fp);
      fwrite("AVI ",4,1,fp);
      fwrite("LIST",4,1,fp);
  fwrite("LIST",4,1,fp);

    printf("hello world exec movisize %u \n",movisize);
    //    printf("hello world exec\n");
    fclose(fp);
}
