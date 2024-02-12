/**
 * @file ImgFolder.h
 * @brief Represents the folder containing images.
 */
#ifndef IMGFOLDER_H_
#define IMGFOLDER_H_

#include <FAT.h>
#include <File.h>

/**
 * @class ImgFolder
 * @brief Represents a folder containing image files.
 *
 * This class provides functionality to navigate through image files within a folder.
 * It supports moving to the next and previous image file, retrieving the current file name,
 * and checking if there are more files available for navigation.
 */
class ImgFolder
{
public:
    ImgFolder(FAT* fs);

    ImgFolder(FAT* fs, bool loop);

    ~ImgFolder();

    void init(File& root_dir, const char* folder_name);
    bool next_file(File& imgFile);
    bool prev_file(File& imgFile);

    /**
     * @brief Gets the number of image files in the folder.
     * @return The number of image files.
     */
    uint8_t get_image_count()
    {
        return image_count;
    }
    bool get_current_file_name(char* buffer);

    /**
     * @brief Checks if there is a next file available for navigation.
     * @return True if there is a next file available, false otherwise.
     */
    bool next_available()
    {
        return index < max_index;
    }

    /**
     * @brief Checks if there is a previous file available for navigation.
     * @return True if there is a previous file available, false otherwise.
     */
    bool prev_available()
    {
        return index > 0;
    }

    /**
     * @brief Gets the current index of the image file.
     * @return The current index.
     */
    int8_t get_index()
    {
        return index;
    }

    /**
     * @brief Checks if the loop flag is enabled.
     * @return True if the loop flag is enabled, false otherwise.
     */
    bool is_looping()
    {
        return loop_on_end;
    }

private:
    File dir;             ///< The directory object.
    int8_t index;         ///< The current index of the image file.
    uint8_t max_index;    ///< The maximum index of the image file.
    char name_buffer[16]; ///< The buffer to store the file name.
    uint8_t image_count;  ///< The number of image files in the folder.
    bool loop_on_end; ///< Flag indicating whether to loop back to the first file when reaching the
                      ///< end.

    bool next_file_name(char* buffer);
    bool prev_file_name(char* buffer);
};
#endif /* IMGFOLDER_H_ */