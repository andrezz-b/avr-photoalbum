/**
 * @file ImgFolder.cpp
 * @brief Represents the folder containing images.
 *
 * This file contains the implementation of the ImgFolder class, which represents a folder
 * containing image files. The ImgFolder class provides methods for initializing the folder,
 * retrieving file names, and opening files.
 */
#include <ImgFolder.h>

/**
 * @brief Constructs a new ImgFolder object with looping enabled.
 *
 * @param fs Pointer to the FAT object.
 */
ImgFolder::ImgFolder(FAT* fs)
    : dir(fs), index(-1), max_index(INT8_MAX), image_count(0), loop_on_end(true)
{
}

/**
 * @brief Constructs a new ImgFolder object.
 *
 * @param fs Pointer to the FAT object.
 * @param loop Flag indicating whether to loop back to the beginning of the folder when reaching the
 * end.
 */
ImgFolder::ImgFolder(FAT* fs, bool loop)
    : dir(fs), index(-1), max_index(INT8_MAX), image_count(0), loop_on_end(loop)
{
}

/**
 * @brief Destructor for the ImgFolder class.
 *
 * @details This destructor closes the directory associated with the ImgFolder object.
 */
ImgFolder::~ImgFolder()
{
    dir.close();
}

/**
 * @brief Initializes the ImgFolder object with the specified root directory and folder name.
 *
 * @details This function opens the directory specified by `root_dir` and `folder_name` in read-only
 * mode. It then counts the number of BMP files in the directory and updates the `image_count` and
 * `max_index` variables accordingly. Finally, it clears the `name_buffer` array.
 *
 * @param root_dir The root directory where the folder is located.
 * @param folder_name The name of the folder to be initialized.
 */
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

/**
 * @brief Get the current file name.
 *
 * @details This function retrieves the name of the current file in the ImgFolder.
 * It is a workaround because the File class does not provide direct methods
 * to get the current file name.
 *
 * @param buffer A pointer to a character array where the file name will be stored.
 * @return true if the file name was successfully retrieved, false otherwise.
 */
bool ImgFolder::get_current_file_name(char* buffer)
{
    if (index < 0)
    {
        return false;
    }
    return dir.ls(buffer, File::LS_FILE, index);
}

/**
 * @brief Retrieves the next file in the image folder.
 *
 * @details This function retrieves the next file in the image folder and opens it using the
 * provided `imgFile` object.
 *
 * @param imgFile The `File` object used to open the next file.
 * @return `true` if the next file was successfully retrieved and opened, `false` otherwise.
 */
bool ImgFolder::next_file(File& imgFile)
{
    memset(name_buffer, 0, sizeof(name_buffer));
    if (!next_file_name(name_buffer))
    {
        return false;
    }
    return imgFile.open(dir, name_buffer, File::O_RDONLY);
}
/**
 * @brief Moves to the previous file in the image folder and opens it.
 *
 * @details This function moves to the previous file in the image folder and opens it using the
 * provided `imgFile` object. It first clears the `name_buffer` using `memset` and then calls
 * `prev_file_name` to get the previous file name. If `prev_file_name` returns false, indicating
 * that there are no more previous files, this function returns false. Otherwise, it opens the file
 * using `imgFile.open` with the directory and file name, and returns the result.
 *
 * @param imgFile The `File` object used to open the image file.
 * @return `true` if the previous file was successfully opened, `false` otherwise.
 */
bool ImgFolder::prev_file(File& imgFile)
{
    memset(name_buffer, 0, sizeof(name_buffer));
    if (!prev_file_name(name_buffer))
    {
        return false;
    }
    return imgFile.open(dir, name_buffer, File::O_RDONLY);
}

/**
 * @brief Retrieves the next file name from the image folder.
 *
 * @details This function retrieves the next file name from the image folder and stores it in the
 * provided buffer. If loop_on_end is enabled, the function will reset the index to -1 when the end
 * of the folder is reached which will load the first file in the folder.
 *
 * @param buffer The buffer to store the file name.
 * @return True if a valid file name is retrieved, false otherwise.
 */
bool ImgFolder::next_file_name(char* buffer)
{
    if (index + 1 > max_index)
    {
        if (loop_on_end)
        {
            index = -1;
        }
        else
        {
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

/**
 * @brief Retrieves the previous file name in the image folder.
 *
 * @details This function retrieves the name of the previous file in the image folder.
 * It updates the provided buffer with the file name and returns true if successful,
 * otherwise returns false.
 *
 * @param buffer A pointer to a character array to store the file name.
 * @return true if the previous file name is retrieved successfully, false otherwise.
 */
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