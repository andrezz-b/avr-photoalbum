
/**
 * @file main.cpp
 * @brief Program entrypoint.
 */

#include <PhotoAlbum.h>
#include <util/delay.h>

/**
 * @brief The main function of the program.
 *
 * @details This function initializes the PhotoAlbum object, listens for user input in a while loop.
 *
 * @return int The exit status of the program.
 */
int main()
{
    PhotoAlbum photoAlbum;
    photoAlbum.init();
    while (1)
    {
        photoAlbum.listen_for_input();
        _delay_ms(50);
    }

    return 0;
}