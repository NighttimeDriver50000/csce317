// LIS3DH.h -- Register mapping for the LIS3DH accelerometer.
// Warning: Do not write to reserved addresses. According to the data sheet,
//   this can potentially harm or destroy the device.

#ifndef LIS3DH_H
#define LIS3DH_H

// ---------------------------------------------------------------------------
// LIS3DSH Register mapping.
//      | Register Name     | Hex | Type | Purpose

// Reserved (do not modify) | 0x00 - 0x06

#define STATUS_REG_AUX      0x07  // r   | Info on data availability and overruns
#define OUT_ADC1_L          0x08  // r   | ADC1 output register
#define OUT_ADC1_H          0x09  // r   | ADC1 output register
#define OUT_ADC2_L          0x0A  // r   | ADC2 output register
#define OUT_ADC2_H          0x0B  // r   | ADC2 output register
#define OUT_ADC3_L          0x0C  // r   | ADC3 output register
#define OUT_ADC3_H          0x0D  // r   | ADC3 output register
#define INT_COUNTER_REG     0x0E  // r   | ...
#define WHO_AM_I            0x0F  // r   | I2C ID (Dummy register on this model)

// Reserved (do not modify) | 0x10 - 0x1E

#define TEMP_CFG_REG        0x1F  // r/w | ADC/Temperature sensor enable
#define CTRL_REG1           0x20  // r/w | Data rate selection / axis enables
#define CTRL_REG2           0x21  // r/w | Filter controls (high/low pass enables)
#define CTRL_REG3           0x22  // r/w | INT1 interrupt config
#define CTRL_REG4           0x23  // r/w | Data update controls / scale selection
#define CTRL_REG5           0x24  // r/w | Reboot / FIFO enable / other enables
#define CTRL_REG6           0x25  // r/w | CLICK config
#define REFERENCE           0x26  // r/w | Reference value for interrupt generation
#define STATUS_REG2         0x27  // r   | 3-axis data available / data overruns
#define OUT_X_L             0x28  // r   | X-axis value (low 8 bits)
#define OUT_X_H             0x29  // r   | X-axis value (high 8 bits)
#define OUT_Y_L             0x2A  // r   | Y-axis value (low 8 bits)
#define OUT_Y_H             0x2B  // r   | Y-axis value (high 8 bits)
#define OUT_Z_L             0x2C  // r   | Z-axis value (low 8 bits)
#define OUT_Z_H             0x2D  // r   | Z-axis value (high 8 bits)
#define FIFO_CTRL_REG       0x2E  // r/w | FIFO registers
#define FIFO_SRC_REG        0x2F  // r   | FIFO event sources
#define INT1_CFG            0x30  // r/w | 3-axis interrupt config
#define INT1_SOURCE         0x31  // r   | 3-axis interrupt sources (low/high triggers)
#define INT1_THS            0x31  // r/w | Threshold value
#define INT1_DURATION       0x32  // r/w | Minimum event duration to recognize (in steps)

// Rserved (do not modify) | 0x34 - 0x37

#define CLICK_CFG           0x38  // r/w | CLICK interrupt config
#define CLICK_SRC           0x39  // r   | CLICK interrupt sources (single/double/XYZ-axis clicks)
#define CLICK_THS           0x3A  // r/w | CLICK-CLICK threshold value
#define TIME_LIMIT          0x3A  // r/w | CLICK-CLICK time limit
#define TIME_LATENCY        0x3A  // r/w | CLICK-CLICK time latency
#define TIME_WINDOW         0x3A  // r/w | CLICK-CLICK time window

#endif // #ifndef LIS3DH_H
