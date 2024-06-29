#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALTURA_MAX 100

// Estructura para un nodo del árbol de Huffman
struct Nodo_Min {
    unsigned char data;
    unsigned freq;
    struct Nodo_Min *left, *right;
};

// Estructura para el montículo mínimo
struct Monticulo_Min {
    unsigned size;
    unsigned capacity;
    struct Nodo_Min** array;
};

// Función para crear un nuevo nodo del árbol de Huffman
struct Nodo_Min* nuevo_Nodo(unsigned char data, unsigned freq) {
    struct Nodo_Min* temp = (struct Nodo_Min*)malloc(sizeof(struct Nodo_Min));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// Función para crear un montículo mínimo de capacidad dada
struct Monticulo_Min* crear_Monticulo_Min(unsigned capacity) {
    struct Monticulo_Min* monticulo_min = (struct Monticulo_Min*)malloc(sizeof(struct Monticulo_Min));
    monticulo_min->size = 0;
    monticulo_min->capacity = capacity;
    monticulo_min->array = (struct Nodo_Min**)malloc(monticulo_min->capacity * sizeof(struct Nodo_Min*));
    return monticulo_min;
}

// Función para intercambiar dos nodos del montículo mínimo
void int_Nodo_Min_Arbol(struct Nodo_Min** a, struct Nodo_Min** b) {
    struct Nodo_Min* t = *a;
    *a = *b;
    *b = t;
}

// Función para mantener la propiedad del montículo mínimo
void mont_min(struct Monticulo_Min* mm, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < mm->size && mm->array[left]->freq < mm->array[smallest]->freq)
        smallest = left;

    if (right < mm->size && mm->array[right]->freq < mm->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        int_Nodo_Min_Arbol(&mm->array[smallest], &mm->array[idx]);
        mont_min(mm, smallest);
    }
}

// Función para verificar si el tamaño del montículo es 1
int es_uno(struct Monticulo_Min* minHeap) {
    return (minHeap->size == 1);
}

// Función para extraer el nodo mínimo del montículo
struct Nodo_Min* extraer_Min(struct Monticulo_Min* minHeap) {
    struct Nodo_Min* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    mont_min(minHeap, 0);
    return temp;
}

// Función para insertar un nuevo nodo en el montículo
void insertar_Min(struct Monticulo_Min* mm, struct Nodo_Min* nodo_min) {
    ++mm->size;
    int i = mm->size - 1;
    while (i && nodo_min->freq < mm->array[(i - 1) / 2]->freq) {
        mm->array[i] = mm->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    mm->array[i] = nodo_min;
}

// Función para construir el montículo mínimo
void construir_mm(struct Monticulo_Min* monticuloMin) {
    int n = monticuloMin->size - 1;
    int i;
    for (i = (n - 1) / 2; i >= 0; --i)
        mont_min(monticuloMin, i);
}

// Función para verificar si un nodo es hoja
int es_hoja(struct Nodo_Min* root) {
    return !(root->left) && !(root->right);
}

// Función para crear y construir el montículo mínimo
struct Monticulo_Min* cc_nodoMin(unsigned char data[], int freq[], int size) {
    struct Monticulo_Min* mm = crear_Monticulo_Min(size);
    for (int i = 0; i < size; ++i)
        mm->array[i] = nuevo_Nodo(data[i], freq[i]);
    mm->size = size;
    construir_mm(mm);
    return mm;
}

// Función principal para construir el árbol de Huffman
struct Nodo_Min* c_arbolHuffman(unsigned char data[], int freq[], int size) {
    struct Nodo_Min *left, *right, *top;
    struct Monticulo_Min* mm = cc_nodoMin(data, freq, size);

    while (!es_uno(mm)) {
        left = extraer_Min(mm);
        right = extraer_Min(mm);
        top = nuevo_Nodo('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertar_Min(mm, top);
    }
    return extraer_Min(mm);
}

// Función para generar los códigos de Huffman
void generaCodigoH(struct Nodo_Min* root, int arr[], int top, char codes[256][ALTURA_MAX]) {
    if (root->left) {
        arr[top] = 0;
        generaCodigoH(root->left, arr, top + 1, codes);
    }
    if (root->right) {
        arr[top] = 1;
        generaCodigoH(root->right, arr, top + 1, codes);
    }
    if (es_hoja(root)) {
        for (int i = 0; i < top; ++i) {
            codes[root->data][i] = arr[i] + '0';
        }
        codes[root->data][top] = '\0';
    }
}

// Función para obtener los códigos de Huffman
void obtener_Codigos(struct Nodo_Min* root, char codes[256][ALTURA_MAX]) {
    int arr[ALTURA_MAX], top = 0;
    generaCodigoH(root, arr, top, codes);
}

// Función para leer el archivo de entrada y contar frecuencias
void cuenta_Frecuencias(const char* filename, unsigned char* data, int* freq, int* size) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        printf("No se pudo abrir el archivo %s\n", filename);
        exit(1);
    }

    unsigned char byte;
    *size = 0;
    while (fread(&byte, sizeof(unsigned char), 1, file) == 1) {
        if (freq[byte] == 0) {
            data[*size] = byte;
            (*size)++;
        }
        freq[byte]++;
    }

    fclose(file);
}

// Función para escribir la codificación en un archivo binario
void codificar(const char* inputFilename, const char* outputFilename, char codes[256][ALTURA_MAX]) {
    FILE* inputFile = fopen(inputFilename, "rb");
    FILE* outputFile = fopen(outputFilename, "wb");

    if (inputFile == NULL || outputFile == NULL) {
        printf("Error al abrir los archivos\n");
        exit(1);
    }

    unsigned char byte;
    unsigned char buffer = 0;
    int bitCount = 0;

    while (fread(&byte, sizeof(unsigned char), 1, inputFile) == 1) {
        for (int i = 0; codes[byte][i] != '\0'; ++i) {
            buffer = (buffer << 1) | (codes[byte][i] - '0');
            bitCount++;

            if (bitCount == 8) {
                fwrite(&buffer, sizeof(unsigned char), 1, outputFile);
                buffer = 0;
                bitCount = 0;
            }
        }
    }

    if (bitCount > 0) {
        buffer <<= (8 - bitCount);
        fwrite(&buffer, sizeof(unsigned char), 1, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);
}

// Función para escribir la tabla de frecuencias en un archivo de texto
void escribir_TablaF(const char* filename, unsigned char* data, int* freq, int size) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("No se pudo crear el archivo %s\n", filename);
        exit(1);
    }

    for (int i = 0; i < size; i++) {
        fprintf(file, "%c: %d\n", data[i], freq[data[i]]);
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <nombre_del_archivo>\n", argv[0]);
        return 1;
    }

    const char* inputFilename = argv[1];
    unsigned char data[256];
    int freq[256] = {0};
    int size = 0;

    cuenta_Frecuencias(inputFilename, data, freq, &size);

    printf("Códigos de Huffman:\n");
    struct Nodo_Min* root = c_arbolHuffman(data, freq, size);
    char codes[256][ALTURA_MAX] = {{0}};
    obtener_Codigos(root, codes);

    for (int i = 0; i < size; ++i) {
        printf("%c: %s\n", data[i], codes[data[i]]);
    }

    codificar(inputFilename, "codificacion.dat", codes);
    escribir_TablaF("frecuencias.txt", data, freq, size);

    printf("\nCodificación completada. Revise 'codificacion.dat' y 'frecuencias.txt'.\n");

    return 0;
}
