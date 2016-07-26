#include"aiv.h"
AVI::AVI(){

}

AVI::~AVI(){

}

void AVI:: setListHDRL(LIST_HDRL hdrl){
    list_hdrl = hdrl;
}

void AVI:: setListMOVI(LIST_MOVI movi){
    list_movi = movi;
}

void AVI::writeListHDRL(){
    fwrite(list_hdrl.name_list,4,1,fp);
    fwrite(&list_hdrl.list_hdrl_size,4,1,fp);
    fwrite(list_hdrl.name_hdrl,4,1,fp);
    writeAvih();

    writeStrlVids();
    writeStrlAuds();
}

void AVI::writeAvih(){
    char *avih = "avih";
    fwrite(&list_hdrl.avih.fcc,4,1,fp);
    fwrite(&list_hdrl.avih.cb,4,1,fp);

    fwrite(&list_hdrl.avih.dwMicroSecPerFrame,4,1,fp);
    fwrite(&list_hdrl.avih.dwMaxBytesPerSec,4,1,fp);
    fwrite(&list_hdrl.avih.dwPaddingGranularity,4,1,fp);
    fwrite(&list_hdrl.avih.dwFlags,4,1,fp);
    fwrite(&list_hdrl.avih.dwTotalFrames,4,1,fp);
    fwrite(&list_hdrl.avih.dwInitialFrames,4,1,fp);
    fwrite(&list_hdrl.avih.dwStreams,4,1,fp);
    fwrite(&list_hdrl.avih.dwSuggestedBufferSize,4,1,fp);
    fwrite(&list_hdrl.avih.dwWidth,4,1,fp);
    fwrite(&list_hdrl.avih.dwHeight,4,1,fp);
    fwrite(&list_hdrl.avih.dwReserved,16,1,fp);
}

void AVI::writeStrlVids(){
    printf("writeStrlVids \n");
    fwrite(list_hdrl.strl_vids.name_list,4,1,fp);
    fwrite(&list_hdrl.strl_vids.list_strl_size,4,1,fp);
    fwrite(list_hdrl.strl_vids.name_strl,4,1,fp);

    // writer strh
    writeStrh(list_hdrl.strl_vids.strh);
    // writer strf

    fwrite("strf",4,1,fp);
    fwrite(&list_hdrl.strl_vids.strf.biSize,4,1,fp);

    fwrite(&list_hdrl.strl_vids.strf.biSize,4,1,fp);
    fwrite(&list_hdrl.strl_vids.strf.biWidth,4,1,fp);
    fwrite(&list_hdrl.strl_vids.strf.biHeight,4,1,fp);
    fwrite(&list_hdrl.strl_vids.strf.biPlanes,2,1,fp);
    fwrite(&list_hdrl.strl_vids.strf.biBitCount,2,1,fp);
    fwrite(&list_hdrl.strl_vids.strf.biCompression,4,1,fp);
    fwrite(&list_hdrl.strl_vids.strf.biSizeImage,4,1,fp);
    fwrite(&list_hdrl.strl_vids.strf.biXPelsPerMeter,4,1,fp);
    fwrite(&list_hdrl.strl_vids.strf.biYPelsPerMeter,4,1,fp);
    fwrite(&list_hdrl.strl_vids.strf.biClrUsed,4,1,fp);
    fwrite(&list_hdrl.strl_vids.strf.biClrImportant,4,1,fp);

}

void AVI::writeStrlAuds(){
    fwrite(list_hdrl.strl_auds.name_list,4,1,fp);
    fwrite(&list_hdrl.strl_auds.list_strl_size,4,1,fp);
    fwrite(list_hdrl.strl_auds.name_strl,4,1,fp);
    // writer strh
    writeStrh(list_hdrl.strl_auds.strh);
    // writer strf

    fwrite("strf",4,1,fp);
    unsigned int aa = 16;
    fwrite(&aa,4,1,fp);

    fwrite(&list_hdrl.strl_auds.strf.wFormatTag,2,1,fp);
    fwrite(&list_hdrl.strl_auds.strf.nChannels,2,1,fp);
    fwrite(&list_hdrl.strl_auds.strf.nSamplesPerSec,4,1,fp);
    fwrite(&list_hdrl.strl_auds.strf.nAvgBytesPerSec,4,1,fp);
    fwrite(&list_hdrl.strl_auds.strf.nBlockAlign,2,1,fp);
    fwrite(&list_hdrl.strl_auds.strf.wBitsPerSample,2,1,fp);

    fwrite(&list_hdrl.strl_auds.strf.cbSize,2,1,fp);
    //    fwrite("rn",2,1,fp);
    //    aa =30;
    //     fwrite(&aa,4,1,fp);

}

void AVI::writeStrh(AVIStreamHeader strh){
    fwrite(&strh.fcc,4,1,fp);
    fwrite(&strh.cb,4,1,fp);

    fwrite(&strh.fccType,4,1,fp);
    fwrite(&strh.fccHandler,4,1,fp);
    fwrite(&strh.dwFlags,4,1,fp);
    fwrite(&strh.wPriority,2,1,fp);
    fwrite(&strh.wLanguage,2,1,fp);
    fwrite(&strh.dwInitialFrames,4,1,fp);
    fwrite(&strh.dwScale,4,1,fp);
    fwrite(&strh.dwRate,4,1,fp);
    fwrite(&strh.dwStart,4,1,fp);
    fwrite(&strh.dwLength,4,1,fp);
    fwrite(&strh.dwSuggestedBufferSize,4,1,fp);
    fwrite(&strh.dwQuality,4,1,fp);
    fwrite(&strh.dwSampleSize,4,1,fp);
    fwrite(&strh.rcFrame.left,2,1,fp);
    fwrite(&strh.rcFrame.top ,2,1,fp);
    fwrite(&strh.rcFrame.right ,2,1,fp);
    fwrite(&strh.rcFrame.bottom ,2,1,fp);

}
void AVI::writeListMOVI(FILE *oriFp){
    printf("writeListMOVI \n");
    fwrite(list_movi.name_list,4,1,fp);
    fwrite(&list_movi.list_movi_size,4,1,fp);
    fwrite(list_movi.name_movi,4,1,fp);
    unsigned int moviDataSize = list_movi.list_movi_size-4;
    char *copybuff ={0};
    copybuff = (char *) malloc(sizeof(char)*(moviDataSize-4));
    fseek(oriFp, list_movi.moviOff, SEEK_SET);
    fread(copybuff,1,moviDataSize,oriFp);
    fwrite(copybuff,moviDataSize,1,fp);
}

void AVI::writeAviFile(FILE *orgfp, unsigned int  strnoff){
    fp = fopen("/home/djh/QtProject/AviParser_Djh/aac.avi", "w+");
    fwrite(name_riff,4,1,fp);
    fwrite(&totalSize,4,1,fp);
    fwrite(name_avi,4,1,fp);
    writeListHDRL();

    unsigned int strnsize =36;
    char *copybuff ={0};
    copybuff = (char *) malloc(sizeof(char)*(strnsize));
    fseek(orgfp, strnoff, SEEK_SET);
    fread(copybuff,1,strnsize,orgfp);
    fwrite(copybuff,strnsize,1,fp);


    writeListMOVI(orgfp);
    fclose(fp);
}

