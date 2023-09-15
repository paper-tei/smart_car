/*********************************************************************************************************************
* TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC264 开源库的一部分
*
* TC264 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          zf_common_fifo
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.8.0
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/

#include "zf_common_fifo.h"

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     FIFO 头指针位移
// 参数说明     *fifo               FIFO 对象指针
// 参数说明     offset              偏移量
// 返回参数     void
// 使用示例     fifo_head_offset(fifo, 1);
// 备注信息     本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
static void fifo_head_offset (fifo_struct *fifo, uint32 offset)
{
    fifo->head += offset;
    
    while(fifo->max <= fifo->head)                                              // 如果范围超过则减缓冲区大小 直到小于最大缓冲区大小
    {
        fifo->head -= fifo->max;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     FIFO 尾指针位移
// 参数说明     *fifo               FIFO 对象指针
// 参数说明     offset              偏移量
// 返回参数     void
// 使用示例     fifo_end_offset(fifo, 1);
// 备注信息     本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
static void fifo_end_offset (fifo_struct *fifo, uint32 offset)
{
    fifo->end += offset;
    
    while(fifo->max <= fifo->end)                                               // 如果范围超过则减缓冲区大小 直到小于最大缓冲区大小
    {
        fifo->end -= fifo->max;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     FIFO 重置缓冲器
// 参数说明     *fifo               FIFO 对象指针
// 返回参数     void
// 使用示例     fifo_clear(fifo);
// 备注信息     清空当前 FIFO 对象的内存
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_clear (fifo_struct *fifo)
{
    fifo_state_enum return_state = FIFO_SUCCESS;
    do
    {
        if(FIFO_CLEAR & fifo->execution)
        {
            return_state = FIFO_CLEAR_UNDO;
            break;
        }
        fifo->execution |= FIFO_CLEAR;
        fifo->head      = 0;
        fifo->end       = 0;
        fifo->size      = fifo->max;
        switch(fifo->type)
        {
            case FIFO_DATA_8BIT:
                memset(fifo->buffer, 0, fifo->max);
                break;
            case FIFO_DATA_16BIT:
                memset(fifo->buffer, 0, fifo->max * 2);
                break;
            case FIFO_DATA_32BIT:
                memset(fifo->buffer, 0, fifo->max * 4);
                break;
        }
        fifo->execution &= ~FIFO_CLEAR;
    }while(0);
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     FIFO 查询当前数据个数
// 参数说明     *fifo               FIFO 对象指针
// 返回参数     uint32              已使用长度
// 使用示例     uint32 len = fifo_used(fifo);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint32 fifo_used (fifo_struct *fifo)
{
    return (fifo->max - fifo->size);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     向 FIFO 中写入数据
// 参数说明     *fifo               FIFO 对象指针
// 参数说明     dat                 数据
// 返回参数     fifo_state_enum     操作状态
// 使用示例     zf_log(fifo_write_element(&fifo, data) == FIFO_SUCCESS, "fifo_write_byte error");
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_write_element (fifo_struct *fifo, uint32 dat)
{
    fifo_state_enum return_state = FIFO_SUCCESS;

    do
    {
        if(FIFO_WRITE & fifo->execution)
        {
            return_state = FIFO_WRITE_UNDO;
            break;
        }
        fifo->execution |= FIFO_WRITE;

        if(1 <= fifo->size)                                                     // 剩余空间足够装下本次数据
        {
            switch(fifo->type)
            {
                case FIFO_DATA_8BIT:
                    ((uint8 *)fifo->buffer)[fifo->head] = dat & 0xFF;
                    break;
                case FIFO_DATA_16BIT:
                    ((uint16 *)fifo->buffer)[fifo->head] = dat & 0xFFFF;
                    break;
                case FIFO_DATA_32BIT:
                    ((uint32 *)fifo->buffer)[fifo->head] = dat;
                    break;
            }
            fifo_head_offset(fifo, 1);                                          // 头指针偏移
            fifo->size -= 1;                                                    // 缓冲区剩余长度减小
        }
        else
        {
            return_state = FIFO_SPACE_NO_ENOUGH;
        }
        fifo->execution &= ~FIFO_WRITE;
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     向 FIFO 中写入数据
// 参数说明     *fifo               FIFO 对象指针
// 参数说明     *dat                数据来源缓冲区指针
// 参数说明     length              需要写入的数据长度
// 返回参数     fifo_state_enum     操作状态
// 使用示例     zf_log(fifo_write_buffer(&fifo, data, 32) == FIFO_SUCCESS, "fifo_write_buffer error");
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_write_buffer (fifo_struct *fifo, void *dat, uint32 length)
{
    fifo_state_enum return_state = FIFO_SUCCESS;
    uint32 temp_length = 0;
    
    do
    {
        if(NULL == dat)
        {
            return_state = FIFO_BUFFER_NULL;
            break;
        }
        if(FIFO_WRITE & fifo->execution)
        {
            return_state = FIFO_WRITE_UNDO;
            break;
        }
        fifo->execution |= FIFO_WRITE;

        if(length <= fifo->size)                                                 // 剩余空间足够装下本次数据
        {
            temp_length = fifo->max - fifo->head;                               // 计算头指针距离缓冲区尾还有多少空间

            if(length > temp_length)                                            // 距离缓冲区尾长度不足写入数据 环形缓冲区分段操作
            {
                switch(fifo->type)
                {
                    case FIFO_DATA_8BIT:
                        memcpy(
                            &(((uint8 *)fifo->buffer)[fifo->head]),
                            dat, temp_length);                                  // 拷贝第一段数据
                        fifo_head_offset(fifo, temp_length);                    // 头指针偏移
                        memcpy(
                            &(((uint8 *)fifo->buffer)[fifo->head]),
                            &(((uint8 *)dat)[temp_length]),
                            length - temp_length);                              // 拷贝第二段数据
                        fifo_head_offset(fifo, length - temp_length);           // 头指针偏移
                        break;
                    case FIFO_DATA_16BIT:
                        memcpy(
                            &(((uint16 *)fifo->buffer)[fifo->head]),
                            dat, temp_length * 2);                              // 拷贝第一段数据
                        fifo_head_offset(fifo, temp_length);                    // 头指针偏移
                        memcpy(
                            &(((uint16 *)fifo->buffer)[fifo->head]),
                            &(((uint16 *)dat)[temp_length]),
                            (length - temp_length) * 2);                        // 拷贝第二段数据
                        fifo_head_offset(fifo, length - temp_length);           // 头指针偏移
                        break;
                    case FIFO_DATA_32BIT:
                        memcpy(
                            &(((uint32 *)fifo->buffer)[fifo->head]),
                            dat, temp_length * 4);                              // 拷贝第一段数据
                        fifo_head_offset(fifo, temp_length);                    // 头指针偏移
                        memcpy(
                            &(((uint32 *)fifo->buffer)[fifo->head]),
                            &(((uint32 *)dat)[temp_length]),
                            (length - temp_length) * 4);                        // 拷贝第二段数据
                        fifo_head_offset(fifo, length - temp_length);           // 头指针偏移
                        break;
                }
            }
            else
            {
                switch(fifo->type)
                {
                    case FIFO_DATA_8BIT:
                        memcpy(
                            &(((uint8 *)fifo->buffer)[fifo->head]),
                            dat, length);                                       // 一次完整写入
                        fifo_head_offset(fifo, length);                         // 头指针偏移
                        break;
                    case FIFO_DATA_16BIT:
                        memcpy(
                            &(((uint16 *)fifo->buffer)[fifo->head]),
                            dat, length * 2);                                   // 一次完整写入
                        fifo_head_offset(fifo, length);                         // 头指针偏移
                        break;
                    case FIFO_DATA_32BIT:
                        memcpy(
                            &(((uint32 *)fifo->buffer)[fifo->head]),
                            dat, length * 4);                                   // 一次完整写入
                        fifo_head_offset(fifo, length);                         // 头指针偏移
                        break;
                }
//                memcpy(&fifo->buffer[fifo->head], dat, length);                 // 一次完整写入
//                fifo_head_offset(fifo, length);                                 // 头指针偏移
            }

            fifo->size -= length;                                               // 缓冲区剩余长度减小
        }
        else
        {
            return_state = FIFO_SPACE_NO_ENOUGH;
        }
        fifo->execution &= ~FIFO_WRITE;
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     从 FIFO 读取数据
// 参数说明     *fifo               FIFO 对象指针
// 参数说明     *dat                目标缓冲区指针
// 参数说明     flag                是否变更 FIFO 状态 可选择是否清空读取的数据
// 返回参数     fifo_state_enum     操作状态
// 使用示例     zf_log(fifo_read_element(&fifo, data, FIFO_READ_ONLY) == FIFO_SUCCESS, "fifo_read_byte error");
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_read_element (fifo_struct *fifo, void *dat, fifo_operation_enum flag)
{
    fifo_state_enum return_state = FIFO_SUCCESS;

    do
    {
        if(NULL == dat)
        {
            return_state = FIFO_BUFFER_NULL;
            break;
        }
        fifo->execution |= FIFO_READ;

        if(1 > fifo_used(fifo))
        {
            return_state = FIFO_DATA_NO_ENOUGH;                                 // 标志数据不够
        }

        switch(fifo->type)
        {
            case FIFO_DATA_8BIT:
                *((uint8 *)dat) = ((uint8 *)fifo->buffer)[fifo->end];
                break;
            case FIFO_DATA_16BIT:
                *((uint16 *)dat) = ((uint16 *)fifo->buffer)[fifo->end];
                break;
            case FIFO_DATA_32BIT:
                *((uint32 *)dat) = ((uint32 *)fifo->buffer)[fifo->end];
                break;
        }

        if(flag == FIFO_READ_AND_CLEAN)                                         // 如果选择读取并更改 FIFO 状态
        {
            if(FIFO_CLEAR & fifo->execution)
            {
                return_state = FIFO_CLEAR_UNDO;
                break;
            }
            fifo->execution |= FIFO_CLEAR;
            fifo_end_offset(fifo, 1);                                           // 移动 FIFO 头指针
            fifo->size += 1;
            fifo->execution &= ~FIFO_CLEAR;
        }
    }while(0);
    fifo->execution &= FIFO_READ;

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     从 FIFO 读取数据
// 参数说明     *fifo               FIFO 对象指针
// 参数说明     *dat                目标缓冲区指针
// 参数说明     *length             读取的数据长度 如果没有这么多数据这里会被修改
// 参数说明     flag                是否变更 FIFO 状态 可选择是否清空读取的数据
// 返回参数     fifo_state_enum     操作状态
// 使用示例     zf_log(fifo_read_buffer(&fifo, data, &length, FIFO_READ_ONLY) == FIFO_SUCCESS, "fifo_read_buffer error");
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_read_buffer (fifo_struct *fifo, void *dat, uint32 *length, fifo_operation_enum flag)
{
    fifo_state_enum return_state = FIFO_SUCCESS;
    uint32 temp_length;

    do
    {
        if(NULL == dat)
        {
            return_state = FIFO_BUFFER_NULL;
            break;
        }
        fifo->execution |= FIFO_READ;

        if(*length > fifo_used(fifo))
        {
            *length = (fifo->max - fifo->size);                                 // 纠正读取的长度
            return_state = FIFO_DATA_NO_ENOUGH;                                 // 标志数据不够
        }

        temp_length = fifo->max - fifo->end;                                    // 计算尾指针距离缓冲区尾还有多少空间
        if(*length <= temp_length)                                              // 足够一次性读取完毕
        {
            switch(fifo->type)
            {
                case FIFO_DATA_8BIT:
                    memcpy(dat, &(((uint8 *)fifo->buffer)[fifo->end]), *length);
                    break;
                case FIFO_DATA_16BIT:
                    memcpy(dat, &(((uint16 *)fifo->buffer)[fifo->end]), *length * 2);
                    break;
                case FIFO_DATA_32BIT:
                    memcpy(dat, &(((uint32 *)fifo->buffer)[fifo->end]), *length * 4);
                    break;
            }
        }
        else
        {
            switch(fifo->type)
            {
                case FIFO_DATA_8BIT:
                    memcpy(dat, &(((uint8 *)fifo->buffer)[fifo->end]), temp_length);
                    memcpy(&(((uint8 *)dat)[temp_length]), fifo->buffer, *length - temp_length);
                    break;
                case FIFO_DATA_16BIT:
                    memcpy(dat, &(((uint16 *)fifo->buffer)[fifo->end]), temp_length * 2);
                    memcpy(&(((uint16 *)dat)[temp_length]), fifo->buffer, (*length - temp_length) * 2);
                    break;
                case FIFO_DATA_32BIT:
                    memcpy(dat, &(((uint32 *)fifo->buffer)[fifo->end]), temp_length * 4);
                    memcpy(&(((uint32 *)dat)[temp_length]), fifo->buffer, (*length - temp_length) * 4);
                    break;
            }
        }

        if(flag == FIFO_READ_AND_CLEAN)                                         // 如果选择读取并更改 FIFO 状态
        {
            if(FIFO_CLEAR & fifo->execution)
            {
                return_state = FIFO_CLEAR_UNDO;
                break;
            }
            fifo->execution |= FIFO_CLEAR;
            fifo_end_offset(fifo, *length);                                     // 移动 FIFO 头指针
            fifo->size += *length;
            fifo->execution &= ~FIFO_CLEAR;
        }
    }while(0);
    fifo->execution &= FIFO_READ;

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     从 FIFO 尾部读取指定长度 buffer
// 参数说明     *fifo               FIFO 对象指针
// 参数说明     *dat                目标缓冲区指针
// 参数说明     *length             读取的数据长度 如果没有这么多数据这里会被修改
// 参数说明     flag                是否变更 FIFO 状态 可选择是否清空读取的数据
// 返回参数     fifo_state_enum     操作状态
// 使用示例     zf_log(fifo_read_tail_buffer(&fifo, data, &length, FIFO_READ_ONLY) == FIFO_SUCCESS, "fifo_read_buffer error");
// 备注信息     如果使用 FIFO_READ_AND_CLEAN 操作 将会丢弃所有数据并清空整个 FIFO
//              如果使用 FIFO_READ_AND_CLEAN 操作 将会丢弃所有数据并清空整个 FIFO
//              如果使用 FIFO_READ_AND_CLEAN 操作 将会丢弃所有数据并清空整个 FIFO
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_read_tail_buffer (fifo_struct *fifo, void *dat, uint32 *length, fifo_operation_enum flag)
{
    fifo_state_enum return_state = FIFO_SUCCESS;
    uint32 temp_length;

    do
    {
        if(NULL == dat)
        {
            return_state = FIFO_BUFFER_NULL;
            break;
        }
        fifo->execution |= FIFO_READ;

        if(*length > fifo_used(fifo))
        {
            *length = (fifo->max - fifo->size);                                 // 纠正读取的长度
            return_state = FIFO_DATA_NO_ENOUGH;                                 // 标志数据不够
        }

        if((fifo->head > fifo->end) || (fifo->head >= *length))
        {
            switch(fifo->type)
            {
                case FIFO_DATA_8BIT:
                    memcpy(dat, &(((uint8 *)fifo->buffer)[fifo->head - *length]), *length);
                    break;
                case FIFO_DATA_16BIT:
                    memcpy(dat, &(((uint16 *)fifo->buffer)[fifo->head - *length]), *length * 2);
                    break;
                case FIFO_DATA_32BIT:
                    memcpy(dat, &(((uint32 *)fifo->buffer)[fifo->head - *length]), *length * 4);
                    break;
            }
        }
        else
        {
            temp_length = *length - fifo->head;                                 // 计算尾指针距离缓冲区尾还有多少空间
            switch(fifo->type)
            {
                case FIFO_DATA_8BIT:
                    memcpy(dat, &(((uint8 *)fifo->buffer)[fifo->max - temp_length]), temp_length);
                    memcpy(&(((uint8 *)dat)[temp_length]), &(((uint8 *)fifo->buffer)[fifo->head - *length]), (*length - temp_length));
                    break;
                case FIFO_DATA_16BIT:
                    memcpy(dat, &(((uint16 *)fifo->buffer)[fifo->max - temp_length]), temp_length * 2);
                    memcpy(&(((uint16 *)dat)[temp_length]), &(((uint16 *)fifo->buffer)[fifo->head - *length]), (*length - temp_length) * 2);
                    break;
                case FIFO_DATA_32BIT:
                    memcpy(dat, &(((uint32 *)fifo->buffer)[fifo->max - temp_length]), temp_length * 4);
                    memcpy(&(((uint32 *)dat)[temp_length]), &(((uint32 *)fifo->buffer)[fifo->head - *length]), (*length - temp_length) * 4);
                    break;
            }
        }

        if(flag == FIFO_READ_AND_CLEAN)                                         // 如果选择读取并更改 FIFO 状态
        {
            if(FIFO_CLEAR & fifo->execution)
            {
                return_state = FIFO_CLEAR_UNDO;
                break;
            }
            fifo->execution |= FIFO_CLEAR;
            fifo_end_offset(fifo, (fifo->max - fifo->size));
            fifo->size = fifo->max;
            fifo->execution &= ~FIFO_CLEAR;
        }
    }while(0);
    fifo->execution &= FIFO_READ;

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     FIFO 初始化 挂载对应缓冲区
// 参数说明     *fifo               FIFO 对象指针
// 参数说明     type                FIFO 数据位数
// 参数说明     *buffer_addr        要挂载的缓冲区
// 参数说明     size                缓冲区大小
// 返回参数     fifo_state_enum     操作状态
// 使用示例     fifo_init(&user_fifo, user_buffer, 64);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
fifo_state_enum fifo_init (fifo_struct *fifo, fifo_data_type_enum type, void *buffer_addr, uint32 size)
{
    fifo_state_enum return_value = FIFO_SUCCESS;
    do
    {
        if(NULL == buffer_addr)
        {
            return_value = FIFO_BUFFER_NULL;
            break;
        }
        fifo->buffer            = buffer_addr;
        fifo->execution         = FIFO_IDLE;
        fifo->type              = type;
        fifo->head              = 0;
        fifo->end               = 0;
        fifo->size              = size;
        fifo->max               = size;
    }while(0);
    return return_value;
}
