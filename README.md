## 对于笔记本电脑的装了双系统后无法调节亮度的问题

###### 1.目前已发现是在/sys/class/backlight中存在两个文件夹，一个是nvidia_wmi_ec_backlight，另一个是nvidia_0,由于亮度条调节的是nvidia_wmi_ec_backlight，切换为独显模式的时候有nvidia_0控制系统的亮度。

2.因此我使用曲线救国路线，检测nvidia_wmi_ec_backlight的值并写入nvidia_wmi_ec_backlight里并做成开机自启动的守护程序，当nvidia_wmi_ec_backlight值发生变化时触发写入。

# 使用方法Usage:
进入install文件夹
```bash
sudo install/install.sh
```



### ps:若另一个文件名不是叫nvidia_wmi_ec_backlight，则打开main.c修改对应的文件名即可
