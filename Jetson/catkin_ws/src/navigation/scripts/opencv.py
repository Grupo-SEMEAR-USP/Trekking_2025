import cv2

def mostrar_coordenadas(evento, x, y, flags, param):
    if evento == cv2.EVENT_LBUTTONDOWN:
        valor = img[y, x]
        print(f"Coordenadas: x={x}, y={y} | Intensidade: {valor}")

# Lê a imagem PGM em tons de cinza
img = cv2.imread('map_simul.pgm', cv2.IMREAD_GRAYSCALE)

if img is None:
    print("Erro ao carregar a imagem 'map_simul.pgm'")
else:
    cv2.imshow('Mapa Simulado', img)
    cv2.setMouseCallback('Mapa Simulado', mostrar_coordenadas)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


#CÓDIGO PARA ENCONTRAR AS COORDENADAS NO MAPA