#include <ImgFolder.h>

ImgFolder::ImgFolder(FAT* fs)
    : dir(fs), index(-1), max_index(INT8_MAX), image_count(0), loop_on_end(true)
{
}

ImgFolder::ImgFolder(FAT* fs, bool loop)
    : dir(fs), index(-1), max_index(INT8_MAX), image_count(0), loop_on_end(loop)
{
}

ImgFolder::~ImgFolder()
{
    dir.close();
}

void ImgFolder::init(File& root_dir, const char* folder_name)
{
    dir.open(root_dir, folder_name, File::O_RDONLY);
    uint8_t count = 0;
    while (dir.ls(name_buffer, File::LS_FILE, count++))
    {
        if (strlen(name_buffer) == 0)
        {
            break;
        }
        if (strcasestr(name_buffer, ".bmp") != NULL)
        {
            image_count++;
        }
    }
    max_index = image_count - 1;
    memset(name_buffer, 0, sizeof(name_buffer));
}

bool ImgFolder::get_current_file_name(char* buffer)
{
    if (index < 0)
    {
        return false;
    }
    return dir.ls(buffer, File::LS_FILE, index);
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
        if (loop_on_end)
        {
            index = -1;
        } else {
            return false;
        }
    }
    uint8_t curr_index = index;
    dir.ls(buffer, File::LS_FILE, ++index);
    if (strlen(buffer) == 0 || strcasestr(buffer, ".bmp") == NULL)
    {
        index = curr_index;
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