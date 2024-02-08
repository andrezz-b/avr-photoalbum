#ifndef IMGFOLDER_H_
#define IMGFOLDER_H_

#include <File.h>

class ImgFolder
{
public:
    ImgFolder(File& dir);
    ~ImgFolder();
    bool first_file(File& imgFile);

    bool next_file(File& imgFile);
    bool prev_file(File& imgFile);

private:
    File dir;
    int8_t index;
    uint8_t max_index;
    char name_buffer[16];

    bool next_file_name(char* buffer);
    bool prev_file_name(char* buffer);
};
#endif /* IMGFOLDER_H_ */