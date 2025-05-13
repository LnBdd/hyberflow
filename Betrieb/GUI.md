# Hyberflow GUI

The `GUI.py` module provides a graphical user interface (GUI) for controlling the Hyberflow system. This GUI allows users to configure flowrate parameters, interact with the system, and monitor real-time data.

---

## Features

- **Serial Port Selection**: Select the appropriate serial port for communication with the Arduino.
- **Flowrate Control**: Set the desired flowrate using a slider or text input.
- **Controller Parameters**: Adjust the proportional (`KP`) and integral (`KI`) components of the controller.
- **Real-Time Plotting**: Visualize the target and actual flowrate in real-time.
- **Start/Stop Control**: Start or stop the regulation process.
- **Data Logging**: Save flowrate data to an Excel file and generate a flowrate plot.

---

## How to Use

### 1. Launch the GUI

Run the `GUI.py` script to start the GUI:

```bash
python GUI.py
```

### 2. Select the Serial Port

- Use the dropdown menu labeled **"Choose serial port"** to select the serial port connected to the Arduino.
- The selected port will be displayed in the console.

### 3. Set the Desired Flowrate

- Use the slider to adjust the flowrate (range: 0 to 2000 µl/min).
- Alternatively, enter a specific value in the text box and press `Enter` to update the slider.

### 4. Start the Regulation Process

- Click the **Start** button to begin the regulation process.
- The system will communicate with the Arduino and start controlling the flowrate.

### 5. Configure Controller Parameters

- Adjust the **P component** (`KP`) and **I component** (`KI`) of the controller using the respective text boxes.
- Click the **Set KP** or **Set KI** buttons to apply the changes.

### 6. Monitor Real-Time Data

- The real-time plot displays:
  - **Set Value**: The target flowrate.
  - **Actual Value**: The measured flowrate.
- The plot updates every 20 milliseconds.

### 7. Stop the Regulation Process

- Click the **Stop** button to halt the regulation process.

### 8. Save Data

- The system automatically logs flowrate data during the regulation process.
- Use the `save_data_to_excel` method to save the data to an Excel file.
- Use the `plot_flow_vs_time` method to generate a plot of flowrate over time.

---

## GUI Components

### Main Window

- **Serial Port Selection**: Dropdown menu to select the serial port.
- **Flowrate Control**: Slider and text box to set the desired flowrate.
- **Controller Parameters**: Text boxes and buttons to configure `KP` and `KI`.
- **Start/Stop Buttons**: Buttons to start or stop the regulation process.
- **Real-Time Plot**: Displays the target and actual flowrate.
- **Log Display**: Text area to display system messages and logs.

---

## Screenshot

![438698921-f1f55479-3b48-4a40-8a07-6002c9fa855d](https://github.com/user-attachments/assets/ecfcf23c-3f79-4eac-96d0-ac9f0ed26c76)



---

## Notes

- Ensure the Arduino is connected and properly configured before starting the GUI.
- The GUI communicates with the Arduino using serial communication. Make sure the correct serial port is selected.
- The `KP` and `KI` values should be adjusted carefully to achieve optimal control performance.

---

## Troubleshooting

- **Serial Port Not Found**: Ensure the Arduino is connected and the correct drivers are installed.
- **No Data on Plot**: Verify that the Arduino is sending data and the serial port is open.
- **Error Messages**: Check the console for detailed error messages.

---

## Requirements

- Python 3.x
- Required Python libraries:
  - `tkinter`
  - `serial`
  - `matplotlib`
  - `pandas`
  - `scipy`

Install the required libraries using:

```bash
pip install pyserial matplotlib pandas scipy
```

---
