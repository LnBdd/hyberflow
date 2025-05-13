# Imports
import tkinter as tk
from tkinter import ttk
import serial
from itertools import count
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import threading
import time
from datetime import datetime
import pandas as pd
from scipy.integrate import simpson
import serial.tools.list_ports
import sys, os

# Class definition
class Gui:
    def __init__(self, root):
        """
        Initializes the main application window and its components.
        Args:
            root (tk.Tk): The root window of the Tkinter application.
        Methods:
            setup_ui(): Sets up the user interface components.
            setup_plots(): Configures the matplotlib plots.
            animate(): Updates the plots with new data.
        """
        
        self.root = root
        self.root.title("Hyberflow Control Panel")
        self.root.geometry("1200x600")
        self.root.configure(bg="#f4f4f4")
        plt.style.use('fivethirtyeight')

        # Setup UI components
        self.setup_ui()

        # Setup matplotlib canvas
        self.x_vals = []
        self.y_vals = []
        self.y2_vals = []
        self.current_flow_value = 0
        self.index = count()
        self.flowrate_target_speed = 0
        self.setup_plots()

        # Initialize animation
        self.ani = FuncAnimation(plt.gcf(), self.animate, interval=20)

        # Initialization
        self.print_status = False
        self.ser = None 
        self.confirmation_status = tk.BooleanVar(value=False)
        self.elapsed_time = 0
        self.old_time = 0   

    # UI Setup
    def setup_ui(self):
        """Sets up all the UI components."""

        style = ttk.Style()
        style.configure("TFrame", background="#f4f4f4")
        style.configure("TLabel", background="#f4f4f4", font=("Arial", 11))
        style.configure("TButton", font=("Arial", 10, "bold"), padding=5)
        style.configure("TEntry", padding=3)
        style.configure("TCombobox", padding=3)

        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky="nsew")

        # title_label = ttk.Label(main_frame, text="Hyberflow Control Panel", font=("Arial", 14, "bold"))
        # title_label.grid(row=0, column=0, columnspan=3, pady=10)

        self.serial_label = ttk.Label(main_frame, text="Choose serial port:")
        self.serial_label.grid(row=1, column=0, sticky="w", pady=5)
        self.serial_combobox = ttk.Combobox(main_frame, values=self.serial_ports())
        self.serial_combobox.grid(row=1, column=1, sticky="ew", pady=5)
        self.serial_combobox.bind('<<ComboboxSelected>>', self.on_select)

        flowrate_label = ttk.Label(main_frame, text="Desired Flowrate:")
        flowrate_label.grid(row=2, column=0, sticky="w", pady=5)
        self.flowrate_entry_var = tk.StringVar()
        flowrate_entry = ttk.Entry(main_frame, textvariable=self.flowrate_entry_var)
        flowrate_entry.grid(row=2, column=2, sticky="ew", pady=5)
        flowrate_scale = tk.Scale(main_frame, from_=0, to=2000, orient="horizontal",
                                  variable=self.flowrate_entry_var, command=self.update_entry_from_scale)
        flowrate_scale.grid(row=2, column=1, padx=5, pady=5)
        self.flowrate_entry_var.set(flowrate_scale.get())
        flowrate_entry.bind("<Return>", self.update_scale_from_entry)

        kp_label = ttk.Label(main_frame, text="P component of the controller: ")
        kp_label.grid(row=3, column=0, sticky="w", pady=5)
        self.kp_entry = ttk.Entry(main_frame)
        self.kp_entry.insert(0, "0.014")  
        self.kp_entry.grid(row=3, column=1, sticky="ew", pady=5)
        self.set_kp_button = ttk.Button(main_frame, text="Set KP", command=self.kp_button_clicked, state="disabled")
        self.set_kp_button.grid(row=3, column=2, padx=5, pady=5)

        ki_label = ttk.Label(main_frame, text="I component of the controller: ")
        ki_label.grid(row=4, column=0, sticky="w", pady=5)
        self.ki_entry = ttk.Entry(main_frame)
        self.ki_entry.insert(0, "0.008") 
        self.ki_entry.grid(row=4, column=1, sticky="ew", pady=5)
        self.set_ki_button = ttk.Button(main_frame, text="Set KI", command=self.ki_button_clicked, state="disabled")
        self.set_ki_button.grid(row=4, column=2, padx=5, pady=5)     

        button_frame = ttk.Frame(main_frame)
        button_frame.grid(row=6, column=0, columnspan=3, pady=10)

        self.start_button = ttk.Button(button_frame, text="Start", command=self.start_regulation)
        self.start_button.grid(row=0, column=0, padx=10)

        stop_button = ttk.Button(button_frame, text="Stop", command=self.stop_regulation)
        stop_button.grid(row=0, column=1, padx=10)

        self.text_display = tk.Text(main_frame, height=15, width=50)
        self.text_display.grid(row=8, column=0, columnspan=3)

        self.confirm_button = ttk.Button(main_frame, text="Confirm", command=self.on_confirm)
        self.confirm_button.grid(row=9, column=0, columnspan=3, pady=10)

    def setup_plots(self):
        """Setup matplotlib plots."""
        self.figure = plt.figure()
        self.canvas = FigureCanvasTkAgg(self.figure, master=self.root)
        self.canvas.get_tk_widget().grid(column=1, columnspan=3, row=0, sticky="nsew")  # Ensure proper alignment
        self.ax = self.figure.add_subplot(1, 1, 1)
        self.ax2 = self.ax.twinx()

        # Adjust layout to prevent text cutoff
        self.figure.subplots_adjust(left=0.15, right=0.85, top=0.9, bottom=0.1)
        self.figure.tight_layout()

    # Utility Methods
    def log_message(self, message):
        """
        Appends a log message to the text display widget and ensures the latest message is visible.
        Args:
            message (str): The log message to be displayed in the text widget.
        """
        
        self.text_display.insert(tk.END, message + '\n')
        self.text_display.see(tk.END)

    def update_entry_from_scale(self, value):
        """
        Updates the flow rate entry variable with the given value and calculates the elapsed time.
        Args:
            value (float): The new value to set for the flow rate entry variable.
        """
        
        self.flowrate_entry_var.set(value)
        self.elapsed_time = time.time() - self.old_time
        self.old_time = time.time()

    def update_scale_from_entry(self, event):
        """Update scale widget when entry changes."""
        try:
            value = float(self.flowrate_entry_var.get())
            if 0 <= value <= 2000:  
                print(f"Entry changed: {value}")
        except ValueError:
            pass

        self.elapsed_time = time.time() - self.old_time
        self.old_time = time.time()

    def on_confirm(self):
        """Handle Confirm button press."""
        self.confirmation_status.set(True)

    def serial_ports(self):
        """List available serial ports."""
        return [port.device for port in serial.tools.list_ports.comports()]

    def on_select(self, event=None):
        """Handle combobox selection."""
        self.port_selection = self.serial_combobox.get()
        print(f"Selected serial port: {self.port_selection}")

    # Serial Communication
    def open_serial_port(self, port='COM9', baudrate=115200):
        """
        Open a serial port with the specified parameters.
        Args:
            port (str): The name of the serial port to open. Defaults to 'COM9'.
            baudrate (int): The baud rate for the serial communication. Defaults to 115200.
        Raises:
            Exception: If there is an error while attempting to open the serial port.
        """
        
        try:
            self.ser = serial.Serial(self.port_selection, baudrate, timeout=1)
            print(f"Port {self.port_selection} opened successfully.")
            self.wait_for_setup_complete()
            
        except Exception as e:
            print(f"Error opening port: {e}")

    def close_serial_port(self):
        """Close serial port."""
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("Serial port closed.")

    def wait_for_setup_complete(self):
        """Wait for Arduino setup to complete."""
        try:
            while True:
                if self.ser.in_waiting > 0:
                    line = self.ser.readline().decode('utf-8').rstrip()
                    print(f"Incoming message: {line}")
                    if line == "Setup complete":
                        print("Arduino setup complete.")
                        message = "Fillsyringe"
                        self.ser.write(message.encode('utf-8'))
                        print(message)
                        print("Setup complete")
                        break
                time.sleep(0.01)
        except KeyboardInterrupt:
            print("Monitoring of the COM port stopped.")

    def wait_for_feedforwardcontrol_complete(self):
        """
        Monitors the serial communication for specific messages indicating the 
        completion or abortion of the feedforward control process. 
        Raises:
            KeyboardInterrupt: If the user interrupts the monitoring process.
        """
        
        print("Waiting for feedforward control to complete...")
        try:
            while True:
                if self.ser.in_waiting > 0:
                    line = self.ser.readline().decode('utf-8').rstrip()
                    print(f"Incoming message: {line}")
                    if line == "Feedforward Control complete":
                        print("Arduino feedforward control complete.")
                        self.set_ki_button.config(state="normal")
                        self.set_kp_button.config(state="normal")
                        print()
                        break
                    if line == "Feedforward Control aborted":
                        print("Arduino feedforward control aborted.")
                        message = "Finished"
                        self.ser.write(message.encode('utf-8'))
                        print(message)
                        print()
                        break
                time.sleep(0.01)
        except KeyboardInterrupt:
            print("Monitoring of the COM port stopped.") 
    
    def send_data_to_arduino(self, target_times, target_speeds, total_time, current_index, timestamp, interval=0.4):
        """
        Sends data to an Arduino device via a serial connection.
        Args:
            target_times (list): A list of target times (not used in the current implementation).
            target_speeds (list): A list of target speeds (not used in the current implementation).
            total_time (float): The total time for the operation (currently hardcoded to 300).
            current_index (int): The current index in the operation sequence.
            timestamp (float): The timestamp when the operation started.
            interval (float, optional): The interval between sending data. Defaults to 0.4.
        Returns:
            tuple: A tuple containing:
                - current_index (int): The current index in the operation sequence.
                - timestamp (float): The timestamp when the operation started.
                - error_flag (bool): A flag indicating whether an error occurred.
        Raises:
            Exception: If the serial port is not open or another error occurs during data transmission.
        """
        
        try:

            if self.ser is None or not self.ser.is_open:
                raise Exception("Serial port is not open.")
            
            elapsed_time = time.time() - timestamp

            target_speed = self.flowrate_entry_var.get()
            self.flowrate_target_speed = target_speed
            if self.all_sent:
                target_speed = float(0)
                print("Sent value:", target_speed)
            else:
                target_speed = float(target_speed)
            total_time =300

            message = f"{elapsed_time:.2f},{target_speed:.2f},{total_time:.2f}\n"
            self.ser.write(message.encode('utf-8'))
            return current_index, timestamp, False 

        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            print(exc_type, fname, exc_tb.tb_lineno)
            print(f"Error sending data: {e}")
            return current_index, timestamp, True  

    def read_and_store_serial_data(self, Parameters, flow_values, timestamp_cycle):  
        """
        Reads and processes serial data from a connected device, storing the parsed values
        into corresponding lists and updating parameters.
        Args:
            Parameters (dict): A dictionary to store parameter values such as 'KP', 'KI',
                'Regelungsintervall', 'I2CIntervall', and 'Window_Size'.
            flow_values (list): A list to store the current flow values.
            timestamp_cycle (list): A list to store timestamps corresponding to flow values.
        Returns:
            tuple: A tuple containing the following elements:
                - flow_values (list): Updated list of flow values.
                - support (list): List of 'Support' values parsed from the serial data.
                - soll (list): List of 'Soll' values parsed from the serial data.
                - ampl (list): List of 'ampl' values parsed from the serial data.
                - Average (list): List of 'Average' values parsed from the serial data.
                - Difference (list): List of 'Diff' values parsed from the serial data.
                - Integral (list): List of 'Integral' values parsed from the serial data.
                - Output (list): List of 'Output' values parsed from the serial data.
                - Parameters (dict): Updated dictionary of parameters.
                - timestamp_cycle (list): Updated list of timestamps.
        Raises:
            ValueError: If there is an error converting serial data to the expected format.
            IndexError: If there is an error accessing data from the parsed serial input.
        """
        
        support = []
        soll = []
        ampl = []
        Average = []
        Difference = []
        Integral = []
        Output = []

        if self.ser.in_waiting > 0:
            try:
                line = self.ser.readline().decode('utf-8').strip().split(",")
                self.ser.flush()
                self.ser.reset_input_buffer()
                
                for i in range(len(line)):
                    if line[i] == "Flow":
                        self.current_flow_value = float(line[i+1])
                        flow_values.append(self.current_flow_value)
                        timestamp_cycle.append(time.time())
                    elif line [i] == "Support":
                        support.append(float(line[i+1]))
                    elif line[i] == "Soll":
                        soll.append(float(line[i+1]))
                    elif line[i] == "ampl":
                        ampl.append(float(line[i+1]))
                    elif line[i] == "Average":
                        Average.append(float(line[i+1]))
                    elif line[i] == "Diff":
                        Difference.append(float(line[i+1]))
                    elif line[i] == "Integral":
                        Integral.append(float(line[i+1]))
                    elif line[i] == "Output":
                        Output.append(float(line[i+1]))
                    elif line[i] == "KP":
                        Parameters['KP'] = float(line[i+1])
                    elif line[i] == "KI":
                        Parameters['KI'] = float(line[i+1])
                    elif line[i] == "Regelungsintervall":
                        Parameters['Regelungsintervall'] = float(line[i+1])
                    elif line[i] == "I2CIntervall":
                        Parameters['I2CIntervall'] = float(line[i+1])
                    elif line[i] == "Window":
                        Parameters['Window_Size'] = float(line[i+1])
                    
            except ValueError:
                print("Error converting serial data.")
            except IndexError:
                print("Error accessing data.")

        return flow_values, support, soll, ampl, Average, Difference, Integral, Output, Parameters, timestamp_cycle

    # Plotting
    def animate(self, i):
        """
        Updates the animation by appending new data points to the x, y, and y2 value lists,
        trimming the lists to the last 50 elements, and re-plotting the data.
        Args:
            i (int): The current frame index of the animation (unused in this function).
        """

        self.x_vals.append(next(self.index))
        self.y_vals.append(int(self.flowrate_target_speed))
        self.y2_vals.append(self.current_flow_value)

        self.x_vals = self.x_vals[-50:]
        self.y_vals = self.y_vals[-50:]
        self.y2_vals = self.y2_vals[-50:]
        
        self.ax.cla()
        self.ax2.cla()
        self.ax.plot(self.x_vals, self.y_vals, label='Set Value')
        self.ax2.plot(self.x_vals, self.y2_vals, color='r', label='Actual Value')

        self.ax.set_xlabel('Time (s)')
        self.ax2.set_ylabel('Flowrate (µl/min)')
        self.ax2.yaxis.label.set_color('black')

        self.ax.set_ylim(0, 2500)
        self.ax2.set_ylim(0, 2500)

        self.ax.legend(loc='upper left')
        self.ax2.legend(loc='upper right')

        # Adjust layout to prevent cutoff
        self.figure.tight_layout()

    def save_data_to_excel(self, timestamp_cycle, flow_values):
        """
        Saves flow rate data to an Excel file.
        Args:
            timestamp_cycle (list of float): A list of timestamps (in seconds) representing
                the time points of the cycle.
            flow_values (list of float): A list of flow rate values (in µl/min) corresponding
                to the timestamps.
        """
        
        data = {
            'Time (s)': [t - timestamp_cycle[0] for t in timestamp_cycle],
            'Flowrate (µl/min)': flow_values
        }
        df = pd.DataFrame(data)

        filename = f"FlowrateData_{datetime.now().strftime('%d%m%Y_%H%M%S')}.xlsx"
        df.to_excel(filename, index=False)
        print(f"Data successfully saved in {filename}.")

    def plot_flow_vs_time(self, timestamp_cycle, flow_values):
        """
        Plots the flow rate over time and calculates the total volume using the Simpson's rule.
        Args:
            timestamp_cycle (list of float): A list of timestamps (in seconds) representing the time points of the measurements.
            flow_values (list of float): A list of flow rate values (in µl/min) corresponding to the timestamps.
        """
        
        relative_times = [t - timestamp_cycle[0] for t in timestamp_cycle]
        flow_values_per_sec = [flow / 60 for flow in flow_values]
        area_simpson = simpson(y=flow_values_per_sec, x=relative_times)
        print(f'Total volume: {area_simpson:.2f} µl')

        # Save the plot as an image
        plt.figure(figsize=(10, 6))
        plt.plot(relative_times, flow_values, color='blue')
        plt.xlabel('Time in s')
        plt.ylabel('Flowrate in µl/min')
        plt.title('Flowrate over Time')
        plt.grid(True)
        plt.savefig("flowrate_plot.png")
        plt.close()

    # Main Control Logic
    def kp_button_clicked(self):
        """Callback for KP button."""
        value = self.kp_entry.get()
        value = float(value)
        print(type(value))
        print(f"KP set to: {value}")
        self.ser.write(f"KP,{value}\n".encode('utf-8'))

    def ki_button_clicked(self):
        """Callback for KI button."""
        value = self.ki_entry.get()
        value = float(value)
        print(f"KI set to: {value}")
        self.ki_value = value
        self.ser.write(f"KI,{value}\n".encode('utf-8'))

    def start_regulation(self):
        """Start control process."""
        self.print_status = True
        print("Starting regulation...")
        threading.Thread(target=self.control_loop, daemon=True).start()
        self.start_button.config(state="disabled")

    def stop_regulation(self):
        """Stop control process."""
        self.all_sent = True
        self.print_status = False
        print("Stopping regulation...")

    def read_from_serial(self):
        """Reads data from the serial port."""
        while self.print_status and self.ser and self.ser.is_open:
            if self.ser.in_waiting > 0:
                line = self.ser.readline().decode('utf-8').strip()
                print("Received: " + line)

    def main(self):
        """
        Main function to control the flow of the program.
        This function handles the initialization, data communication with an Arduino device,
        and error handling for a flow control system.
        
        Attributes:
            timestamp_cycle (list): A list to store timestamps of the data cycle.
            flow_values (list): A list to store flow values read from the Arduino.
            confirmation_status (tkinter.Variable): A tkinter variable to manage user confirmation.
            ser (serial.Serial): Serial communication object for Arduino communication.
        Exceptions:
            Catches and logs any exceptions that occur during the process, including
            the type of exception, file name, and line number.
        Returns:
            bool: True if the process completes successfully, False if aborted due to an error.
        """


        timestamp_cycle = []
        flow_values = []

        try:
            target_times = [2]
            target_speeds = [300]
            total_time = 2


            print("Confirm print.")

            root.wait_variable(self.confirmation_status)

            self.confirmation_status.set(False)
            print()
            timestamp = time.time()
            current_index = 0
            Parameters = {'KP': None, 'KI': None, 'Regelungsintervall': None, 'I2CIntervall': None, 'Window_Size': None}
            self.all_sent = False

            last_read_time = time.time()
            last_send_time = time.time()

            while not self.all_sent:
                current_time = time.time()
            
                if current_time - last_read_time >= 0.2:
                    
                    flow_values, support, soll, ampl, Average, Difference, Integral, Output, Parameters, timestamp_cycle = self.read_and_store_serial_data(Parameters, flow_values, timestamp_cycle)
                    last_read_time = current_time


                if current_time - last_send_time >= 0.4:
                    current_index, timestamp, all_sent = self.send_data_to_arduino(self.ser, target_times, target_speeds, total_time, current_index, timestamp)
                    last_send_time = current_time

            current_index, timestamp, all_sent = self.send_data_to_arduino(self.ser, target_times, target_speeds, total_time, current_index, timestamp)


            # self.save_data_to_excel(timestamp_cycle, flow_values)
            # self.plot_flow_vs_time(timestamp_cycle, flow_values)

            message = "Finished"
            self.ser.write(message.encode('utf-8'))
            print(message)

            self.flowrate_target_speed = 0
            self.current_flow_value = 0
                
            

        except Exception as e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            print(exc_type, fname, exc_tb.tb_lineno)
            print(f"An error occurred: {e}")
            print("Please confirm with 'j' if you want to re-enter the print parameters.")
            printing_parameters = input("If the print should be aborted, confirm with the (Enter key):").strip().lower()
            if printing_parameters == 'j':
                print()
                self.main()
            else:
                message = "Finished"
                self.ser.write(message.encode('utf-8'))
                self.ser.write(message.encode('utf-8'))
                self.ser.write(message.encode('utf-8'))
                # send 3 times to ensure the message is received
                print(message)
                return False

        return True

    def control_loop(self):
        """
        Main control loop managing user interactions, hardware communication, 
        and process flow for the print cycle. 
        Exceptions:
            Logs errors and ensures serial port is closed properly.
        """

        try:
            print("Running control loop...")

            while True:
                try: 
                    self.confirmation_status.set(False)
                    self.log_message("\nDo you want to start a new print cycle?\n")
                    root.wait_variable(self.confirmation_status)

                    self.confirmation_status.set(False)
                    self.log_message("\nHave you filled the reservoir, inserted the syringe into the device, and provided a drip tray for the bio-ink during calibration?")
                    root.wait_variable(self.confirmation_status)
                    self.open_serial_port()
                    self.confirmation_status.set(False)

                    while True:
                        self.log_message("\nThe system pumps drive fluid into the syringe to fill the empty volume in the syringe. Please confirm as soon as the syringe is filled by pressing the Confirm button.")
                        root.wait_variable(self.confirmation_status)
                        
                        if self.confirmation_status:
                            self.confirmation_status.set(False)
                            message = "Stop"
                            self.ser.write(message.encode('utf-8'))
                            self.log_message("\nAutomatic residual filling of the syringe completed. Measured values for pre-control are recorded.")
                            self.ser.flush()
                            self.ser.reset_input_buffer()
                            self.wait_for_feedforwardcontrol_complete()
                            self.log_message("\nRemove the drip tray and confirm.")
                            successful_run = self.main()
                            if not successful_run:
                                self.log_message("\nRestart in 5 seconds...")
                                time.sleep(5)
                            break
                        else:
                            message = "Finished"
                            self.ser.write(message.encode('utf-8'))
                            print(message)
                            break

                except Exception as e:
                    self.log_message(f"An unexpected error occurred: {e}")
                finally:
                    self.close_serial_port()
                    self.ser = None

        except Exception as e:
            self.log_message(f"Error in control loop: {e}")

# Main execution
if __name__ == "__main__":
    root = tk.Tk()
    app = Gui(root)
    root.mainloop()
