/*                      								INSTITUTO TECNOLÓGICO DE COSTA RICA
													ADMINISTRACIÓN DE TECNOLOGÍAS DE INFORMACIÓN
															LENGUAJES DE PROGRAMACIÓN
															
																TAREA PROGRAMADA I
												
												Integrantes:
																LUIS DIEGO COTO MATA
																ERICK VARGAS VICTOR
																MARIAM RAMIREZ CORDERO
												PROFESOR: 
																ANDRÉI FUENTES LEIVA
																
																	I SEMESTRE 2013.

 */
 
 /*
																Librerías Utilizadas
*/ 
 
 
#include <stdlib.h> // standard library: funciones como atoi, malloc serán utilizadas y se importarán de dicha biblioteca 
#include <string.h> //contiene es de utilidad para trabajar con cadenas de caracteres y algunas operaciones de manipulación de memoria
#include <stdio.h> //standard input-output header, es la biblioteca estándar del lenguaje de programación C, el archivo de cabecera que contiene las definiciones de macros, las constantes, las declaraciones de funciones y la definición de tipos usados por varias operaciones estándar de entrada y salida.
#include <arpa/inet.h> // Funciones para el manejo de directorios.
#include <unistd.h>// Proporciona acceso al POSIX
#include <signal.h>




/*Función encargada del envio de archivos */

int cliente(int argc, char *argv[]) { 
	printf("\033[33m** Habilitado el envio de archivos** \n \033[0m"); //  \033[33m** esto hará que los printfs puedan tener colores en la consola
	int respuesta;
	sleep(1);
	printf("\033[33m Digite 1 para enviar archivos \n \033[0m");
	scanf("%d",&respuesta);
					
	if (respuesta==1){
		if (argc!=4){
			perror("No se ingresó la cantidad de datos solicitados"); // Se refiere a que deberá de ingresar los datos exactos que son el nombre del programa .\transmisor ip(inet) puerto1 puerto2
			return -1;
			}
		
		char archivo;
		printf ("\033[33m Digite el nombre del archivo: \033[0m"); // Se pedirá el nombre del archivo si este no leyó lo deseado se utilizará un error que el archivo no existe
		scanf ("%s",&archivo);
		
		
		

 
	//  descriptor Socket: SOCK_STREAM es para TCP y SOCK_DGRAM para UDP 
		int desc = socket(AF_INET, SOCK_STREAM, 0);//desc contiene el descriptor del socket
 
     // Direccion Local
		struct sockaddr_in *direcc_loc = malloc(sizeof(struct sockaddr_in));
			{  (*direcc_loc).sin_family = AF_INET;
			(*direcc_loc).sin_addr.s_addr = INADDR_ANY;
			(*direcc_loc).sin_port = htons(atoi(argv[3]));
 
			bind(desc, (struct sockaddr *)direcc_loc, sizeof(struct sockaddr_in));
			}
  
     // Direccion remota (se desea conectar a) 
		struct sockaddr_in *direcc_rem = malloc(sizeof(struct sockaddr_in));
				{       
					(*direcc_rem).sin_family = AF_INET;
					(*direcc_rem).sin_addr.s_addr = inet_addr(argv[1]);
					(*direcc_rem).sin_port = htons(atoi(argv[2])); }
 
      //Conectarse al servidor
		connect(desc, (struct sockaddr *)direcc_rem , sizeof(struct sockaddr_in) );
		
	  //Archivo para enviar  el modelo 
		FILE *archivoEnviar; // Archivo a enviar 
		const char *archiv=&archivo;
		char buf[30]; // enviar el tamaño
		char buff[30]; // enviar el nombre
		char barchivo; //buf donde se enviará un solo byte 
	  
	  //Ejemplo:"/home/usuario(luisd,erick,mariam)/Desktop/..."
		archivoEnviar=fopen(archiv,"rb");  //Se leerá el archivo mediante una lectura binaria, rb= read binaria
	  
	  //En caso que el archivo no encontrará nada devolverá un error que no se halló 
		if (archivoEnviar==NULL){
			perror("No se puede abrir el archivo, proceso terminado \n");
			return -1;}
		
		//Se pondrá el puntero al final del archivo para saber su tamaño  
		fseek(archivoEnviar, 0, SEEK_END); //se coloca al final  del archivo 
		int len = ftell(archivoEnviar); //le indicará la cantidad de bytes para enviar en el archivo
		sprintf(buf,"%d",len); //se utilizará sprintf para enviar la cantidad almacenada en len y la pondrá el formato en el buf
		sprintf(buff,"%s",archiv);		
		
		rewind(archivoEnviar);// se asegura devolver el archivo para que empiece de cero su lectura
 
        //Se envía un mensaje de conexión establecida con el servidor 
		send( desc,"\033[34m Conexión establecida  \n \033[0m",30,0);
		
		printf("\033[33m Cliente: Enviando Archivo...  \n \033[0m");
		send(desc,buff,1,0);//nombre			
		send(desc,buf,30, 0);//tamano
       
        //La función de read se basa en leer byte por byte y de esta manera se podrá enviar un archivo de cualquier tamaño
		sleep(1); // Para imprimir en orden y dejar que el código se espera a que no empiece con la emisión de archivos
		while (!feof( archivoEnviar )){		   
			fread (&barchivo, 1, 1, archivoEnviar);
			send(desc,&barchivo, 1,0);  //Se enviará un archivo de cantidad ilimitada debido a que se envía byte por byte         
        }
      
	   // Cerrar el socket y la conexion
		fclose(archivoEnviar);
		sleep(2);
		//printf("\033[33m Cliente: %s enviado correctamente \n \033[0m",buff);
		close(desc);
		free(direcc_loc); // se liberan la dirección local 
		free(direcc_rem); // se libera la dirección remota
		return 0;
				
	}
	else{ // Si el usuario digita Finalizar
		exit(-1);}
return 0;	
}

/*Función encargada de la recepción de archivos  */
int servidor(int argc, char *argv[]){
	sleep(1);
	printf("\033[32m** Habilitada la recepción de archivos** \n \033[0m");
	int tam_direccion = sizeof(struct sockaddr_in);
 
    //  descriptor Socket: SOCK_STREAM es para TCP y SOCK_DGRAM para UDP 
    int desc = socket(AF_INET, SOCK_STREAM, 0);
         
    // Direccion Local 
    struct sockaddr_in *direcc_loc = malloc(sizeof(struct sockaddr_in));

        { // Puerto local se utilizará el 3650 
          (*direcc_loc).sin_family = AF_INET;
          (*direcc_loc).sin_addr.s_addr = INADDR_ANY;// inet_addr(argv[1]);
          (*direcc_loc).sin_port = htons(atoi(argv[2]));
           bind(desc, (struct sockaddr *)direcc_loc , sizeof(struct sockaddr_in));
        }
  
	// Va a contener maximo 5 conexiones
    listen(desc, 5);
    
    // Direccion remota (se desea conectar a) 
    struct sockaddr_in *direcc_rem = malloc(sizeof(struct sockaddr_in));
   
        
    //Conexión entrante aquí se realiza la conexión y la acepta
    int clientremoto= accept(desc, (struct sockaddr *)direcc_rem, (void *)&tam_direccion);
 
    
    
    /*Datos inicializados*/
    char barchiv;  
    int len;    
    int cont=1;
	char cantidad;
    char buf_bienvenida[30];  
    char buf_nombrearch[30];
    
   
   
    
    /*Se recibirá un mensaje para mostrar que los procesos */
    recv(clientremoto,buf_bienvenida , 30, 0); 
    printf("%s \n",buf_bienvenida); // Se imprimirá un mensaje de conexión establecida
    recv(clientremoto,buf_nombrearch,1,0);
        
		    
    //Archivo recibido
    FILE *archivoRecibido;    
	archivoRecibido = fopen ("ArchivoNuevo", "wb"); //Se abrirá un archivoe haya este archivo sobreescribirá y sino lo encuentra este lo creará
		
		/*Si el archivo no lo pudiera abrir el archivo se terminará el proceso*/
		if (archivoRecibido==NULL){
			perror("No se puede abrir el archivo,proceso terminado \n");
			return -1;}
		
		/* Se recebirá el tamaño del archivo*/ 
		recv(clientremoto, &cantidad, 30, 0);	   
		char*cantid=&cantidad;
		
		//Se utiliza la función atoi para transformar la cantidad debido a que cantidad se encuentra el tamaño en tipo char
		while (cont!=atoi(cantid)){
			memset( &barchiv, '\0', sizeof(barchiv) );// memset: sobrescribe  en un área de memoria con un patrón de bytes dado 
			
        if( (len = recv ( clientremoto, &barchiv, 1, 0 )) == -1 ){ // Se comprobará que envié al menos un byte 
			return -1;}
        else if ( len > 0 ){ 
                fwrite (&barchiv, 1, 1, archivoRecibido); //Se empezará a escribir byte por byte el archivo
                cont++;}
        }
			
            
		fclose(archivoRecibido); // Se deberá cerrar el archivo para que se termine su creación
		printf("\033[32mServidor: ArchivoNuevo: %s recibido correctamente \n \033[0m",buf_nombrearch);
		printf("Servidor: Tamaño del archivo recibido: %d \n \n",atoi(cantid));
		

        //Cerrar el socket 
        close(desc);
     
		//se libera el emisor y el receptor 
        free(direcc_loc ); 
        free(direcc_rem);  
	
        return 0;
         
}
int main(int argc, char *argv[]){
	int hijo=fork();  // Se clona el proceso y se utilizara el PID para diferenciarlos	
	
	if (hijo ==0){ // Si es el proceso hijo entrará aca
		int procesoHijo= getpid();
		procesoHijo--;
		cliente( argc,argv);
		kill(procesoHijo, SIGKILL); // Terminamos el proceso 
	

	}
	else{  // Si es el proceso padre, tomará este camino
		servidor(argc,argv);   // Se llama a servidor
		sleep(1);
		printf("\033[30m\"El que no posee el don de maravillarse,ni de entusiasmarse\n ");
		printf(" más le valdría estar muerto,porque sus ojos están cerrados.\"\n \033[0m");
		sleep(1);		
		printf("Salida con éxito \n");	
		int procesoPadre= getpid();
		procesoPadre--;	
		kill(procesoPadre,SIGKILL);
		}	
			

	return 0;

	
}






