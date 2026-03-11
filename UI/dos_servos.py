import tkinter as tk
import serial
import time

# --- CONFIGURACIÓN DEL PUERTO SERIE ---
puerto_arduino = 'COM11' 

try:
    arduino = serial.Serial(puerto_arduino, 9600)
    time.sleep(2) 
except Exception as e:
    print(f"Error al conectar con el Arduino: {e}")
    exit()

# --- FUNCIONES DE ENVÍO CON PREFIJOS ---
def enviar_angulo_1(val):
    # Agregamos la letra 'A' antes del número
    comando = 'A' + str(val) + '\n'
    arduino.write(comando.encode())

def enviar_angulo_2(val):
    # Agregamos la letra 'B' antes del número
    comando = 'B' + str(val) + '\n'
    arduino.write(comando.encode())

# --- CREACIÓN DE LA INTERFAZ GRÁFICA ---
ventana = tk.Tk()
ventana.title("Control de Brazo Robótico")
ventana.geometry("400x400")

# Slider 1
titulo1 = tk.Label(ventana, text="Motor 1 (A)", font=("Arial", 12))
titulo1.pack(pady=10)
slider = tk.Scale(ventana, from_=90, to=0, orient='horizontal', 
                  length=250, width=30, command=enviar_angulo_1)
slider.set(90) 
slider.pack()

# Slider 2
titulo2 = tk.Label(ventana, text="Motor 2 (B)", font=("Arial", 12))
titulo2.pack(pady=10)
slider2 = tk.Scale(ventana, from_=150, to=0, orient='horizontal',
                   length=250, width=30, command=enviar_angulo_2)
slider2.set(90)
slider2.pack()

ventana.mainloop()