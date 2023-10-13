// study_vlc.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <Windows.h>
#include <iostream>
#include <conio.h>
#include "vlc.h"

int main()
{
    int arg = 1;
    char* argv[2];
    argv[0] = (char*)"--ignore-config";
    libvlc_instance_t* vlc_ins = libvlc_new(arg, argv);
    if (vlc_ins == NULL) {
        printf("-------------------------------------");
    }
    libvlc_media_t* media = libvlc_media_new_path(vlc_ins, "D:\\edoyun\\study_vlc\\study_vlc\\study_vlc\\tyson_fury.mp4");
    if (media == NULL) {
        printf("((((((((((((((((((((((((((((((((((((((((");
    }

    libvlc_media_player_t* player = libvlc_media_player_new_from_media(media);
    if (player == NULL) {
        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    }


    do {
        int ret = libvlc_media_player_play(player);
        if (ret == -1)
        {
            printf("error found\r\n");
            break;
        }
        Sleep(300);
        int vol = libvlc_audio_get_volume(player);
        printf("volumn is %d\r\n", vol);

        libvlc_audio_set_volume(player, 30);
        vol = libvlc_audio_get_volume(player);
        printf("volumn is %d\r\n", vol);

        libvlc_time_t tm = libvlc_media_player_get_length(player);

        printf("***************************************%02d:%02d:%02d\r\n", int(tm / 3600000), int(tm / 60000) % 60, int(tm / 1000) % 60, int(tm) % 1000);

        int width = libvlc_video_get_width(player);
        int height = libvlc_video_get_height(player);

        printf("width = %d, height = %d\r\n", width, height);

        while (!_kbhit())
        {
            printf("%f%%\r", 100 * libvlc_media_player_get_position(player));
            Sleep(500);
        }
        getchar();
        libvlc_media_player_pause(player);

        getchar();

        libvlc_media_player_play(player);
        getchar();
        libvlc_media_player_stop(player);

    } while (0);

    libvlc_media_player_release(player);
    libvlc_media_release(media);
    libvlc_release(vlc_ins);
    return 0;
}

