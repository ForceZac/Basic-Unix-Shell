/*Basic shell program*/
/* do stuff */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>



//Exit Function
int exitFunction() {
  exit(0);
 }

//Change path function
char* pathFunction(char *pathTemp, char *path) {

  path = pathTemp +5;//account for path+space
  printf("%s",path);

 return path;
 }//End path function

/*/Change Directory function
void changeDirectory(char* input) {
  chdir(input);
  printf("Directory changed\n");
}*/



int main(int argc, char* argv[])
{
  char error_message[30]="An error has ocured\n";
  int exitFlag =1;
  char *string;
  size_t bufsize=256;
  size_t characters;//Actual string size
  char  *token;//pointer to tokenized string location
  char   *pathTemp;
  char *path="/bin";
 // File *fptr;//File pointer
  char *wDirectory;
  char *wBuffer;
  wDirectory=getcwd(wBuffer,bufsize);
  printf("%s\n",wDirectory);
  //---BATCH MODE---//
if(argc>0)
{

}
else if(argc>1)//More than 1 file supplied
{
  exit(1);
}









//Get input from the user---Start Loop--
 //--INTERACTIVE MODE--
  while(exitFlag==1) {
  printf("grsh> ");
  string = (char *)malloc(bufsize * sizeof(char));
  characters=getline(&string, &bufsize, stdin);
   //Copy string contents to new variable
  strcpy(pathTemp,string);

//BUILT IN COMMANDS

 //If user types exit
  if(strcmp(string,"exit\n")==0)
        {
          exitFunction();
        }


//END OF BUILT INS
  token=strtok(string,"/");//Begin tokenizer--First token-
  if(strcmp(token,"path ")==0)
        {

          pathFunction(pathTemp,path);
        }
 //WORK IN PROGRESS
 /* if(strcmp(token,"cd ")==0 || strcmp(token,"cd\n")==0)
        {
           token=strtok(NULL," ");
           changeDirectory(token);
 wDirectory=getcwd(wBuffer,bufsize);
           printf("%s\n",wDirectory);
        }*/
  while(token !=NULL)
        {
          token=strtok(NULL,"/");//tokenize rest of string
        }

 }//End of while loop
//End  Program
return 0;
}           
