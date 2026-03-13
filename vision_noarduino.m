clear; clc; close all;

% Activar la cámara web
cam = webcam(3);

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
        
        % =========================
        % Detectar CUADRADO
        % =========================
        elseif relacion > 0.9 && relacion < 1.1
            
            salida = insertShape(salida,'Rectangle',bb,...
                'Color','green','LineWidth',4);
            
            salida = insertText(salida,[bb(1) bb(2)-30],...
                'CUADRADO','BoxColor','green','TextColor','white');
        end
        
    end
    
    % Mostrar resultado
    imshow(salida)
    drawnow
    
end

clear cam