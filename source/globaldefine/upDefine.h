#ifndef UPDEFINE_H
#define UPDEFINE_H

/*Servo type configuration*/
#define     Digital_UBT12HB             0x01 //UBT12HB 数字舵机
#define     Simulate_LD1602             0x02 //LD1602 模拟舵机

#define     Digital_DisAbled            0x00
#define     Digital_Enabled             0x01

#define     DEFAULT_CSV_PATH            QDir::currentPath()+"/KeyFrame"
#define     DEFAULT_BIN_PATH            QDir::currentPath()+"/MiniAction"
#define     DEFAULT_ODR_PATH            QDir::currentPath()+"/Sequence"
#define     DEFAULT_ARQ_WAIT            10
#define     DEFAULT_DBG_MODEL           0
#define     DEFAULT_MSG_LEN             200
#define     DEFAULT_IS_MSG_LIMITED      false
#define     DEFAULT_INTERPPLATE_TIME    20
#define     DEFAULT_IS_ITP_LIMITED      false
#define     DEFAULT_TAB_DRC             2
#define     DEFAULT_LOG_FORMAT          10

#define     MODEL_CONNECTED             0
#define     MODEL_FILE                  1
#define     MODEL_CURRENT               2

#endif // UPDEFINE_H
