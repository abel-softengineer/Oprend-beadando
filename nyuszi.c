
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    
#include <sys/wait.h>
#include <time.h> 

#define MAX_NAME 100
#define MAX_POEM 256
#define MAX_EGG 64
#define FILENAME "nyuszik.txt"

//Nyuszi struct letrehozasa

typedef struct {
    char name[MAX_NAME];
    char poem[MAX_POEM];
    int eggs;
} Rabbit;


//Fuggvenyek elodeklarasa

void add_rabbit();
void list_rabbits();
void modify_rabbit();
void delete_rabbit();
void rabbit_helper(char* line, Rabbit* r);
void start_loop();
void save_to_file(Rabbit rabbits[], int count);
int main() {
    int choice;
    while (1) {
        printf("\n-- Easter Bunny King Competition --\n");
        printf("1. New rabbit registration\n");
        printf("2. List of registered rabbits\n");
        printf("3. Modify rabbit details\n");
        printf("4. Delete rabbit\n");
        printf("5. Start The Loop!\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar(); // az enter elnyerese erdekeben

        switch (choice) {
            case 1: add_rabbit(); break;
            case 2: list_rabbits(); break;
            case 3: modify_rabbit(); break;
            case 4: delete_rabbit(); break;
            case 5: start_loop(); break;
            case 0: exit(0);
            default: printf("Not a valid choice!\n");
        }
    }
    return 0;
}

void add_rabbit() {
    FILE *file = fopen(FILENAME, "a");
    if (!file) {
        perror("ERROR : Cannot open file!");
        return;
    }

    Rabbit r; //Letrehozunk egy nyuszit, amibe beleirjuk az informaciokat

    //strcspn-el levaguk a vegere bekerulo karaktert. fgetsel beallitjuk
    //fgets beallitja a reszeit

    printf("Name of the rabbit : ");
    fgets(r.name, MAX_NAME, stdin);
    r.name[strcspn(r.name, "\n")] = 0;

    printf("Write the poem: ");
    fgets(r.poem, MAX_POEM, stdin);
    r.poem[strcspn(r.poem, "\n")] = 0;

    r.eggs = 0;

    fprintf(file, "%s|%s|%d\n", r.name, r.poem, r.eggs);
    fclose(file);
    printf("Rabbit successfully added!\n");
}

void list_rabbits() {

    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        perror("ERROR : Cannot read file!");
        return;
    }

    Rabbit r;
    int index = 1;
    char line[512];

    printf("\n-- Jelentkezett nyuszik --\n");

    while (fgets(line, sizeof(line), file)) {
        
        rabbit_helper(line,&r);

        printf("%d. %s - %d tojás   Vers: %s\n", index++, r.name, r.eggs, r.poem);
    }
    fclose(file);
}

void modify_rabbit() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        perror("ERROR : Cannot open file!");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        perror("ERROR : Cannot create temporary file!");
        fclose(file);
        return;
    }

    int index;
    int i = 1;

    list_rabbits();
    printf("Which rabbit do you want to modify? (index)");
    scanf("%d", &index);
    getchar();

    Rabbit r;
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        
        //rabbit helper megkapja a lineokat es beallitja a nyuszi reszeit
        rabbit_helper(line,&r);
    
        if(i == index)
        {
            printf("New egg number: ");
            scanf("%d", &r.eggs); 
            getchar(); 

            printf("New poem: ");
            fgets(r.poem, MAX_POEM, stdin); 
            r.poem[strcspn(r.poem, "\n")] = 0;
        
            printf("Successful modification!\n");
        }
        fprintf(temp, "%s|%s|%d\n", r.name, r.poem, r.eggs);
        ++i;
    }


    //Bezarjuk a fileokat es a tempet atnevezzuk
    fclose(file);
    fclose(temp);

    
    remove(FILENAME);
    rename("temp.txt", FILENAME);


}

void delete_rabbit() {

    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        perror("ERROR : Cannot open file!");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        perror("ERROR : Cannot create temporary file!");
        fclose(file);
        return;
    }

    Rabbit r;
    int index;
    int i = 0;

    list_rabbits();
    printf("Which index rabbit do you want to delete?");
    
    scanf("%d", &index);
    getchar();

    char line[512]; //Biztos nem lesz egy sor ilyen hosszu


    while (fgets(line, sizeof(line), file)) {
        rabbit_helper(line,&r);

        //A nyuszit kihagyjuk amelyiket torolni kellene 
        if (++i != index) {
            fprintf(temp, "%s|%s|%d\n", r.name, r.poem, r.eggs);
        } else {
            printf("Rabbit successfully deleted!\n");
        }
        ++i;
    }

    fclose(file);
    fclose(temp);

    
    remove(FILENAME);
    rename("temp.txt", FILENAME);
}

void rabbit_helper(char* line, Rabbit* r) {

    if (!line || !r)
    {
        printf("Error occured in Rabbit reading!");
        return;
    }

    //strtok mindig feldarabolja az adott sort a | -k menten


    char *part = strtok(line, "|");
    if (part) {
        strncpy(r->name, part, MAX_NAME);
    }
    //Itt most a part az a nev, az elso | karakterig ment el

    part = strtok(NULL, "|");
    if (part) {
        strncpy(r->poem, part, MAX_POEM);
    }

    part = strtok(NULL, "|");
    if (part) {
        r->eggs = atoi(part);  
    }

}


void start_loop()
{

    #define pt(fstr, ...) {printf("[%s] "fstr"\n", whoami ?: "N/A", ##__VA_ARGS__);}
    char* whoami = 0;


        FILE *file = fopen(FILENAME, "r");
        if (!file) {
            perror("HIBA: NEM LEHEt A FILET MEGNYITNI!");
            return;
        }
    

        Rabbit rabbits[100];
        int count = 0;
        char line[512];
        srand(time(NULL));  

        while (fgets(line, sizeof(line), file) && count < 100) {
            rabbit_helper(line, &rabbits[count]);
            count++;
        }

        fclose(file);
    
        int pipes[count][2]; 
        for(int i = 0; i < count; i++)
        {
            pipe(pipes[i]);
        }


        pid_t gy1 = fork();

        if(gy1)
        {

            whoami = "Fonyuszi";
            pt("Megerkeztem");

            int max_eggs = -1;
            int winner = -1;
            
            for (int i = 0; i < count; ++i) {
                int eggs = 0;
                read(pipes[i][0], &eggs, sizeof(int));
                close(pipes[i][1]);
                close(pipes[i][0]);
            
                rabbits[i].eggs = eggs;
            
                if (eggs > max_eggs) {
                    max_eggs = eggs;
                    winner = i;
                }
            }

            save_to_file(rabbits, count);
        
            if (winner != -1) {
                pt("🥚🐰 A Húsvéti Locsolókirály: %s lett, %d tojással! 🐰🥚",
                rabbits[winner].name, rabbits[winner].eggs);
            } else {
                pt("Nincsenek résztvevők!");
            }
        
            pt("Kilepes");
            waitpid(gy1, 0, 0); 

        } else
        {
            whoami = "gyerek";
            

            for (int i = 0; i < count; ++i) 
            {
                close(pipes[i][0]);
        
                pt("Nyuszi fiú [%s] megérkezett locsolni!", rabbits[i].name);
                pt("Versike: %s", rabbits[i].poem);
    
                int earned_eggs = rand() % 20 + 1;
        
                pt("%s %d tojást kapott!", rabbits[i].name, earned_eggs);
        
                write(pipes[i][1], &earned_eggs, sizeof(int));
                close(pipes[i][1]);

                pt("Kilepes: %s", rabbits[i].name);
            }
            pt("Osszes nyuszi kilep!");
            exit(0);
        }



        

    }

void save_to_file(Rabbit rabbits[], int count)
{
    FILE *fout = fopen(FILENAME, "w");
    if (!fout) {
        perror("HIBA: NEM LEHET A FILEBA IRNI!");
        return;
    }

    for (int i = 0; i < count; ++i) {
        fprintf(fout, "%s|%s|%d\n", rabbits[i].name, rabbits[i].poem, rabbits[i].eggs);
    }

    fclose(fout);

}








//Forrasok

//https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input?utm_source=chatgpt.com
//strcspn

//https://cplusplus.com/reference/cstring/strcpy/
//strncpy

//strtok
//https://cplusplus.com/reference/cstring/strtok/