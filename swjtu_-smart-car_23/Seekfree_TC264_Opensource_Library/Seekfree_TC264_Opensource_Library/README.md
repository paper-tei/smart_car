# 欢迎

## 介绍

    本项目为 西南交通大学 2023年 大学生智能汽车竞赛 独轮车组 “听党指挥，独轮不倒”队 开源项目

## 怎么看代码?
    从main函数的循环体开始往下读,大部分图像处理放在这里的
    同时查看定时器中断服务函数里面的逻辑,大多数控制算法是放在这里的
    不要从code直接看函数,有相当大一部分函数是没掉用或者被废弃了的
    善用VSCODE等现代编辑器查看编写代码
    善用copilot帮助理解和编写代码


## 项目目录
0. [里程计导航算法](导航算法.md) [独轮控制思路入门](独轮控制思路入门.md)
1. **code** 各模块代码
    - Adc_Em 电磁相关模数转换
    - banlance 车辆平衡相关
    - current 电流读取(废弃)
    - debug 屏幕显示，调参通讯等debug用
    - element 上交巡线元素判断(废弃)
    - encoder 编码器读取用
    - error 摄像头寻线中线偏差值用
    - filtering 滤波用
    - image 二值化寻线
    - imu 陀螺仪相关
    - In_out_bound 里程计导航
    - In_perspective 逆透视变换
    - init 初始化
    - key 按键
    - motor 电机驱动
    - pico_GY 图传
    - pic_link_ii 图传
    - pid pid算法
    - protocol 调参用
    - ring 元素判断
    - time 定时器用
    - tracing 上交巡线迷宫走线(废弃)
2. **libraries** 英飞凌逐飞库
3. **user** 主函数

