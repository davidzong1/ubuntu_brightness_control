/*
    * 程序功能：      监听nvidia_wmi_ec_backlight目录下的brightness文件的变化，
                    当brightness文件发生变化时，将亮度值转换为nvidia_0目录下的brightness文件的值
    * 作者：         davidzong
    * 创建时间：      2024-04-17
    * 最后修改时间：   2024-04-17
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <syslog.h>
#include <syslog.h>


#define MAX_EVENTS 1024
#define NAME_MAX 255
#define EVENT_BUF_LEN (sizeof(struct inotify_event) + NAME_MAX + 1)

const char* nvidia_wmi_ec_path = "/sys/class/backlight/nvidia_wmi_ec_backlight";
const char* nvidia_0_path = "/sys/class/backlight/nvidia_0";

/* 
    * 功能：读取nvidia_wmi_ec_backlight目录下的brightness文件，将亮度值转换为nvidia_0目录下的brightness文件的值
    * 参数：无
    * 返回值：成功返回0，失败返回-1
 */

int screen_change(void) 
{
    long actual_brightness, scaled_brightness;
    char brightness_str[255];
    openlog("davidzong_brightness_log", LOG_PID, LOG_DAEMON);
    // 获取调节亮度的文件
    FILE* brightness_file = fopen(strcat(strcpy(brightness_str, nvidia_wmi_ec_path), "/brightness"), "r");
    if (!brightness_file) {
        syslog(LOG_INFO, "fopen nvidia_wmi_ec_path failed");
        return -1;
    }
    if (fscanf(brightness_file, "%ld", &actual_brightness) != 1) {
        syslog(LOG_INFO, "read brightness failed");
        return -1;
    }
    fclose(brightness_file);
    // 调节亮度
    scaled_brightness = ((actual_brightness - 5) * 99) / (255 - 5) + 1;
    // Write scaled brightness to nvidia_0
    FILE* target_brightness_file = fopen(strcat(strcpy(brightness_str, nvidia_0_path), "/brightness"), "w");
    if (!target_brightness_file) {
        syslog(LOG_INFO, "fopen nvidia_wmi_ec_path failed");
        return -1;
    }
    if (fprintf(target_brightness_file, "%ld", scaled_brightness) < 0) {
        syslog(LOG_INFO, "write brightness failed");
        return -1;
    }
    fclose(target_brightness_file);
    return 0;
}


int main() {
    int inotify_fd, wd;
    char event_buf[EVENT_BUF_LEN];
    char *event_buf_ptr;
    struct inotify_event* event_ptr;
    ssize_t len;
    long actual_brightness, scaled_brightness;
    
    // 开机先调节第一次亮度
    if(screen_change() == -1){
        syslog(LOG_INFO, "screen_change");
    }
    // Open inotify instance
    inotify_fd = inotify_init();
    if (inotify_fd == -1) {
        syslog(LOG_INFO, "inotify_init");
        return EXIT_FAILURE;
    }

    // Add watch for brightness file changes in nvidia_wmi_ec_backlight directory
    wd = inotify_add_watch(inotify_fd, nvidia_wmi_ec_path,
                           IN_CLOSE_WRITE | IN_MODIFY);
    if (wd == -1) {
        syslog(LOG_INFO, "inotify_add_watch");
        return EXIT_FAILURE;
    }

    while (1) {
        // 阻塞等待事件
        len = read(inotify_fd, event_buf, EVENT_BUF_LEN);
        if (len > EVENT_BUF_LEN) {
            syslog(LOG_INFO, "Buffer overflow detected");
            break;
        }
        event_buf_ptr = event_buf;      //该指针指向event_buf的首地址，用以遍历event_buf，因为event_buf是const的，不能直接操作
        if (len == -1) {
            syslog(LOG_INFO, "read event failed");
            break;
        }

        // Process events
        while (len > 0) {   //event_buf中可能有多个事件，所以要循环处理
            struct inotify_event* event = (struct inotify_event*)event_buf;
            event_ptr = event;            //该指针与event_buf_ptr同理
            if ((event->mask & IN_CLOSE_WRITE) || (event->mask & IN_MODIFY)) {
                screen_change();
            }
            // 减去当前event的长度，指向下一个event，同时重复上述操作
            len -= sizeof(struct inotify_event) + event_ptr->len;
            event_buf_ptr += sizeof(struct inotify_event) + event_ptr->len;
        }
    }

    // Clean up
    inotify_rm_watch(inotify_fd, wd);
    close(inotify_fd);

    return EXIT_SUCCESS;
}