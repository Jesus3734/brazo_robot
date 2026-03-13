import tkinter as tk
import serial
import time

# --- CONFIGURACIÓN DEL PUERTO SERIE ---
puerto_arduino = 'COM11' 

try:
    # ¡ATENCIÓN! Cambiamos a 57600 baudios para coincidir con el Arduino
    arduino = serial.Serial(puerto_arduino, 57600)
    time.sleep(2) 
except Exception as e:
    print(f"Error al conectar con el Arduino: {e}")
    exit()

# --- FUNCIONES DE ENVÍO CON PREFIJOS ---
def enviar_angulo_1(val):
    comando = 'A' + str(val) + '\n'
    arduino.write(comando.encode())

def enviar_angulo_2(val):
    comando = 'B' + str(val) + '\n'
    arduino.write(comando.encode())

# --- FUNCIÓN DE LECTURA (El "Oído" de Python) ---
def leer_serial():
    # Si hay datos esperando en el cable USB...
    if arduino.in_waiting > 0:
        try:
            # Leemos la línea, la decodificamos y le quitamos los saltos de línea
            linea = arduino.readline().decode('utf-8').strip()
            
            # Separamos los 3 valores separados por coma (M1, M2, MPU)
            datos = linea.split(',')
            
            if len(datos) == 3:
                # Actualizamos las etiquetas en la pantalla
                lbl_lectura_m1.config(text=f"Enviado M1: {datos[0]}°")
                lbl_lectura_m2.config(text=f"Enviado M2: {datos[1]}°")
                lbl_lectura_mpu.config(text=f"Lectura MPU: {datos[2]}°")
                
                # Imprimimos en la terminal para que puedas copiar y pegar en Excel
                print(f"{datos[0]}\t{datos[1]}\t{datos[2]}")
        except Exception as e:
            # Ignoramos errores de lectura si llegan datos incompletos
            pass
            
    # Le decimos a tkinter que vuelva a ejecutar esta función en 10 milisegundos
    ventana.after(10, leer_serial)

# --- CREACIÓN DE LA INTERFAZ GRÁFICA ---
ventana = tk.Tk()
ventana.title("Control de Brazo y Lectura MPU")
ventana.geometry("450x500")

# Slider 1
titulo1 = tk.Label(ventana, text="Motor 1 (A)", font=("Arial", 12, "bold"))
titulo1.pack(pady=(15, 0))
slider = tk.Scale(ventana, from_=90, to=0, orient='horizontal', 
                  length=250, width=30, command=enviar_angulo_1)
slider.set(90) 
slider.pack()

# Slider 2
titulo2 = tk.Label(ventana, text="Motor 2 (B)", font=("Arial", 12, "bold"))
titulo2.pack(pady=(15, 0))
slider2 = tk.Scale(ventana, from_=150, to=0, orient='horizontal',
                   length=250, width=30, command=enviar_angulo_2)
slider2.set(90)
slider2.pack()

# --- SECCIÓN DE LECTURA EN PANTALLA ---
separador = tk.Frame(ventana, height=2, bd=1, relief="sunken", width=400)
separador.pack(pady=20)

titulo3 = tk.Label(ventana, text="Datos en Tiempo Real", font=("Arial", 12, "bold"))
titulo3.pack()

lbl_lectura_m1 = tk.Label(ventana, text="Enviado M1: --", font=("Arial", 11), fg="blue")
lbl_lectura_m1.pack()

lbl_lectura_m2 = tk.Label(ventana, text="Enviado M2: --", font=("Arial", 11), fg="blue")
lbl_lectura_m2.pack()

# Destacamos el MPU en rojo para que se note
lbl_lectura_mpu = tk.Label(ventana, text="Lectura MPU: --", font=("Arial", 14, "bold"), fg="red")
lbl_lectura_mpu.pack(pady=10)

# Arrancamos el ciclo de lectura justo antes de iniciar la ventana
ventana.after(100, leer_serial)

# Evitar que el puerto se quede abierto si cierras la ventana
def on_closing():
    arduino.close()
    ventana.destroy()

ventana.protocol("WM_DELETE_WINDOW", on_closing)
ventana.mainloop()