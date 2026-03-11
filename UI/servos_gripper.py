import tkinter as tk
import serial
import time

# --- CONFIGURACIÓN DEL PUERTO SERIE ---
# declaracion de puerto usb
puerto_arduino = 'COM11' 

try:
    arduino = serial.Serial(puerto_arduino, 9600)
    time.sleep(2) # bootloader de arduino 
except Exception as e:
    print(f"Error al conectar con el Arduino: {e}")
    exit()

# --- FUNCIÓN QUE SE EJECUTA AL MOVER EL SLIDER ---
def enviar_angulo(val):
    # convertir y enviar angulo mediante slider
    comando = str(val) + '\n'
    arduino.write(comando.encode())

# --- CREACIÓN DE LA INTERFAZ GRÁFICA ---
ventana = tk.Tk()
ventana.title("Control de Brazo Robótico")
ventana.geometry("300x400")

titulo = tk.Label(ventana, text="Ángulo del Servo", font=("Arial", 14))
titulo.pack(pady=20)

# configuracion del slider
slider = tk.Scale(ventana, from_=50, to=0, orient='vertical', 
                  length=250, width=30, command=enviar_angulo)

slider.set(10) # posicion de arranque del slider, dependiendo del codigo de arduino puede ser necesario cambiarlo a 0 o 180
slider.pack()

# inicio del bucle de  la app
ventana.mainloop()