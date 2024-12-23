import tkinter as tk
from tkinter import ttk
import serial
import serial.tools.list_ports
import threading

def list_serial_ports():
    """Return a list of available serial ports."""
    return [port.device for port in serial.tools.list_ports.comports()]

def toggle_connection():
    """Toggle the serial connection."""
    global serial_connection

    if serial_connection and serial_connection.is_open:
        disconnect()
        connection_button.config(text="Connect")
    else:
        if connect():
            connection_button.config(text="Disconnect")

def connect():
    """Establish a serial connection."""
    global serial_connection

    port = port_combo.get()
    baud = baud_rate_combo.get()

    if not port or not baud:
        log_message("Error: Please select a valid port and baud rate.")
        return False

    try:
        serial_connection = serial.Serial(port, baudrate=int(baud), timeout=1)
        log_message(f"Connected to {port} at {baud} baud.")
        return True
    except Exception as e:
        log_message(f"Error: Failed to connect: {e}")
        return False

def disconnect():
    """Close the serial connection."""
    global serial_connection

    if serial_connection and serial_connection.is_open:
        serial_connection.close()
        log_message("Serial connection closed.")

    serial_connection = None

def send_command(command):
    """Send a command to the ESP32."""
    global serial_connection

    if not serial_connection or not serial_connection.is_open:
        log_message("Error: No active serial connection.")
        return

    if command:
        serial_connection.write((command + '\n').encode('utf-8'))
        log_message(f"Sent: {command}")

def toggle_sleep_mode():
    """Toggle the sleep mode."""
    if no_sleep_button.config('text')[-1] == "Disable Sleep":
        send_command("NO_SLEEP=1")
        no_sleep_button.config(text="Enable Sleep")
    else:
        send_command("NO_SLEEP=0")
        no_sleep_button.config(text="Disable Sleep")

def toggle_led():
    """Toggle the LED state."""
    if led_button.config('text')[-1] == "Turn LED On":
        send_command("LED=1")
        led_button.config(text="Turn LED Off")
    else:
        send_command("LED=0")
        led_button.config(text="Turn LED On")

def toggle_mqtt():
    """Toggle the MQTT state."""
    if mqtt_button.config('text')[-1] == "Enable MQTT":
        send_command("MQTT=1")
        mqtt_button.config(text="Disable MQTT")
    else:
        send_command("MQTT=0")
        mqtt_button.config(text="Enable MQTT")

def set_volume(val):
    """Send volume level to the ESP32."""
    send_command(f"VOLUME {val}")

def set_sleep_time():
    """Send sleep time to the ESP32."""
    sleep_time = sleep_time_entry.get()
    if sleep_time.isdigit():
        send_command(f"TIME_SLEEP {sleep_time}")
    else:
        log_message("Error: Sleep time must be a number.")

def sleep_device():
    """Send sleep command to the ESP32."""
    send_command("SLEEP")

def log_message(message):
    """Log a message to the log text box."""
    log_text.insert(tk.END, f"{message}\n")
    log_text.see(tk.END)

def clear_log():
    """Clear the log text box."""
    log_text.delete(1.0, tk.END)

# Initialize the GUI
root = tk.Tk()
root.title("ESP32 Controller")

# Serial Port Selection
port_label = tk.Label(root, text="Port:")
port_label.grid(row=0, column=0, padx=5, pady=5)
port_combo = ttk.Combobox(root, values=list_serial_ports(), state="readonly")
port_combo.grid(row=0, column=1, padx=5, pady=5)

# Baud Rate Selection
baud_rate_label = tk.Label(root, text="Baud Rate:")
baud_rate_label.grid(row=1, column=0, padx=5, pady=5)
baud_rate_combo = ttk.Combobox(root, values=[9600, 115200], state="readonly")
baud_rate_combo.grid(row=1, column=1, padx=5, pady=5)

# Connect/Disconnect Button with Sleep, No Sleep, LED Toggle, and MQTT Toggle
connection_button = tk.Button(root, text="Connect", command=toggle_connection)
connection_button.grid(row=2, column=0, padx=5, pady=5)
sleep_button = tk.Button(root, text="Sleep", command=sleep_device)
sleep_button.grid(row=2, column=1, padx=5, pady=5)
no_sleep_button = tk.Button(root, text="Disable Sleep", command=toggle_sleep_mode)
no_sleep_button.grid(row=2, column=2, padx=5, pady=5)
led_button = tk.Button(root, text="Turn LED On", command=toggle_led)
led_button.grid(row=2, column=3, padx=5, pady=5)
mqtt_button = tk.Button(root, text="Enable MQTT", command=toggle_mqtt)
mqtt_button.grid(row=2, column=4, padx=5, pady=5)

# Volume Slider
volume_label = tk.Label(root, text="Volume:")
volume_label.grid(row=3, column=0, padx=5, pady=5)
volume_slider = tk.Scale(root, from_=0, to=100, orient=tk.HORIZONTAL, command=set_volume)
volume_slider.grid(row=3, column=1, columnspan=3, padx=5, pady=5)

# Sleep Time Entry
sleep_time_label = tk.Label(root, text="Sleep Time (s):")
sleep_time_label.grid(row=4, column=0, padx=5, pady=5)
sleep_time_entry = tk.Entry(root, width=10)
sleep_time_entry.grid(row=4, column=1, padx=5, pady=5)
sleep_time_button = tk.Button(root, text="Set Sleep Time", command=set_sleep_time)
sleep_time_button.grid(row=4, column=2, columnspan=2, padx=5, pady=5)

# Log Text Box
log_label = tk.Label(root, text="Log:")
log_label.grid(row=5, column=0, padx=5, pady=5)
log_text = tk.Text(root, height=10, width=50, state="normal")
log_text.grid(row=6, column=0, columnspan=4, padx=5, pady=5)

# Clear Log Button
clear_log_button = tk.Button(root, text="Clear Log", command=clear_log)
clear_log_button.grid(row=7, column=0, columnspan=4, padx=5, pady=5)

# Initialize Serial Connection
serial_connection = None

root.mainloop()
