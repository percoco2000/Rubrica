/*
 * Program : Rubrica.c
 * --------------------------------------------------------------------
 * Gregorio Marino <percoco2000@minty>
 * 
 * Una semplice rubrica telefonica
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Defines :
 * --------------------------
 * CLEAR()       : A macro to clear the screen in linux terminal
 * WAIT_ENTER()  : A macro that wait for user to press ENTER key  
 * RUBRICA       : Nome del file
 * MAX_NOMI      : Nomi gestibili dal programma
 * MAX_NAME_CHAR : Massima lunghezza campo nome (-1)
 * MAX_PHON_CHAR : Massima lunghezza campo numero (-1)
 * MAX_NOTE_CHAR : Massima lunghezza campo note (-1)
 */

# define CLEAR() printf("\033[H\033[J")
# define WAIT_ENTER() while (getchar()!='\n')
# define RUBRICA "rubrica.txt"
# define MAX_NOMI 200
# define MAX_NAME_CHAR 41
# define MAX_PHON_CHAR 14
# define MAX_NOTE_CHAR 128
/*
 * Typedefs :
 * --------------------------
 * bool       : Self Explanatory
 * Elemento_T : Struct che memorizza una singola entry
 *              il campo 'index' memorizza l'indice che
 *              il record occupa nell'array. Utile nelle
 *              funzioni 'Cancella_Nome' e 'Modifica_Nome' 
 * Rubrica_T  : Struct che memozizza l'intera rubrica
 *              Puo' contenere max 200 nominativi.
 *              n_nominativi contiene il numero di 
 *              elementi insriti nella rubrica
 */

typedef enum {FALSE,TRUE} bool ;

typedef struct Elemento_T 
             {
	          char *nome;
	          char *numero;
	          char *note;
			  int index;
			  } Elemento_T;
			 
typedef struct Rubrica_T
             {
	          Elemento_T nominativi[MAX_NOMI];
	          int n_nominativi;
			  } Rubrica_T;
			     			    

/*
 * Functions prototypes
 */
static Rubrica_T Inizializza_Rubrica (void);
static void Write_File (Rubrica_T *rubrica);		     			    
static int Main_Menu(Rubrica_T *rubrica);
static void Cerca_Nome(Rubrica_T *rubrica);
static void Aggiungi_Nome(Rubrica_T *rubrica);
static void Modifica_Nome(Rubrica_T *rubrica);
static void Cancella_Nome(Rubrica_T *rubrica);
static void Stampa_Rubrica(Rubrica_T *rubrica);
static void Inserisci_Record (Rubrica_T *rubrica, Elemento_T elemento);
static Rubrica_T Cerca_Corrispondenze (Rubrica_T *rubrica, char chiave[]);
static void Cancella_Record (Rubrica_T *rubrica,int index);
static void Ordina_Rubrica(Rubrica_T *rubrica);

// Accesorie's Functions
static void Input_String (char line[],int n);
static int Input_Integer(void);
static void Strip_Spaces (char stringa[]);
static bool FindString (char string[], char sub[]);
static void ToLower(char strnig[]);
static bool YesOrNo(void);

/*
 * Main Function
 */
int main(void)
  {
   Rubrica_T rubrica;
   bool File_Change;
   int scelta;
   
   scelta=0;
   rubrica=Inizializza_Rubrica();
   
   while (scelta!=6)
       {
        File_Change=FALSE;
      
        
        scelta=Main_Menu(&rubrica);
        switch (scelta)
             {
              case 1:
                 Cerca_Nome(&rubrica);
                 break;
                 
              case 2:
                 Aggiungi_Nome(&rubrica);
                 File_Change=TRUE;
                 break;
                 
              case 3:
                 Modifica_Nome(&rubrica);
                 File_Change=TRUE;
                 break;
                 
              case 4:
                 Cancella_Nome(&rubrica);
                 File_Change=TRUE;
                 break;
                 
              case 5:
                 Stampa_Rubrica(&rubrica);
                 printf(" Premi ENTER per continuare");
                 WAIT_ENTER();
                 break;
                 
              default:
                    break;
              }
        if (File_Change) 
         {
          Write_File(&rubrica);
          File_Change=FALSE;
          }                  
        //printf ("scelta %d \n",scelta);
        }
   
   
   }



/*
 * Function : Main_Menu
 * Usage    : choice = Main_menu();
 * -----------------------------------------------------------
 * This function display the user menu, wait for a choice, performs
 * some input controls, and return the choice to the caller.
 */
 static int Main_Menu(Rubrica_T *rubrica)
      {
	   bool scelta_errata;
	   char temp[16];
	   int scelta;
	   
	   scelta_errata=FALSE;
	   scelta=0;
	   
	   while (scelta==0)
	       {
	        CLEAR();
	        printf ("\n          **** Rubrica Telefonica ****\n");
	        printf ("-----------------------------------------------\n");
	        printf (" Presenti %d Records su %d\n\n",rubrica->n_nominativi,MAX_NOMI);
	        printf ("   Main Menu\n");
	        printf ("  -----------\n\n");
	        printf (" 1) Cerca un nome\n");
	        printf (" 2) Aggiungi un nome\n");
	        printf (" 3) Modifica un nome\n");
	        printf (" 4) Cancella un nome\n");
	        printf (" 5) Stampa intera Rubrica\n");
            printf (" 6) Exit\n\n");
	        
	        if (scelta_errata)
	         {
			  printf ("Opzione non valida\n");
			  scelta_errata=FALSE;
		      }else printf("\n");
	        
	        printf ("Scegli un'opzione : ");
	        fgets(temp,8,stdin);
	        scelta=temp[0]-'0';
	        
	        if (scelta<1 || scelta>6)
	         {
			  scelta=0;
			  scelta_errata=TRUE;
		      }
	        }
	   return (scelta);
       }
       
/*
 * Function : Inizializza_Rubrica
 * Usage    : rubrica=Inizializza_Rubrica();
 * -----------------------------------------------------------
 * This function initialize a rubrica database with the data
 * read from the file. If the file desn't exist, the function 
 * creates one with only the header. It returns the database.
 */
static Rubrica_T Inizializza_Rubrica (void)
          {
		   FILE *file;
           Rubrica_T temp;
           char buffer[255],*temp1,*temp2,*temp3,termchar;
           int num_letti=0,i;
           
           temp.n_nominativi=0;
           
           
           if((file = fopen(RUBRICA,"r"))!=NULL)
            {
              // Skip the header
              for (i=0;i<7;i++)
                {
				 fgets(buffer,254,file);
			     }
              
              // Simple check on the last header's line
              if (strcmp(buffer,"___________________________________________\n"))
               {
				printf("Bad Header in %s ",RUBRICA);
				exit(1);
			    }
			    
              // Read the records
              while (fgets(buffer,254,file)!=NULL)
                  {
		           
		           temp1=malloc(MAX_NAME_CHAR);
		           temp2=malloc(MAX_PHON_CHAR);
		           temp3=malloc(MAX_NOTE_CHAR); 
		           sscanf(buffer,"%39[^,],%12[^,],%120[^\n]%c",temp1,temp2,temp3,&termchar);
		           
		           temp.nominativi[num_letti].nome=temp1;
		           temp.nominativi[num_letti].numero=temp2;
		           temp.nominativi[num_letti].note=temp3;
		           temp.nominativi[num_letti].index=num_letti;
		           num_letti++;
		   
		           if (num_letti>MAX_NOMI) 
		            {
			         printf("Troppi nomi nel file\n");
			         fclose(file);
                     exit (1);
		             }
		           }
		     temp.n_nominativi=num_letti;     
	         fclose(file);
             }else
                 {
                  // File doesn't exist, creates an empty one
                  Write_File (&temp);
                  }
	       
           Ordina_Rubrica(&temp);
           
           return temp; 
		   }

/* 
 * Function : Write_file
 * Usage    : Write_File( &rubrica );
 * --------------------------------------------------------------------
 * This function save the content of 'rubrica' in the file as specified 
 * in the defined RUBRICA. Furthermore it writes the file header.
 */
static void Write_File (Rubrica_T *rubrica)
        {
	  	 FILE *file;
	  	 char temp[255];
	  	 int i;
	  	 
	  	 file=fopen(RUBRICA,"w");
         if (file==NULL) printf("Errore nel Creare il file ... Sorry");
         
         // Writes file header
         fputs("-----Database per il programma Rubrica-----\n",file);
         fputs("-------------------------------------------\n",file);
         fputs("Formato dei records\n",file);
         fputs("Nome e cognome, Numero telefonico, Note\n",file);
         fputs("-------------------------------------------\n",file);
         fputs("Non Modificare a mano!!!!! \n",file);
         fputs("___________________________________________\n",file);
         
         // Now the rubrica database
         for (i=0;i<rubrica->n_nominativi;i++)
           {
			sprintf(temp,"%s,%s,%s\n",rubrica->nominativi[i].nome,rubrica->nominativi[i].numero,rubrica->nominativi[i].note);
			fputs(temp,file);
		    }
                  
         fclose(file); 
		 }

/* 
 * Function : Cerca_Nome 
 * Usage    : Cerca_Nome (& rubrica);
 * ---------------------------------------------------------
 * Cerca un nome nel database e visualizza tutte le occorrenze
 * di quel nome. La funzione e' implementata in modo da cercare 
 * anche con pezzi di nome. Man mano che trova le corrispondenze
 * costruisce un nuovo database da passare a 'Stampa_Rubrica'
 */          
static void Cerca_Nome(Rubrica_T *rubrica)
     {
      char chiave[41];
      Rubrica_T temp_rb;
      
      temp_rb.n_nominativi=0;
      
      CLEAR();
      printf ("\n\n   Ricerca un nome nella Rubrica\n\n");
	  printf ("----------------------------------------\n");
	  printf ("Immetti un nome vuoto per annullare\n\n");
      printf ("Nome da cercare ? ");
      
      Input_String(chiave,40);
      
      // Annullare?
	  if (chiave[0]=='\0') return;
      
      temp_rb=Cerca_Corrispondenze(rubrica,chiave);
		 
      
      // Se nessun record e' stato trovato visualizza 
      // l'informazione ed esce dalla funzione
      
      if (temp_rb.n_nominativi==0)
       {
		CLEAR();   
		printf ("Nessuna corrispondenza trovata \n");
		printf ("Con la chiave di ricerca '%s' !\n\n",chiave);
		printf ("Premi ENTER per continuare");
		WAIT_ENTER();
		return;
	    } 
       
       Stampa_Rubrica(&temp_rb);
       printf ("Premi ENTER per continuare");
       WAIT_ENTER();
       
      }           

/*
 * Function : Aggiungi_Nome
 * Usage    : Aggiungi_Nome( &rubrica)
 * ----------------------------------------------------------
 * Funzione che permette di aggiungere un nome alla rubrica:
 * Nome, Numero e note vengono conservati nelle variabili 
 * temp[1,2,3], e, quando l'utente ha confermato la correttezza
 * dei dati inseriti, viene allocata la memoria per copiarle
 * nella struct 'elemento' , che viene poi passato alla funzione
 * 'Inserisci_Record'. Sarebbe stato possibile passare direttamente
 * i tre valori a 'Inserisci_Record', ma cosi prendo confidenza
 * con le struct, i puntatori e la gestione memoria. Per lo stesso 
 * motivo, la funzione 'Inserisci_Record' alloca la memoria necessaria
 * a conservare i nuovo record, ragion per cui, 'elemento' va' 
 * deallocato dopo la chiamata.
 * Viene data la possibilita' all'utente di annullare l'operazione
 * premendo semplicemente 'ENTER' alla richiesta del nome.
 * Viene inoltre controllato che il nuovo record non superi la 
 * capacita' del database, nel qual caso viene avvisato l'utente 
 */

static void Aggiungi_Nome(Rubrica_T *rubrica)
     {
      
      Elemento_T nominativo;
      char temp1[255],temp2[255],temp3[255];
      
      CLEAR();
      
      if (rubrica->n_nominativi<MAX_NOMI)
       {
		do 
		  {
		   CLEAR();	  
		   printf ("\n\n   Aggiungi un nome alla rubrica\n\n");
		   printf ("----------------------------------------\n");
		   printf ("Immetti un nome vuoto per annullare\n\n");
		
		   printf ("Nome da aggiungere : ");
		   Input_String(temp1,MAX_NAME_CHAR-1);
		
		   // Annullare?
		   if (temp1[0]=='\0') return;
		
		   
		   printf ("Numero telefonico  : ");
		   Input_String(temp2,MAX_PHON_CHAR-1);
		   
		
		   printf ("Note varie         : ");
		   Input_String(temp3,MAX_NOTE_CHAR-1);
		   
           printf("\n\nE' corretto (y/n) ? ");  
	       } while (!YesOrNo()); 
	    
	    nominativo.nome=malloc(MAX_NAME_CHAR);
	    strcpy(nominativo.nome,temp1);
		
		nominativo.numero=malloc(MAX_NAME_CHAR);
		strcpy(nominativo.numero,temp2); 
		 
		nominativo.note=malloc(MAX_NOTE_CHAR);
		strcpy(nominativo.note,temp3);
        
        Inserisci_Record(rubrica,nominativo);
        
        free (nominativo.nome);
        free (nominativo.numero);
        free (nominativo.note);
	       
	    } else 
	         {
			  printf ("\n\n\n    Impossibile aggiungere il record\n");
			  printf ("   Massimo numero di nominativi raggiunto\n\n");
		      printf (" Premi ENTER");
		      WAIT_ENTER();
		      }
		      		   
	          
      }


/*
 * Function : Modifica_Nome
 * Usage    : Modifica_Nome(&rubrica);
 * ----------------------------------------------------------
 * Implementazione della funzione per modificare un record.
 * Molto simile alla funzione Cancella_Nome, dopo aver scelto il 
 * record da modificare vengono chiesti all'utente i nuovi dati, dando
 * la possibilita' di lasciare invariato il campo. Se resta invariato 
 * il nome, viene aggiornato il record, altrimenti viene cancellato il 
 * vecchio record e creato uno nuovo.
 * 
 * temp_rb    = Database temporaneo popolato con i record corrisondenti alla ricerca
 * nominativo = Record temporaneo in cui vengono immagazzinati i nuovi dati.
 */

static void Modifica_Nome(Rubrica_T *rubrica)
     {
      
      int j;
      char temp[255];
      Elemento_T nominativo;
      Rubrica_T temp_rb;
      
     
      temp_rb.n_nominativi=0;
      nominativo.index=0;
      
      CLEAR();
      printf ("\n\n   Modifica un record dalla Rubrica\n\n");
	  printf ("----------------------------------------\n");
	  printf ("Immetti un nome vuoto per annullare\n\n");
      printf ("Nome da modificare ? ");
      
      Input_String(temp,40);
      
      // Annullare?
	  if (temp[0]=='\0') return;
      
      temp_rb=Cerca_Corrispondenze(rubrica,temp); 
      
      if (temp_rb.n_nominativi==0)
       {
		CLEAR();
		printf ("Nessuna corrispondenza trovata\n");
		printf ("Con la chiave di ricerca '%s'!\n\n",temp);
		printf ("Premi ENTER per continuare");
		WAIT_ENTER();
		return;
	    }
		
      /*
       * 'j' corrisponde al numero di record trovati:
       * Se sono piu' di 1 (j>1) vengono stampati a video ed all'utente
       * viene chiesto quale cancellare. Il risultato e' assegnato sempre a 'j'.
       * Si puo' annullare l'operazione rispondendo '0'.
       * Se e' un solo record, 'j' vale 1, e si passa direttamente alla 
       * conferma di cancellazione del record.
       */ 
      
      j=temp_rb.n_nominativi;
      
      // Vi e' piu' di un nominativo che corrisponde al criterio di ricerca?
      if (j>1)
       {
        j=-1;
        Stampa_Rubrica(&temp_rb);
        while (TRUE)
            {
             printf ("Quale record vuoi modificare?\n");
             printf ("Immetti 0 per annulare\n -> ");;
             j=Input_Integer();
             if (j>-1 && j<=temp_rb.n_nominativi) break;
             printf("Record non valido!\n");
             }
        // Annullare?
        if (j==0) return;     
        }     
      
      CLEAR();
      
      /*
       * Viene riallineato il numero del record con l'indice
       * dell'array temp_rb
       */
      j--;
      
      printf ("Modifica il record:\n");
      printf ("-------------------------------------------------------\n");
      printf ("Nome  :%-30s -> Num: %s\n",temp_rb.nominativi[j].nome,temp_rb.nominativi[j].numero);
      printf ("Note  :%s\n\n",temp_rb.nominativi[j].note);
      
      // Alloca memoria per record temporaneo
      nominativo.nome=malloc(MAX_NAME_CHAR);
      nominativo.numero=malloc(MAX_PHON_CHAR);
      nominativo.note=malloc(MAX_NOTE_CHAR);
      
      //  Ininizializza il record temporaneo (nominativo) con i dati del record da modificare
      //  Il nome viene posto a "" e cambiato solo se variato.      
      nominativo.index=temp_rb.nominativi[j].index;
      nominativo.nome[0]='\0';
      
      strcpy(nominativo.numero,rubrica->nominativi[nominativo.index].numero);
      strcpy(nominativo.note,rubrica->nominativi[nominativo.index].note);
      
      // Nome
      printf ("Immetti Nome (premi ENTER per lasciare invariato)\n");
      Input_String(temp,MAX_NAME_CHAR-1);
      
      if (temp[0]!='\0')
       {
		strcpy(nominativo.nome,temp);
	    }
	    
	  // Numero
      printf ("Immetti Numero (Premi ENTER per lasciare invariato)\n");
      Input_String(temp,MAX_PHON_CHAR-1);
      if (temp[0]!='\0')
       {
        strcpy (nominativo.numero,temp);
        }
      
      //Note
      printf ("Immetti note (Premi ENTER per lasciare invariato)\n");
      Input_String(temp,MAX_NOTE_CHAR-1);
      if (temp[0]!='\0')
       {
		strcpy(nominativo.note,temp);
	    }  
	  
      // Aggiorna o, in caso di variazione del nome, sostituisce il record
      if (nominativo.nome[0]=='\0')
       {
        strcpy(rubrica->nominativi[nominativo.index].numero,nominativo.numero);
        strcpy(rubrica->nominativi[nominativo.index].note,nominativo.note);
        }else
            {
             Cancella_Record(rubrica,nominativo.index);   
             Inserisci_Record(rubrica,nominativo);
             }
   
   free(nominativo.nome);
   free(nominativo.numero);
   free(nominativo.note);

   return;
  }    


/*
 * Function : Cancella_Nome
 * Usage    : Cancella_nome(&rubrica);
 * -----------------------------------------------------
 * Implementazione della funzione per cancellare un record.
 * La funzione richiede il nome da cancellare, chiama Cerca_Corrispondenze
 * per popolare il database temp_rb. Se piu' risultati
 * corrispondono al criterio di ricerca, chiede quale record 
 * va cancellato. Viene quindi chiamata la funzione Cancella_Record
 * per cancellare il record con indice rubrica.nominativi[j].index.
 * 
 * Ad ogni passaggio, viene data la possibilita di annulare l'operazione. 
 */
static void Cancella_Nome(Rubrica_T *rubrica)
     {
      int j;
      char chiave[41];
      Rubrica_T temp_rb;
      
      temp_rb.n_nominativi=0;
      
      CLEAR();
      printf ("\n\n   Elimina un nome dalla Rubrica\n\n");
	  printf ("----------------------------------------\n");
	  printf ("Immetti un nome vuoto per annullare\n\n");
      printf ("Nome da cancellare ? ");
      
      Input_String(chiave,40);
      
      // Annullare?
	  if (chiave[0]=='\0') return;
      
      temp_rb=Cerca_Corrispondenze(rubrica,chiave); 
      
      if (temp_rb.n_nominativi==0)
       {
		CLEAR();
		printf ("Nessuna corrispondenza trovata!\n\n");
		printf ("Premi ENTER per continuare");
		WAIT_ENTER();
		return;
	    }
		
      /*
       * 'j' corrisponde al numero di record trovati:
       * Se sono piu' di 1 (j>1) vengono stampati a video ed all'utente
       * viene chiesto quale cancellare. Il risultato e' assegnato sempre a 'j'.
       * Si puo' annullare l'operazione rispondendo '0'.
       * Se e' un solo record, 'j' vale 1, e si passa direttamente alla 
       * conferma di cancellazione del record.
       */ 
      
      j=temp_rb.n_nominativi;
      
      // Vi e' piu' di un nominativo che corrisponde al criterio di ricerca?
      if (j>1)
       {
        j=-1;
        Stampa_Rubrica(&temp_rb);
        while (TRUE)
            {
             printf ("Quale record vuoi cancellare?\n");
             printf ("Immetti 0 per annulare\n -> ");;
             j=Input_Integer();
             if (j>-1 && j<=temp_rb.n_nominativi) break;
             printf("Record non valido!\n");
             }
        // Annullare?
        if (j==0) return;     
        }     
      
      CLEAR();
      
      /*
       * Viene riallineato il numero del record con l'indice
       * dell'array temp_rb
       */
      j--;
      
      printf ("Cancella il record:\n");
      printf ("-------------------------------------------------------\n");
      printf ("Nome  :%-30s -> Num: %s\n",temp_rb.nominativi[j].nome,temp_rb.nominativi[j].numero);
      printf ("Note  :%s\n\n",temp_rb.nominativi[j].note);
      printf ("Confermi (y/n)? ");
      if (!YesOrNo()) return;
      
      
      // Cancella il record con indice temp_rb.nominativi[j].index dal database 'rubrica'
          
      Cancella_Record(rubrica,temp_rb.nominativi[j].index);
	}       
      
        
/*
 * Function : Stampa_Rubrica
 * Usage    : Stampa_Rubrica(&rubrica);
 * -------------------------------------------------------------------
 * Print the entire rubrica on the screen, pausing every 8 records;
 */
 static void Stampa_Rubrica(Rubrica_T *rubrica)
      {
       int i;
       CLEAR();
       printf ("Record Trovati %d\n\n",rubrica->n_nominativi);
       for (i=0;i<rubrica->n_nominativi;i++)
         {
	      printf ("       ----------------> Record %d <----------------\n",i+1);
          printf ("Nome  :%-30s -> Num: %s\n",rubrica->nominativi[i].nome,rubrica->nominativi[i].numero);
	      printf ("Note  :%s\n\n",rubrica->nominativi[i].note);
          if (!((i+1)%5))
           {
            printf ("ENTER -> Prossimi nomi");
            WAIT_ENTER();
            CLEAR();
            }
          }	 
       }



/* Function : Cerca_Corrispondenze
 * Usage    : risultato=Cerca_Corrispondenze(&record,chiave)
 * --------------------------------------------------------------------------
 * Questa funzione ritorna un database con tutti i record che contengono
 * la stringa 'chiave' nel campo 'nome'
 */
 static Rubrica_T Cerca_Corrispondenze (Rubrica_T *rubrica, char chiave[])
      {
	   char temp_st[41];
	   int i;
	   Rubrica_T temp_rb;
	   
	   Strip_Spaces(chiave);
       ToLower(chiave);
      
       temp_rb.n_nominativi=0;
       
       for (i=0;i<rubrica->n_nominativi;i++)
         {
		  strcpy(temp_st,rubrica->nominativi[i].nome);
		  Strip_Spaces(temp_st);
		  ToLower(temp_st);
		 
		  if (FindString(temp_st,chiave))
		   {
		    
	        temp_rb.nominativi[temp_rb.n_nominativi].nome=rubrica->nominativi[i].nome;
	        temp_rb.nominativi[temp_rb.n_nominativi].numero=rubrica->nominativi[i].numero;
	        temp_rb.nominativi[temp_rb.n_nominativi].note=rubrica->nominativi[i].note;
	        temp_rb.nominativi[temp_rb.n_nominativi].index=i;
	        
	        temp_rb.n_nominativi++;
	        }
	      }
	   return (temp_rb);
	   }     
	
/* 
 * Function : Inserisci_Record 
 * Usage    : Inserisci_Record (&rubrica,elemento);
 * -----------------------------------------------
 * Questa funzione inserisce il record 'elemento' nel
 * database 'rubrica'. La funzione e' implementata in
 * modo da inserire il record mantenendo il database
 * ordinato alfabeticamente. Viene quindi cercato 
 * l'indice dove inserire il record, viene traslato il 
 * resto del database, viene inserito l'elemento, ed
 * aggiornato il numero dei records presenti. 
 */
static void Inserisci_Record (Rubrica_T *rubrica, Elemento_T elemento) 
     {
      int i,j;
      
      i=0;
     // Dove va inserito il nuovo nominativo?
      while (i < rubrica->n_nominativi)
          {
		   if (strcasecmp(rubrica->nominativi[i].nome,elemento.nome) >0) break;
		   i++;
	       }
	  
	  //Trasla il database copiando i puntatori
	  for (j=rubrica->n_nominativi;j>i;j--)
        {
		 rubrica->nominativi[j].nome=rubrica->nominativi[j-1].nome;
		 rubrica->nominativi[j].note=rubrica->nominativi[j-1].note;
		 rubrica->nominativi[j].numero=rubrica->nominativi[j-1].numero;
	     rubrica->nominativi[j].index=j;
         }
	  
	  // Alloca memoria per un nuovo nome
      rubrica->nominativi[i].nome=malloc(MAX_NAME_CHAR);
      rubrica->nominativi[i].note=malloc(MAX_NOTE_CHAR);
      rubrica->nominativi[i].numero=malloc(MAX_PHON_CHAR);
      
      // copia il nuovo elemento nella posizione
	  strcpy(rubrica->nominativi[i].nome,elemento.nome);
	  strcpy(rubrica->nominativi[i].numero,elemento.numero);
	  strcpy(rubrica->nominativi[i].note,elemento.note);  
      rubrica->nominativi[i].index=i;
      rubrica->n_nominativi++;
      
      }
/*
 * Function : Cancella_Record
 * Usage    : Cancella_Record(&rubrica,index);
 * -------------------------------------------------------------
 * 
 * Cancella il recondo con indice 'index' dal database 'rubrica'
 * Prima dealloca la memoria, poi ricompatta il database verso
 * sinistra, aggiorna il campo index e decrementa il numero di
 * records presenti.
 */
 static void Cancella_Record(Rubrica_T *rubrica,int index)
      {
       int i;
          
       // Deallochiamo la memoria del record da cancellare
       free (rubrica->nominativi[index].nome);   
	   free (rubrica->nominativi[index].numero);
	   free (rubrica->nominativi[index].note);
      
       // Shift del database
       rubrica->n_nominativi--;
       for (i=index;i<rubrica->n_nominativi;i++)
         {
		  rubrica->nominativi[i].nome=rubrica->nominativi[i+1].nome;
		  rubrica->nominativi[i].numero=rubrica->nominativi[i+1].numero;
		  rubrica->nominativi[i].note=rubrica->nominativi[i+1].note;
	      rubrica->nominativi[i].index=i;
          }
       return;
       }



/*
 * Function : Ordina_Rubrica
 * Usage    : Ordina_Rubrica(&rubrica);
 * ------------------------------------------------------------
 * Funzione per ordinare la rubrica.
 * Per ottimizzare, l'algoritmo di sorting viene eseguito solo
 * se la rubrica non e' ordinata
 */
 static void Ordina_Rubrica(Rubrica_T *rubrica)
      {
       bool Sorted;
       char *temp1,*temp2,*temp3;
       int i,j;
       
       Sorted=TRUE;
       i=0;
       
       while (rubrica->n_nominativi>1 && i<rubrica->n_nominativi-1)
           {
            if (strcasecmp(rubrica->nominativi[i].nome,rubrica->nominativi[i+1].nome)>0)
             {
              Sorted=FALSE;
              break;
              }
            i++;
            }
       
       if (!Sorted)
        {
         for (i=0;i<rubrica->n_nominativi;i++)
           {
            for (j=i;j<rubrica->n_nominativi;j++)
              {
               if (strcasecmp(rubrica->nominativi[i].nome,rubrica->nominativi[j].nome)>0)
                {
                 temp1=rubrica->nominativi[i].nome;
                 temp2=rubrica->nominativi[i].numero;
                 temp3=rubrica->nominativi[i].note;
                 
                 rubrica->nominativi[i].nome=rubrica->nominativi[j].nome;
                 rubrica->nominativi[i].numero=rubrica->nominativi[j].numero;
                 rubrica->nominativi[i].note=rubrica->nominativi[j].note;
                 
                 rubrica->nominativi[j].nome=temp1;
                 rubrica->nominativi[j].numero=temp2;
                 rubrica->nominativi[j].note=temp3;
                 }
               }
            }
          }       
                 
       Write_File(rubrica);
       }
/* 
 * Function : input_string 
 * Usage    : input_string (line,n);
 * -----------------------------------------------
 * This function ask the user for a string, and store
 * it in line[]. The '\n' character is discarged and a '\0'
 * is added to cbrica(orrectly terminate the string. If the user 
 * enters more than n character, the function signals the 
 * error and abort the program.
 */
static void Input_String (char line[],int n)  
     {
	  int i=-1;
	  char ch=0;
	  
	  while (ch!='\n')
	      {
		   i++;
		   if(i>=n) 
		    {
			 printf("Error in function input_string: Too many characters");
			 exit(1);
		     }
		   ch=getchar();
		   line[i]=ch;
	       }
	  line[i]='\0';
      }     

/* 
 * Function : Input_Integer
 * Usage    : int = Input_Integer();
 * ----------------------------------------------
 * This functions,asks the user for a string, using the 
 * input_string function, validate it as a string of decimal
 * digits, and returns the converted integer value. 
 * The function can be easily adapted to convert a string
 * passed as parameter.
 */
static int Input_Integer(void)
     {
	  char temp[255];
	  int lenght,total,index,t,i;
	 
	  /* Ask the user for a string, and, while determining
	   * its lenght, validate it as only digits string.
	   * If not, display an error and loop. If yes the 
	   * lenght of the string is stored in 'lenght'.
	   * A preliminary check is made for a '-' in front of
	   * the digits.In this case the conversion is made starting
	   * from the [1] element and the result is made negative.
	   */
	  
	  lenght=-1;
	  
	  while (lenght==-1)
	      {
		   index=0;  
		   lenght++;
		   Input_String(temp,7);
		   while (lenght>=0&&temp[lenght]!='\0')
		       {
				if (lenght==0&&temp[lenght]=='-')
				 { 
				  lenght++;
				  index=1;
			      }
				if (temp[lenght]>='0'&&temp[lenght]<='9')
				 {
				  lenght++;
			      } else
			           {
						lenght=-1;
					    }
				}
			if (lenght==-1)
			 {
			  printf("Not ant integer. Retry : ");
		      }
		    }  		     
	
	/* String conversion:
	 * For each digit, depending from the position, a cycle of multiply
	 * is made, and added to total.
	 * 'lenght' keep the value of most significant digit, while index
	 * travel between digits. Thus the position value of every digit
	 * is "lenght-index" position. 
	 */ 	
	total=0;
	for (;index<lenght;index++)
	  {
	   t=temp[index]-'0';
	   // A simple power of 10 cicle.
	   for (i=0;i<lenght-index-1;i++)
	     {
		  t*=10;
	      }
	   total+=t;
       }   
		
	// The integer is negative?
	if (temp[0]=='-')total=-total;
	return (total);
	} 



/*
 * Function : Strip_Spaces
 * Usage    : Strip_Spaces (Stringa[]);
 * -----------------------------------------------------
 * Strtips spaces from 'Stringa'.
 * As example : "La Casa" -> "LaCasa"
 */      
static void Strip_Spaces(char stringa[])
     {
	  char temp[40];
	  int i,j;
	
	  i=0;
	  j=0;
	  while (stringa[i]!='\0')
	      {
		   if (isalpha(stringa[i]))
		    {
		     temp[j]=stringa[i];
		     j++;
	         }
	       i++;
	       }
	  temp[j]='\0';
	  strcpy(stringa,temp);
      }

/*
 * Function : FindString
 * Usage : if (FindString(text,str))......;
 * ---------------------------------------------------
 * Questa funzione ritorna TRUE se la stringa 'sub' e' 
 * una sottostringa di 'string'
 */
static bool FindString (char string[], char sub[] )
     {
	  /*
	  char *temp;
	  bool found=FALSE;
	  
	  temp=strstr(string,sub);
	  
	  if (NULL!=temp)
       {
	    found=TRUE;     	 
        }
	    	  
	  return (found);
	  */
	  return (strstr(string,sub)!=NULL)?TRUE:FALSE;
	  }

/*
 * Function : ToLower
 * Usage : ToLower(string);
 * ---------------------------------------------------
 * This function converts 'string' in lower case chars
 */
static void ToLower(char string[]) 
     {
	  int i;
	  
	  i=0;
	  
	  while (string[i]!='\0')
	      {
		   string[i]=tolower(string[i]);
		   i++;
	       }
	   }         


/*
 * Function : YesOrNo
 * Usage : If (YesOrNo) {.....
 * ---------------------------------------------------
 * Ask user for Y or N, and return TRUE if the answer is Y
 * False otherwise. The function is case insensitive, and
 * check only for the first char of the entered text, discarding
 * all the others chars.
 */
static bool YesOrNo(void)
     {
	  bool answer;
	  char temp[10];
	  
	  answer=TRUE;
	  temp[0]='@';
	  
	  while (TRUE)
	      {
	       fgets(temp,8,stdin);
	       temp[0]=tolower(temp[0]);
	       if (temp[0]=='y' || temp[0]=='n') break;
	       printf ("Devi rispondere Y oppure N. ? "); 
	       }
      
      if (temp[0]=='n') answer=FALSE;
      
      return (answer);
      }	  
	  
	  
