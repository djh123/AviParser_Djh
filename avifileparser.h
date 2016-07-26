#ifndef AVIFILEPARSER_H
#define AVIFILEPARSER_H


/*!
@brief avi文件分析提取器
@author jwybobo2007
@file avifileparser.h
@note 代码在使用时，请标明作者，保留出处：http://blog.csdn.net/jwybobo2007
*/
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <map>

//////////////////////////////////////////////////////////////////////////
#define AVIIF_KEYFRAME  0x00000010L     // 索引里面的关键帧标志
#define AVIIF_LIST  0x00000001L

// Flags for dwFlags|AVI头中的标志位
#define AVIFILEINFO_HASINDEX        0x00000010  // 是否有索引
#define AVIFILEINFO_HASINDEX_F    0x11111801  // 是否有索引
#define AVIFILEINFO_MUSTUSEINDEX    0x00000020
#define AVIFILEINFO_ISINTERLEAVED   0x00000100
#define AVIFILEINFO_WASCAPTUREFILE  0x00010000
#define AVIFILEINFO_COPYRIGHTED     0x00020000
#define read_le_w(p) ((uint32_t(((uint8_t*)(p))[3]) << 24) | (uint32_t(((uint8_t*)(p))[2]) << 16) | (uint32_t(((uint8_t*)(p))[1]) << 8) | (uint32_t(((uint8_t*)(p))[0])))
#define read_le_dw(p) ((uint32_t(((uint8_t*)(p))[3]) << 24) | (uint32_t(((uint8_t*)(p))[2]) << 16) | (uint32_t(((uint8_t*)(p))[1]) << 8) | (uint32_t(((uint8_t*)(p))[0])))
// 最大允许的AVI头大小
#define MAX_ALLOWED_AVI_HEADER_SIZE 131072

// 打不开文件
#define ERROR_OPEN_AVI              0
// 不是有效AVI
#define ERROR_INVALID_AVI           1

// 由于编译器c,c++标准过低，没有包含stdint.h头文件，并且现在也没有使用boost库，因此定义一些类型，来兼容以前的代码

//typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
//typedef long long int64_t;
//typedef unsigned long long uint64_t;


// 双字
typedef uint32_t DWORD;
// 单字
typedef uint16_t WORD;
// 定义长整型
typedef DWORD LONG;
// 字节
typedef uint8_t BYTE;
// 定义four cc;
typedef DWORD FourCC;

// 定义fourcc对应的整数值，avi文件中保存的是小端
const DWORD FOURCC_RIFF = 0x46464952;
const DWORD FOURCC_AVI = 0x20495641;
const DWORD FOURCC_LIST = 0x5453494C;
const DWORD FOURCC_hdrl = 0x6C726468;
const DWORD FOURCC_avih = 0x68697661;
const DWORD FOURCC_strl = 0x6C727473;
const DWORD FOURCC_strh = 0x68727473;
const DWORD FOURCC_strn = 0x6E727473;
const DWORD FOURCC_strf = 0x66727473;
const DWORD FOURCC_STRD = 0x64727473;
const DWORD FOURCC_vids = 0x73646976;
const DWORD FOURCC_auds = 0x73647561;
const DWORD FOURCC_INFO = 0x4F464E49;
const DWORD FOURCC_ISFT = 0x54465349;
const DWORD FOURCC_idx1 = 0x31786469;
const DWORD FOURCC_movi = 0x69766F6D;
const DWORD FOURCC_JUNK = 0x4B4E554A;
const DWORD FOURCC_vprp = 0x70727076;
const DWORD FOURCC_PAD = 0x20444150;
const DWORD FOURCC_DIV3 = 861292868;
const DWORD FOURCC_DIVX = 1482049860;
const DWORD FOURCC_XVID = 1145656920;
const DWORD FOURCC_DX50 = 808802372;

const DWORD FOURCC_fmt = 0x20746D66;    // for WAVE files
const DWORD FOURCC_data = 0x61746164;   // for WAVE files
const DWORD FOURCC_WAVE = 0x45564157;   // for WAVE files


/*!
@brief 从缓冲区中以LE方式读取uint32_t
@param p 缓冲区指针
*/



// 调色板
typedef struct
{
    BYTE rgbBlue;                       // 蓝
    BYTE rgbGreen;                      // 绿
    BYTE rgbRed;                        // 红
    BYTE rgbReserved;                   // 保留
} RGBQUAD;

// AVI主头部
typedef struct
{
    FourCC fcc;                         // 必须为 avih
    DWORD cb;                           // 本数据结构的大小，不包括最初的8个字节（fcc和cb两个域）
    DWORD dwMicroSecPerFrame;           // 视频帧间隔时间（以毫秒为单位）
    DWORD dwMaxBytesPerSec;             // 这个AVI文件的最大数据率
    DWORD dwPaddingGranularity;         // 数据填充的粒度
    DWORD dwFlags;                      // AVI文件的全局标记，比如是否含有索引块等
    DWORD dwTotalFrames;                // 总帧数
    DWORD dwInitialFrames;              // 为交互格式指定初始帧数（非交互格式应该指定为0）
    DWORD dwStreams;                    // 本文件包含的流的个数
    DWORD dwSuggestedBufferSize;        // 建议读取本文件的缓存大小（应能容纳最大的块）
    DWORD dwWidth;                      // 视频图像的宽（以像素为单位）
    DWORD dwHeight;                     // 视频图像的高（以像素为单位）
    DWORD dwReserved[4];                // 保留
} AVIMainHeader;

// 定义矩形区域
typedef struct
{
    short int left;                     // 总边距
    short int top;                      // 顶边距
    short int right;                    // 右边距
    short int bottom;                   // 底边距
}RECT;

// AVI流头部
typedef struct
{
    FourCC fcc;                         // 必须为 strh
    DWORD cb;                           // 本数据结构的大小,不包括最初的8个字节(fcc和cb两个域)
    FourCC fccType;                     // 流的类型: auds(音频流) vids(视频流) mids(MIDI流) txts(文字流)
    FourCC fccHandler;                  // 指定流的处理者，对于音视频来说就是解码器
    DWORD dwFlags;                      // 标记：是否允许这个流输出？调色板是否变化？
    WORD wPriority;                     // 流的优先级（当有多个相同类型的流时优先级最高的为默认流）
    WORD wLanguage;                     // 语言
    DWORD dwInitialFrames;              // 为交互格式指定初始帧数
    DWORD dwScale;                      // 每帧视频大小或者音频采样大小
    DWORD dwRate;                       // dwScale/dwRate，每秒采样率
    DWORD dwStart;                      // 流的开始时间
    DWORD dwLength;                     // 流的长度（单位与dwScale和dwRate的定义有关）
    DWORD dwSuggestedBufferSize;        // 读取这个流数据建议使用的缓存大小
    DWORD dwQuality;                    // 流数据的质量指标（0 ~ 10,000）
    DWORD dwSampleSize;                 // Sample的大小
    RECT rcFrame;                       // 指定这个流（视频流或文字流）在视频主窗口中的显示位置，视频主窗口由AVIMAINHEADER结构中的dwWidth和dwHeight决定
} AVIStreamHeader;

// 位图头
typedef struct
{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
} BitmapInfoHeader;

// 位图信息
typedef struct
{
    BitmapInfoHeader bmiHeader;         // 位图头
    RGBQUAD bmiColors[1];               // 调色板
} BitmapInfo;

// 音频波形信息
typedef struct
{
    WORD wFormatTag;
    WORD nChannels;                     // 声道数
    DWORD nSamplesPerSec;               // 采样率
    DWORD nAvgBytesPerSec;              // 每秒的数据量
    WORD nBlockAlign;                   // 数据块对齐标志
    WORD wBitsPerSample;                // 每次采样的数据量
    WORD cbSize;                        // 大小
} WaveFormatEx;

// 索引节点信息
typedef struct
{
    DWORD dwChunkId;                    // 本数据块的四字符码(00dc 01wb)
    DWORD dwFlags;                      // 说明本数据块是不是关键帧、是不是‘rec ’列表等信息
    DWORD dwOffset;                     // 本数据块在文件中的偏移量
    DWORD dwSize;                       // 本数据块的大小
} AVIIndexEntry;

// 索引信息
typedef struct
{
    FourCC fcc;                                                         // 必须为‘idx1’
    DWORD cb;                                                           // 本数据结构的大小，不包括最初的8个字节（fcc和cb两个域）
    uint32_t position;                                                  // 数据起始位置偏移
    std::map<uint32_t, std::vector<AVIIndexEntry> > videoIndexMap;      // 视频索引表,00dc等转换成整形表示
    std::map<uint32_t, std::vector<AVIIndexEntry> > audioIndexMap;      // 音频索引表,00wb等转换成整形表示
} AVIIndex;

/*!
@brief avi文件分析提取器
*/
class AVIFileParser
{
public:
    AVIFileParser();
    AVIFileParser(const char* file);
    ~AVIFileParser(void);
      FILE* _aviFd;
      // movi的开始位置偏移
      uint32_t _moviOff;
        uint32_t strnOff;
public:
    /*!
    @brief 打开AVI文件
    */
    void openAVI(const char* file);
    /*!
    @brief 是否是有效avi
    */
    bool isValid(){return _isValid;}
    /*!
    @brief 是否有音频
    */
    bool hasAudio(){return _hasAudio;}
    /*!
    @brief 是否有视频
    */
    bool hasVideo(){return _hasVideo;}
    /*!
    @brief 返回avi头
    */
    const AVIMainHeader* aviMainHeader(){return &_aviMainHeader;}
    /*!
    @brief 返回avi视频流头
    */
    const AVIStreamHeader* aviVideoStreamHeader(){return &_aviVideoStreamHeader;}
    /*!
    @brief 返回avi音频流头
    */
    const AVIStreamHeader* aviAudioStreamHeader(){return &_aviAudioStreamHeader;}
    /*!
    @brief 返回位图信息
    */
    const BitmapInfo* bitmapInfo(){return &_bitmapInfo;}
    /*!
    @brief 返回音频信息
    */
    const WaveFormatEx* waveFormatEx(){return &_waveInfo;}
    /*!
    @brief 最大视频帧大小
    */
    uint32_t maxFrameSize(){return _maxFrameSize;}
    /*!
    @brief 获取视频帧
    @param buf 视频帧存储缓冲
    @param index 帧索引,默认-1,表示从当前的索引继续往下读取
    @return 视频帧大小
    */
    int32_t getVideoFrame(char* buf, int32_t index = -1);
    /*!
    @brief 偏移视频帧从指定位置开始
    @param index 指定索引
    */
    bool seekVideoFrame(int32_t index);
    /*!
    @brief 获取音频帧
    */


    void writeAVI(int index);
private:
    /// 解析AVI
    void _parseFile();
    /// 从文件读指定长度的数据,出错自动抛出异常
    bool readFile(char* buf, uint32_t len);
    /// 从文件读12个字节并分析
    bool readFileTagSize(DWORD& fourcc, uint32_t& size, DWORD& fourcc2);
    /// 从文件读4字节
    uint32_t readFileDW();

    /// 从文件读2字节
    uint16_t readFileW();
    //////////////////////////////////////////////////////////////////////////
    // 重载
    /// 从文件读4字节
    bool readFileDW(uint32_t& dw);
    /// 从文件读2字节
    bool readFileW(uint16_t& w);
private:
    // 文件句柄
//    FILE* _aviFd;
    // 是否有音频
    bool _hasAudio;
    // 是否有视频
    bool _hasVideo;
    // 是否有效
    bool _isValid;
    // 文件长度
    uint32_t _fLen;
    // avi主头
    AVIMainHeader _aviMainHeader;
    // avi视频流头部
    AVIStreamHeader _aviVideoStreamHeader;
    // avi音频流头部
    AVIStreamHeader _aviAudioStreamHeader;
    // 位图信息
    BitmapInfo _bitmapInfo;
    // 音频信息
    WaveFormatEx _waveInfo;
    // 编码程序
    std::string _soft;
    // 索引信息
    AVIIndex _aviIndex;
    // movi的开始位置偏移
//    uint32_t _moviOff;
    // 最大帧大小
    uint32_t _maxFrameSize;
    // 当前视频位置索引(数组中的位置,下标)
    int32_t _currVideoIndex;
};

#endif // AVIFILEPARSER_H
