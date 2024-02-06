#ifndef IMGFOLDER_H_
#define IMGFOLDER_H_

#include <File.h>

class ImgFolder
{
	public:
	ImgFolder(File& dir) : dir(dir), index(0), max_index(127)
	{
		memset(name_buffer, 0, 16);
	}
	bool first_file(File &imgFile) {
    index = index - 1;
		return next_file(imgFile);
	}

	bool next_file(File& imgFile)
	{
		memset(name_buffer, 0, 16);
		if (!next_file_name(name_buffer))
		{
			return false;
		}
		return imgFile.open(dir, name_buffer, File::O_RDONLY);
	}
	bool prev_file(File& imgFile)
	{
		memset(name_buffer, 0, 16);
		if (!prev_file_name(name_buffer))
		{
			return false;
		}
		return imgFile.open(dir, name_buffer, File::O_RDONLY);
	}

	private:
	File dir;
	int8_t index;
	uint8_t max_index;
	char name_buffer[16];

	bool next_file_name(char* buffer)
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
	bool prev_file_name(char* buffer)
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
};
#endif /* IMGFOLDER_H_ */