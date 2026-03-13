clear; clc; close all;

% ===== Conexion con Arduino =====
% Ajusta el COM al que estés usando y recuerda cerrar el Serial Monitor de Arduino
arduino = serialport("COM11", 9600); 

% Activar la cámara web
cam = webcam(3);
pause(1); % Damos un segundo para que la cámara enfoque

figure

while true
    
    % Tomar una imagen de la cámara
    img = snapshot(cam);
    
    % Convertir imagen a escala de grises
    gray = rgb2gray(img);
    
    % Detectar zonas blancas (las figuras)
    BW = gray > 170;
    
    % Limpiar ruido pequeño
    BW = bwareaopen(BW,500);
    
    % Rellenar la figura
    BW = imfill(BW,'holes');
    
    % Copia de la imagen para dibujar resultados
    salida = img;
    
    % Obtener propiedades de cada objeto detectado
    stats = regionprops(BW,'Area','BoundingBox','Circularity','Centroid');
    
    for i = 1:length(stats)
        
        % Ignorar objetos muy pequeños
        if stats(i).Area < 500
            continue
        end
        
        bb = stats(i).BoundingBox;
        circ = stats(i).Circularity;
        centro = stats(i).Centroid;
        
        ancho = bb(3);
        alto  = bb(4);
        
        % Relación entre ancho y alto
        relacion = ancho / alto;
        
        % =========================
        % Detectar CIRCULO
        % =========================
        if circ > 0.8
            
            radio = (ancho + alto)/4;
            
            salida = insertShape(salida,'Circle',...
                [centro radio],...
                'Color','red','LineWidth',4);
            
            salida = insertText(salida,[centro(1)-40 centro(2)-40],...
                'CIRCULO','BoxColor','red','TextColor','white');
        
            % Enviar señal al Arduino
            write(arduino, "C", "char");
            
            % --- EL FRENO SALVAVIDAS ---
            % Detiene el bucle de MATLAB 3 segundos para que el Arduino respire
            pause(3); 
            
        % =========================
        % Detectar CUADRADO
        % =========================
        elseif relacion > 0.9 && relacion < 1.1
            
            salida = insertShape(salida,'Rectangle',bb,...
                'Color','green','LineWidth',4);
            
            salida = insertText(salida,[bb(1) bb(2)-30],...
                'CUADRADO','BoxColor','green','TextColor','white');
                
            % Enviar señal al Arduino
            write(arduino, "S", "char");
            
            % --- EL FRENO SALVAVIDAS ---
            % Detiene el bucle de MATLAB 3 segundos para que el Arduino respire
            pause(3); 
        end
        
    end
    
    % Mostrar resultado
    imshow(salida)
    drawnow
    
end
clear cam