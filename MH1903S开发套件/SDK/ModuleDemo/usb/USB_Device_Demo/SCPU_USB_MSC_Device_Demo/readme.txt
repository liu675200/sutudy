1> 该Demo程序实现的是一个MassStorage设备功能；
2> 实现操作说明：
   a> 将目标板通过串口和PC连接(查看打印)
   b> 使用USB线(一头Mini B型插头，一头A型插头)将板子与电脑连接，在设备管理器能看到多了一个USB Mass Storage Device，进入“我的电脑”会看到多出一个盘符(可移动磁盘)

备注：
   a> 修改其他端点时只需要修改usbd_conf.h中的MSC_IN_EP(0x81~0x87)与MSC_OUT_EP(0x01~0x07)，同时对应的usb_conf.h中的TXn_FIFO_FS_SIZE要对应修改为64
   b> 如果只是修改usbd_conf.h中到端点号而对应FIFO仍为0，则插入电脑检测到一个MassStorage设备插入，但是不能找到相应的驱动，"我的电脑"下也不会多一个盘符
