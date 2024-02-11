#ifndef IMGFOLDER_H_
#define IMGFOLDER_H_

#include <File.h>
#include <FAT.h>

class ImgFolder
{
public:
    ImgFolder(FAT *fs);
    ~ImgFolder();
    void init(File& root_dir, const char* folder_name);

    bool next_file(File& imgFile);
    bool prev_file(File& imgFile);
    uint8_t get_image_count() { return image_count; }
    bool get_current_file_name(char* buffer);
    bool next_available() { return index < max_index; }
    bool prev_available() { return index > 0; }
    int8_t get_index() { return index; }

private:
    File dir;
    int8_t index;
    uint8_t max_index;
    char name_buffer[16];
    uint8_t image_count;

    bool next_file_name(char* buffer);
    bool prev_file_name(char* buffer);
};
#endif /* IMGFOLDER_H_ */