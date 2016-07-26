#ifndef AIV_H
#define AIV_H

#include "avifileparser.h"
typedef struct
{
    char * name_list ="LIST";
    char * name_strl ="strl";
        uint32_t  list_strl_size;
    AVIStreamHeader strh;
    BitmapInfoHeader strf;

} LIST_STRL_VIDS;

typedef struct
{
    char * name_list ="LIST";
    char * name_strl ="strl";
        uint32_t  list_strl_size;
    AVIStreamHeader strh;
    WaveFormatEx strf;

} LIST_STRL_AUDS;


typedef struct
{
    char * name_list ="LIST";
    char * name_hdrl ="hdrl";
    uint32_t  list_hdrl_size;
    AVIMainHeader avih;
    LIST_STRL_VIDS strl_vids;
    LIST_STRL_AUDS strl_auds;
} LIST_HDRL;

typedef struct
{
    char * name_list ="LIST";
    char * name_movi ="movi";
    uint32_t  list_movi_size;
    uint32_t moviOff;
} LIST_MOVI;




class AVI {
    char * name_riff ="RIFF";
    char * name_avi ="AVI ";
    LIST_HDRL list_hdrl;
    LIST_MOVI list_movi;
public:
    uint32_t  totalSize;

public:
    AVI();
    void setListHDRL(LIST_HDRL hdrl);
    void setListMOVI(LIST_MOVI movi);
    void writeAviFile(FILE *fp,unsigned int  strnoff);
    ~AVI();

private:
    FILE * fp;
    void writeListHDRL();
    void writeListMOVI(FILE *oriFp);
    void writeAvih();
    void writeStrlVids();
    void writeStrlAuds();
    void writeStrh(AVIStreamHeader strh);
};

#endif // AIV_H
