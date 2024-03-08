# Scripts and Tools for Maintenance

In the case that the JoyWarrior56FR1 no longer sends data but is recognized and displayed correctly in the device manager, all device drivers must be reinstalled.
For this purpose we provide the PowerShell script, which uninstalls all devices with the appropriate VendorID and ProductID.

### Uninstall it yourself

1. Start Device Manager with the JoyWarrior56FR1 connected
2. In the list, navigate to the "Human Interface Devices" section. A JoyWarrior56FR1 will have 5 entries that need to be removed:
   - 1x HID-compliant game controller
   - 2x HID-compliant devices
   - 2x USB input device
3. Select an entry from the list (double-click) and go to the "Details" tab. Under Properties, choose "Device Instance Path" and check if the value contains the following entry: ***VID_07C0&PID_111A***
4. Once you have identified an entry, go to the "Driver" tab and uninstall the device
5. Repeat steps 3 and 4 until no device with VID_07C0&PID_111A is left
6. Disconnect the JoyWarrior56FR1 from the USB
7. Restart your PC (optional)
8. Reconnect the JW56FR1 to the USB

After this procedure, the JoyWarrior56FR1 should function normally again.
