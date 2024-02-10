#include <ImgFolder.h>

ImgFolder::ImgFolder(FAT *fs) : dir(fs), index(-1), max_index(INT8_MAX)
{
    memset(name_buffer, 0, sizeof(name_buffer));
}

ImgFolder::~ImgFolder()
{
    dir.close();
}

void ImgFolder::init(File& root_dir, const char* folder_name)
{
    dir.open(root_dir, folder_name, File::O_RDONLY);
}

bool ImgFolder::next_file(File& imgFile)
{
    memset(name_buffer, 0, sizeof(name_buffer));
    if (!next_file_name(name_buffer))
    {
        return false;
    }
    return imgFile.open(dir, name_buffer, File::O_RDONLY);
}
bool ImgFolder::prev_file(File& imgFile)
{
    memset(name_buffer, 0, sizeof(name_buffer));
    if (!prev_file_name(name_buffer))
    {
        return false;
    }
    return imgFile.open(dir, name_buffer, File::O_RDONLY);
}

bool ImgFolder::next_file_name(char* buffer)
{
    if (index + 1 > max_index)
    {
        return false;
    }
    uint8_t curr_index = index;
    dir.ls(buffer, File::LS_FILE, ++index);
    if (strlen(buffer) == 0 || strcasestr(buffer, ".bmp") == NULL)
    {
        index = curr_index;
        max_index = curr_index;
        return false;
    }
    return true;
}

bool ImgFolder::prev_file_name(char* buffer)
{
    if (index - 1 < 0)
    {
        return false;
    }
    uint8_t curr_index = index;
    dir.ls(buffer, File::LS_FILE, --index);
    if (strlen(buffer) == 0 || strcasestr(buffer, ".bmp") == NULL)
    {
        index = curr_index;
        return false;
    }
    return true;
}