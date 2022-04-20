#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "funciones.h"
#include "hashmap.h"
#include "list.h"
#include "pila.h"
#define MAX 300
#define CAP_PROD 300
#define CAP_MT 100

// Struct con los productos, marcas y tipos de producto disponibles en el almacen
// ademas de los carritos de compra del usuario
typedef struct Almacen{
    HashMap * Productos;
    HashMap * Marcas;
    HashMap * Tipos;
    List * Carritos;
    size_t tot_prod, tot_marc, tot_tipos, tot_car; //opcional.
}Almacen; 

// Struct que almacena informacion de cada producto particular
typedef struct Producto{
    char Nom_prod [MAX];
    char Marca [MAX];
    char Tipo [MAX];
    size_t precio, stock;
}Producto;

// Struct que almacena informacion de cada carrito de compra particular.
typedef struct Carrito{
    char Nom_car [MAX];
    size_t tot_car;
    Stack * Productos;
}Carrito;

// Busca un tipo de producto especifico en el mapa de tipos (La clave es el nombre del tipo)
// Accede a la lista almacenada en el mapa, imprimiendo todos sus productos.
void Buscar_tipo (char * t, Almacen * Global){

    Pair * aux;
    List * aux2;
    Producto * aux3;


    aux = searchMap(Global->Tipos, t);

    if (aux == NULL){
        printf("No existe este tipo de producto");
        return;
    }

    aux2 = (List *) return_value(aux);
    aux3= (Producto *)firstList (aux2);

    if (aux3 == NULL){
        printf("NO queda");
        return; 
    }

    printf("----Productos del tipo %s----\n", t);
    printf("-----------------------------------------------------------------\n");

    while (aux3 != NULL){
        printf("-->%-61s|\n",  aux3->Nom_prod);//imprime info producto
        printf("Marca: %-26s Stock:%4ld | Precio:%9ld |\n",aux3->Marca, aux3->stock, aux3->precio);
        printf("                                                                |\n");
        aux3 = (Producto *) nextList(aux2);
    }
    printf("-----------------------------------------------------------------\n");
    return; 
}

// Comprueba la existencia de un producto y lo agrega a los mapas y listas correspondientes.
void agregar_producto(Almacen * Global, char * Nom, char * Marc, 
                      char * Type, size_t Price, size_t Stock){
    Pair * aux, * aux2, * aux3;
    Producto * aux_prod;
    List * aux_list_mar;
    List * aux_list_tip;
    
    aux = (Pair  *) searchMap(Global->Productos, Nom);
    if (aux == NULL)
    {
        aux_prod = create_product(Nom, Marc, Type, Price, Stock); 
        insertMap(Global->Productos, aux_prod->Nom_prod , aux_prod);
    }
    else
    {
        aux_prod = (Producto *) return_value(aux);
        aux_prod->stock += Stock;
    }

    aux2 = (Pair *) searchMap(Global->Marcas, Marc);
    if (aux2 == NULL)
    {   
        aux_list_mar = createList();
        pushBack(aux_list_mar, aux_prod);
        insertMap(Global->Marcas, aux_prod->Marca, aux_list_mar);
    }
    else
    {
        aux_list_mar = (List *) return_value(aux2);
        pushBack(aux_list_mar, aux_prod);
    }

    aux3 = (Pair *) searchMap(Global->Tipos, Type);
    if (aux3 == NULL)
    {
        aux_list_tip = createList();
        pushBack(aux_list_tip, aux_prod);
        insertMap(Global->Tipos, aux_prod->Tipo, aux_list_tip);
    }
    else
    {
        aux_list_tip = (List *) return_value(aux3);
        pushBack(aux_list_tip, aux_prod);
    }
}

const char *get_csv_field (char * tmp, int k) {
    int open_mark = 0;
    char* ret=(char*) malloc (150*sizeof(char));
    int ini_i=0, i=0;
    int j=0;
    while(tmp[i+1]!='\0'){

        if(tmp[i]== '\"'){
            open_mark = 1-open_mark;
            if(open_mark) ini_i = i+1;
            i++;
            continue;
        }

        if(open_mark || tmp[i]!= ','){
            if(k==j) ret[i-ini_i] = tmp[i];
            i++;
            continue;
        }

        if(tmp[i]== ','){
            if(k==j) {
               ret[i-ini_i] = 0;
               return ret;
            }
            j++; ini_i = i+1;
        }

        i++;
    }

    if(k==j) {

        i = i-1;
       while(tmp[i]!=  '\0')
        {
            ret[i-ini_i] =  tmp[i];
            i++;
        }
       ret[i-ini_i] = 0;
       return ret;
    }

    return NULL;
}

// Crea nuevo porducto, rellenando los campos correspondientes. 
Producto * create_product(char * Nom, char * Marc, char * Tipo, size_t costo, size_t stock){
    Producto * new_prod = (Producto *) malloc (sizeof(Producto));
    // Comprobar reserva

    strcpy(new_prod->Nom_prod, Nom);
    strcpy(new_prod->Marca, Marc);
    strcpy(new_prod->Tipo, Tipo);
    new_prod->precio = costo;
    new_prod->stock = stock;

    return new_prod;
}

// Importa la informacion sobre los productos disponibles desde un
// archivo csv con formato especifico.
void importar_archivo(char * Nom_Arch, Almacen * gl){
    FILE * entrada;
    char str[MAX];
    char a_nom[MAX], a_Mar[MAX], 
         a_tip[MAX], a_price[MAX], a_stock[MAX];


    entrada = fopen(Nom_Arch, "rt");
    if(entrada == NULL)
    {
        perror("No se pudo abrir el archivo de lectura");
        exit(1);
    }

    while(fscanf(entrada,"%[^\n]", str) != EOF)
    {
        for (int i = 0; i<5 ; i++){
            const char * aux = get_csv_field(str, i);

            switch (i)
            {
            case 0:
                strcpy(a_nom, aux);
                break;
            case 1:
                strcpy(a_Mar, aux);
                break;
            case 2:
                strcpy(a_tip, aux);
                break;
            case 3:
                strcpy(a_stock, aux);
                break;
            case 4:
                strcpy(a_price, aux);
                break;
            default:
                printf("Se produjo un error\n");
                exit(1);
                break;
            }
        }

        agregar_producto(gl, a_nom, a_Mar, a_tip, (size_t) atol(a_price), (size_t) atol(a_stock));
        getc(entrada);
    }

    fclose(entrada);
    return;
}

// Crea un dato del tipo almacen con tal de guardar la informacion relevante.
Almacen * create_Almacen(void){
    Almacen * new_alm = (Almacen *) malloc(sizeof(Almacen));
    // Comprobar reserva

    new_alm->Carritos = createList();
    new_alm->Marcas = createMap(CAP_MT);
    new_alm->Productos = createMap(CAP_PROD);
    new_alm->Tipos = createMap(CAP_MT);

    new_alm->tot_car = 0;
    new_alm->tot_marc = 0;
    new_alm->tot_prod = 0;
    new_alm->tot_tipos = 0;

    return new_alm;
}

// Comprueba la exitencia de un carrito, agregando el producto deseado si es pertinente
void Agregar_a_carr(Carrito  * car, Producto * prod){


    return;
}

// Se recorre hashmap de marcas por ser de menor tamanyo que el de productos.
void exportar_archivo(Almacen * Global, char * arch){
    FILE * salida;
    Pair  * rec_marc;
    List * lista;
    Producto * rec_list;

    salida = fopen(arch, "wt");
    if(salida == NULL)
    {
        perror("No se pudo abrir el archivo de salida");
        exit(1);
    }

    rec_marc = firstMap(Global->Marcas);
    while(rec_marc != NULL)
    {
        lista  = return_value(rec_marc);
        rec_list = (Producto *) firstList(lista);

        while (rec_list != NULL)
        {
            fprintf(salida, "%s,%s,%s,%ld,%ld\n", rec_list->Nom_prod, rec_list->Marca, 
                        rec_list->Tipo, rec_list->stock, rec_list->precio);
            rec_list = (Producto *) nextList(lista);
        } 
        rec_marc = nextMap(Global->Marcas);      
    }

    fclose(salida);

    return;
}
