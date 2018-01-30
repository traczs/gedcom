#include <ctype.h>
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"

void upperCase(char** word);

void upperCase(char** word)
{

  for(int i =0;i<strlen(*word);i++)
  {
    (*word)[i] = toupper((*word)[i]);
  }
  return;
}

GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj)
{
  GEDCOMerror error;

  if (fileName==NULL)
  {
    *obj = NULL;
    error.type = INV_FILE;
    error.line = -1;
    return error;
  }
  //invalid file error checking
  FILE* file = fopen(fileName,"r");
  char* extention = strchr(fileName,'.');
  //file doesnt exist or is empty
  if(file == NULL || (strcmp(fileName,"")==0) )
  {
    *obj = NULL;
    error.type = INV_FILE;
    error.line = -1;
    return error;
  }
  if(extention != NULL)
  {
    //check for file extention
    if(strncmp(extention,".ged",4) != 0)
    {
      fclose(file);
      *obj = NULL;
      error.type = INV_FILE;
      error.line = -1;
      return error;
    }
  }
  else
  {
    fclose(file);
    *obj = NULL;
    error.type = INV_FILE;
    error.line = -1;
    return error;
  }
  if(file != NULL)
  {
    fseek(file,0,SEEK_END);
    int size = ftell(file);
    if(size == 0)
    {
      fclose(file);
      *obj = NULL;
      error.type = INV_FILE;
      error.line = -1;
      return error;
    }
  }


  fseek(file,0,SEEK_SET);
  char string[256] = "";
  char* token[256];
  char* wholeList[500][256];
  for(int i =0 ; i<500;i++)
  {
    for(int j =0; j<256;j++)
    {
      wholeList[i][j] = malloc(sizeof(char)*50);
      strcpy(wholeList[i][j] , "\0");
    }
  }
  int listCount = 0;
  char delim[] = " \n\r\t\v\f";
  bool headCheck = false;
  bool trlrCheck = false;
  char* tempStr;

  while(fgets(string,strlen(string),file)!=NULL)
  {
    if(strlen(string)> 256)
    {
      fclose(file);
      *obj = NULL;
      error.type = INV_GEDCOM;
      error.line = -1;
      return error;
    }
    if(strcmp(string,"\n")==0){
      continue;
    }
    int tokNum=0;
    token[tokNum] = strtok(string,delim);
    strcpy(wholeList[listCount][tokNum],token[tokNum]);
    tokNum++;

    while((token[tokNum] = strtok(NULL,delim)) != NULL)
    {
      if(tokNum==1)
      {
        upperCase(&(token[tokNum]));
      }
      strcpy(wholeList[listCount][tokNum],token[tokNum]);

      //printf("wholeList[%d][%d]= %s\n",listCount,tokNum,wholeList[listCount][tokNum]);
      tokNum++;
      //token[tokNum] = wholeList[listCount][tokNum];
    }

    if(token[0]!=NULL)
    {
      if(strcmp(token[0],"0")==0)
      {
        if(token[1] != NULL)
        {
          if(strcmp(token[1],"HEAD") == 0)
          {
            headCheck = true;
          }
          if(strcmp(token[1],"TRLR") == 0)
          {
            trlrCheck = true;
          }
        }
        else
        {
          if(headCheck != true)
          {
            fclose(file);
            *obj = NULL;
            error.type = INV_GEDCOM;
            error.line = -1;
            return error;
          }
        }
      }
      else
      {
        if(headCheck != true)
        {
          fclose(file);
          *obj = NULL;
          error.type = INV_GEDCOM;
          error.line = -1;
          return error;
        }
      }
    }
    listCount++;
  }

  bool headerChk,sourChk,versChk, submChk,gedcChk,formChk,charChk;
  headerChk = sourChk = versChk = submChk = gedcChk = formChk = charChk = false;
  for(int i =0; i<listCount;i++)
  {
    int j=0;
    while ((strcmp(wholeList[i][j],"0" )!=0) && (strcmp(wholeList[i][j],"" )!=0))
    {
      if(strcmp(wholeList[i][j+1],"HEAD")==0)
        headerChk = true;
      else if(strcmp(wholeList[i][j+1],"SOUR")==0)
        sourChk = true;
      else if(strcmp(wholeList[i][j+1],"VERS")==0)
        versChk = true;
      else if(strcmp(wholeList[i][j+1],"SUBM")==0)
        submChk = true;
      else if(strcmp(wholeList[i][j+1],"GEDC")==0)
        gedcChk = true;
      else if(strcmp(wholeList[i][j+1],"FORM")==0)
        formChk = true;
      else if(strcmp(wholeList[i][j+1],"CHAR")==0)
        charChk = true;
    }
  }
  if(headerChk==true && sourChk ==true && versChk ==true && submChk== true && gedcChk==true && formChk==true && charChk==true)
  {
    //TODO: make the header
    Header* header = malloc(sizeof(Header));
    

  }

  for(int i =0; i<listCount;i++)
  {
    int j = 0;
    while (strcmp(wholeList[i][j],"\0" )!=0)
    {
      printf("MYLIST[%d][%d]= %s\n",i,j,wholeList[i][j]);
      j++;
    }
  }




  fclose(file);
  error.type = OK;
  error.line = -1;
  return error;
}
