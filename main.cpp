#include <PhotoAlbum.h>
#include <util/delay.h>

// void handle_error()
// {
//     switch (disk.get_error())
//     {
//         case SDCard::Error::CMD0:
//             printf("timeout error for command CMD0\n");
//             break;
//         case SDCard::Error::CMD8:
//             printf("CMD8 was not accepted - not a valid SD card\n");
//             break;
//         case SDCard::Error::ACMD41:
//             printf("card's ACMD41 initialization process timeout\n");
//             break;
//         case SDCard::Error::CMD58:
//             printf("card returned an error response for CMD58 (read OCR)\n");
//             break;
//         case SDCard::Error::CMD24:
//             printf("card returned an error response for CMD24 (write block)\n");
//             break;
//         default:
//             printf("Unknown error. Code %x\n", (uint8_t) disk.get_error());
//             break;
//     }
// }

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