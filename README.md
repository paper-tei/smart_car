# 0.前言

各位好，我是SWJTU今年智能车队的队长paper，从大一开始算起，参加智能车竞赛已经有三年了，从第一年的全向行进，到去年的四轮摄像头，再到今年的独轮。虽说期间陆陆续续打了很多其他的比赛。但是智能车还是贯穿了我的整个大学竞赛生涯。在今年西部赛区中，或许是打了这么久智能车积攒的好运一下爆发开了，得到了决赛第三的好成绩，坦言说，实在是受之有愧。我们独轮车的基础属性其实属于是t2梯队，板子复杂，机械臃肿，调参时间不够，匆匆忙忙地就前往了赛场。今年我实在是太忙了，同时参加了蓝桥杯、电赛、RM、智能车这些比赛，再加上大三课程很多，同时我还得去准备企业的面试，能分给独轮车的时间的确很少很少。在这里我先感谢我的两个队友，我们一起在最后的几周内两班倒才成功的完成了整车的上场准备工作。 

在七月中旬对比赛规则的再次研读中，我确立了**惯导**对独轮车组别的重要性，如果我们可以出断路横跨赛道的话，无疑是直接回车库是最优路径。怀抱着这样的想法，我进行了简单的测试，并且向去年取得了智能视觉组全国季军的张斌学长、RM实验室负责哨兵导航的李逸凡同学咨询了一些关于惯导方面基础性的知识，便开始了代码编写，幸运的在前往重庆之前完成了惯导回库的整车验证。

在备赛中我和多位车友进行了技术上的交流，这些坦诚的交流让我越发确认了**交流**对于技术进步的重要性。国赛在即，我可能做出了一个很多人不太能理解的决定，即**开源**惯导算法的思路细节。希望以此为契机，引起大家的一次技术讨论，优化我们各自的算法，在国赛中跑出更优秀的成绩，为大家带来一场视觉盛宴。

因为毕竟国赛还没进行，考虑到智能车车友的平均代码水平，我决定以局部代码+思路讲解的形式开源，在国赛进行再后开源整车工程的代码与硬件，且因时间紧张，一些简单的过程进行略过处理，国赛后进行补充。若有关于算法细节上的疑问，欢迎直接在*英飞凌独轮车组技术支持交流群--逐飞科技（群号983308060）*内直接艾特我，而非进行私聊的形式进行交流。

另外，毕竟是四五天内写出来的东西，很多地方处理都不是最优解，欢迎大家指正。

最后，借用RM的一句话警醒正在打比赛的自己与各位。

***<u>初心高于成败</u>***



# 1.数据处理

我们组所采用的传感器为逐飞科技提供的**IMU660RA**。

姿态角度解算的算法参考自

> [GitHub - SJTU-AuTop/RT1064-Code: RT1064主控代码 ](https://github.com/SJTU-AuTop/RT1064-Code) 

对数据的获取进行了滤波和减去零飘的处理。

```c
void ICM_getValues() {
    // 一阶低通滤波，单位g/s
    icm_data_acc_x = (((float)imu660ra_acc_x) * alpha) + icm_data_acc_x * (1 - alpha);
    icm_data_acc_y = (((float)imu660ra_acc_y) * alpha) + icm_data_acc_y * (1 - alpha);
    icm_data_acc_z = (((float)imu660ra_acc_z) * alpha) + icm_data_acc_z * (1 - alpha);
    // 陀螺仪角速度转弧度
    icm_data_gyro_x = ((float)imu660ra_gyro_x - GyroOffset_Xdata) * M_PI / 180 / 16.4f;
    icm_data_gyro_y = ((float)imu660ra_gyro_y - GyroOffset_Ydata) * M_PI / 180 / 16.4f;
    icm_data_gyro_z = ((float)imu660ra_gyro_z - GyroOffset_Zdata) * M_PI / 180 / 16.4f;
}
```

`GyroOffset`的获取很简单，取车辆静止不动的一段时间内的数据平均值即可。

角度解算的数据获取中断周期为2MS。

速度编码器的数据获取中断周期为20MS。

通过四元数最后的到了车辆的偏航角。

# 2.车辆自身坐标获取

我们假设有两个按键，一个按键决定了车辆当前坐标系，一个按键决定了发车与否。

那我们将车放在车库中心，按下第一个按键，调用以下函数

```c
void Inertial_Navigation_Start(void) {
    //设置当前坐标,抓取当前坐标系为基准坐标系
    navigation.start_yaw = eulerAngle_yaw_total;
    navigation.x_cur = 0;
    navigation.y_cur = 0;
    navigation.x_start = 0 * 1; //mm
    navigation.y_start = 0 * 1; //mm
    navigation.x_set = 0 * 1;   //mm
    navigation.y_set = 0 * 1;   //mm
}
```

即可获得一个参考二维坐标系，这个坐标系以车库为中心，出车库右侧为X轴正方向，车辆前方为Y轴正方向。我们在中断中进行以下计算，周期为2MS。

```c
navigation.cur_yaw = eulerAngle_yaw_total - navigation.start_yaw;
navigation.x_cur = navigation.x_cur + sin(navigation.cur_yaw / 180 * 3.14159f) * (encoder_bdc / 150 * 50 * 0.002);
navigation.y_cur = navigation.y_cur + cos(navigation.cur_yaw / 180 * 3.14159f) * (encoder_bdc / 150 * 50 * 0.002);
```

我们车轮子行进一米大概会让编码器产生150000个脉冲，以20ms为编码器速度获取周期进行换算，得到的`x_cur`,`y_cur`即为车辆的坐标。

`eulerAngle_yaw_total`为yaw轴偏航累计角度，因为直接读取姿态解算的yaw值的话，它会有-180~180的限制，且在边界会出现跳变，故采用以下代码进行`eulerAngle_yaw_total`值的获取。

```c
float i = eulerAngle_yaw - eulerAngle_yaw_old;
if (i < -180) {
    i += 360;
}
else if (i > 180) {
    i -= 360;
}
eulerAngle_yaw_total += i;
eulerAngle_yaw_old = eulerAngle_yaw;
```

# 3.车库（目标点）方向计算

这部分思路很简单，既然得到了车辆自身的坐标，那么只需要进行atan计算即可获取车辆所需的偏航角。但是需要注意的是atan的返回值在-45~45°之间，需要我们针对车辆坐标系进行判断后才能知道准确的-180~180之间的偏航角。

```c
if (navigation.y_set - navigation.y_cur < 0 && navigation.x_set - navigation.x_cur > 0) {
    navigation.inertial_navigation_set_yaw_update = atan((navigation.x_set - navigation.x_cur) / (navigation.y_set - navigation.y_cur)) / 3.14159f * 180 + 180;
}
else if (navigation.y_set - navigation.y_cur < 0 && navigation.x_set - navigation.x_cur < 0) {
    navigation.inertial_navigation_set_yaw_update = atan((navigation.x_set - navigation.x_cur) / (navigation.y_set - navigation.y_cur)) / 3.14159f * 180 - 180;
}
else {
    navigation.inertial_navigation_set_yaw_update = atan((navigation.x_set - navigation.x_cur) / (navigation.y_set - navigation.y_cur)) / 3.14159f * 180;
}
```

获取到```navigation.inertial_navigation_set_yaw_update```后进行类似偏航累计的处理

```c
float i = navigation.inertial_navigation_set_yaw_update - navigation.inertial_navigation_set_yaw_old;
while (i > 180) {
    i = i - 360;
    if (i < 180) {
        break;
    }
}
while (i < -180) {
    i = i + 360;
    if (i > -180) {
        break;
    }
}
```

至此我们得到了目标偏航角的累加。我们再取一次余。

```c
if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw > 360) {
    while (1) {
        navigation.inertial_navigation_set_yaw_up_total -= 360;
        if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw < 360) {
            break;
        }
    }
}
else if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw < -360) {
    while (1) {
        navigation.inertial_navigation_set_yaw_up_total += 360;
        if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw > -360) {
            break;
        }
    }
}
```

然后我们车辆在惯导回车库的时候，实际上不需要车头朝向车库，旋转最近的旋转角度，使车头或者车尾朝向车库即可。

```c
if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw < 90 && navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw > -90) {
    speed_reversal_flag = 0;
}
if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw > 90) {
    while (1) {
        //
        navigation.inertial_navigation_set_yaw_up_total -= 180;
        if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw < 90) {
            speed_reversal_flag = 1;
            break;
        }
    }
}
if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw < -90) {
    while (1) {
        navigation.inertial_navigation_set_yaw_up_total += 180;
        if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw > -90) {
            speed_reversal_flag = 1;
            break;
        }
    }
}
```

在speed_reversal_flag == 1时，我们在行进轮速度控制的时候使车辆倒车即可。

# 4.回库流程

在出断路后，我们将pid计算中的角度环的偏差替换为`navigation.cur_yaw - navigation.inertial_navigation_set_yaw`,其中`navigation.inertial_navigation_set_yaw`为经过了斜坡函数获得的值。

```c
if (navigation.inertial_navigation_set_yaw < navigation.inertial_navigation_set_yaw_up_total) {
    navigation.inertial_navigation_set_yaw += 0.04f;
}
else if (navigation.inertial_navigation_set_yaw > navigation.inertial_navigation_set_yaw_up_total) {
    navigation.inertial_navigation_set_yaw -= 0.04f;
}
```

我们车辆基础性能有限，只能进行了这样的处理，希望大家可以多多交流控制方面的思路，互相进步。



在回库过程中，我们只需要设置目标值`navigation.x_set`与`navigation.y_set`即可让车回到设定点位。  

我们组先将车开回（0，0），再设置到（0，x），x值为一个常量，为计时器的触发地点，随后再将目标值设定为（0，0），车辆到达后，进行关闭输出处理。

需要注意的是，因为无法精确地到达目标点，我们可以在到达目标点半径一定范围后就将状态机切换为下一个状态。

# 5.后续

因为时间不足，第一版的思路开源大概就写到这里了，希望各位能给个star，下个月打完比赛找好工作之后将进行代码的开源和思路讲解更新。

最后重复一遍，若有关于算法细节上的疑问，欢迎直接在*英飞凌独轮车组技术支持交流群--逐飞科技（群号983308060）*内直接艾特我，而非进行私聊的形式进行交流，我希望我们谈论的东西能被大家都看到，这才是我开源的意义。

