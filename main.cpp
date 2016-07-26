#include <QCoreApplication>
#include"avifileparser.h"
#include"aiv.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    printf("hello world \n");
    AVIFileParser aviFileParser;
    aviFileParser.openAVI("/home/djh/QtProject/AviParser_Djh/dd.avi");
    //    aviFileParser.writeAVI(1);

    AVI avi ;

    //    avi.totalSize =639695260;



    //movi
    fseek(aviFileParser._aviFd,aviFileParser. _moviOff, SEEK_SET);
    int  movisize=0;
    int i=0;
    char buff[4] = {0};
    unsigned int movioffDiv =0;
    while(i<10000){
        i++;
        int32_t ret = fread(buff, 1,4, aviFileParser._aviFd);
        printf("hello world exec %s \n",buff);
        unsigned int size = 0;
        ret = fread(buff, 1,4,aviFileParser. _aviFd);
        size = read_le_w(buff);
        printf("hello world exec %u \n",size);
        fseek(aviFileParser._aviFd, size, SEEK_CUR);
        movioffDiv = movioffDiv +8+size;
    }

    while(i<20000){
        i++;
        int32_t ret = fread(buff, 1,4, aviFileParser._aviFd);
        printf("hello world exec %s \n",buff);
        unsigned int size = 0;
        ret = fread(buff, 1,4,aviFileParser. _aviFd);
        size = read_le_w(buff);
        printf("hello world exec %u \n",size);
        fseek(aviFileParser._aviFd, size, SEEK_CUR);
        movisize =movisize+8+size;
    }



    LIST_MOVI listMovi;
    listMovi.list_movi_size= movisize+4;
    listMovi.moviOff = aviFileParser._moviOff+movioffDiv;
    printf("hello world aviFileParser._moviOff %u \n",aviFileParser._moviOff);
    avi.setListMOVI(listMovi);

    //hdrl
    LIST_HDRL listHdrl;
    listHdrl.list_hdrl_size=330;
    listHdrl.avih =*(aviFileParser.aviMainHeader());
    listHdrl.avih.dwTotalFrames = 1000;
    listHdrl.avih.dwFlags=  listHdrl.avih.dwFlags&AVIFILEINFO_HASINDEX_F;
    listHdrl.avih.dwReserved[0]= 0;
    listHdrl.avih.dwReserved[1]= 0;
    listHdrl.avih.dwReserved[2]= 0;
    listHdrl.avih.dwReserved[3]= 0;

    listHdrl.strl_vids.list_strl_size =116;
    listHdrl.strl_vids.strh = *(aviFileParser.aviVideoStreamHeader());
    listHdrl.strl_vids.strf = (*(aviFileParser.bitmapInfo())).bmiHeader;

    listHdrl.strl_auds.list_strl_size = 130;
    listHdrl.strl_auds.strh=*(aviFileParser.aviAudioStreamHeader());
    listHdrl.strl_auds.strf = *(aviFileParser.waveFormatEx());
    avi.setListHDRL(listHdrl);

      avi.totalSize =330+movisize+4+16+12;


    avi.writeAviFile(aviFileParser._aviFd,aviFileParser.strnOff);
    return a.exec();
}
