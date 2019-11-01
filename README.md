# spresense-cs2200ctrl

# Cirrus Logic CS2200CP controller for SPRESENSE




This program controls [CS2200CP](https://www.cirrus.com/products/cs2200/) connected by I2C bus from Spresense. All CS2200CP registers can be displayed and written.

Usage

```
nsh> cs2200ctrl ?
Usage: cs2200ctrl <operation> <register name> <slave id> [args...]
Ex.: cs2200ctrl r device_id 4e
Operations:
        ? : help
        r : read
        w : write
Registers:
        all : Read all registers [R only]
        device_id : Device I.D. and Revision (Address 01h) [R only]
        device_control : Device Control (Address 02h) [R/W]
        device_configuration1 : Device Configuration 1 (Address 03h) [R/W]
        global_configuration : Global Configuration (Address 05h) [R/W]
        ratio : Ratio (Address 06h - 09h) [R/W]
        frequency : Frequency (Address 06h - 09h) [W only]
        function_configuration1 : Function Configuration 1 (Address 16h) [R/W]
        function_configuration2 : Function Configuration 2 (Address 17h) [R/W]
```
Write register
```
nsh> cs2200ctrl w function_configuration1 4e divide1 push_pull
Function Configuration 1 (Address 16h)
Succeeded.
```
Read register
```
nsh> cs2200ctrl r function_configuration1 4e
Function Configuration 1 (Address 16h)
        AUX PLL Lock Output Configuration (AuxLockCfg) : (00) Push-Pull. Active High.
        Reference Clock Input Divider (RefClkDiv[1:0]) : (02) 1/1 8 MHz to 14 MHz.
Succeeded.
```
Read all registers
```
nsh> cs2200ctrl r all 4e
Read all registers
Device I.D. and Revision (Address 01h)
        Device Identification (Device[4:0]) : (00) CS2200.
        Device Revision (Revision[2:0]) : (06) C1.
Device Control (Address 02h)
        Unlock Indicator (Unlock) : (00) PLL is Locked.
        Auxiliary Output Disable (AuxOutDis) : (00) AUX_OUT output driver enabled.
        PLL Clock Output Disable (ClkOutDis) : (00) CLK_OUT output driver enabled.
Device Configuration 1 (Address 03h)
        R-Mod Selection (RModSel[2:0]) : (00) Left-shift R-value by 0 (x 1).
        Auxiliary Output Source Selection (AuxOutSrc[1:0]) : (02) CLK_OUT.
        Enable Device Configuration Register 1 (EnDevCfg1) : (01) Enabled.
Global Configuration (Address 05h)
        Device Configuration Freeze (Freeze) : (00) Register changes take effect immediately.
        Enable Device Configuration Register 2 (EnDevCfg2) : (01) Enabled.
Ratio (Address 06h - 09h)
        Ratio : 2097152 (0x00200000)
Function Configuration 1 (Address 16h)
        AUX PLL Lock Output Configuration (AuxLockCfg) : (00) Push-Pull. Active High.
        Reference Clock Input Divider (RefClkDiv[1:0]) : (02) 1/1 8 MHz to 14 MHz.
Function Configuration 2 (Address 17h)
        Enable PLL Clock Output on Unlock (ClkOutUnl) : (00) Clock outputs are driven "low" when PLL is unlocked.
Succeeded.
```
Example (Change the output frequency to 20MHz with the 10MHz clock oscillator connected.)
```
cs2200ctrl w device_configuration1 4e left0 clk_out enable
cs2200ctrl w frequency 4e 10000000 20000000
cs2200ctrl w function_configuration1 4e divide1 push_pull
cs2200ctrl w function_configuration2 4e clk_out_low
cs2200ctrl w global_configuration 4e enable
```

The following CONFIG must be enabled.

- Kernel
  - CONFIG_I2C
  - CONFIG_I2C_DRIVER
- SDK
  - CONFIG_CXD56_I2C
  - CONFIG_CXD56_I2C0
  - CONFIG_SYSTEM_I2CTOOL
  - CONFIG_NSH_MAXARGUMENTS = 7 (or more)

