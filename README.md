# spresense-cs2200ctrl

# CIRRUS LOGIC CS2200CP controller for SPRESENSE

This program controls CS2200CP connected by I2C bus from Spresense. All CS2200CP registers can be displayed and written.
The following CONFIG must be enabled.

- Kernel
  - CONFIG_I2C
  - CONFIG_I2C_DRIVER
- SDK
  - CONFIG_CXD56_I2C
  - CONFIG_CXD56_I2C0
  - CONFIG_SYSTEM_I2CTOOL
  - CONFIG_NSH_MAXARGUMENTS = 7 (or more)

