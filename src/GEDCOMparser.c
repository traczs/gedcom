#include <ctype.h>
#include "LinkedListAPI.h"
#include "GEDCOMparser.h"

//struct to hold the pointers of the individuals
typedef struct {
  char id[25];
  void * pointer;
} Point;

//function declerations
void upperCase(char** word);
void deletePoint(void* toBeDeleted);
int comparePoints(const void* first,const void* second);
char* printPoint(void* toBePrinted);
bool compareIndividualsBool(const void* first,const void* second);
void getDescendantListRecursion(const GEDCOMobject* familyRecord,List* generations, List people, unsigned int maxGen);
void getAncestorListRecursion(const GEDCOMobject* familyRecord,List* generations, List people, unsigned int maxGen);

//takes in a word and turns it to all capitals(for the gedcom tags)
void upperCase(char** word)
{

  for(int i =0;i<strlen(*word);i++)
  {
    (*word)[i] = toupper((*word)[i]);
  }
  return;
}

/*
* createGEDCOM
* parses the file and puts it into the gedcom objects
* returns an error struct with an error enum and line number
*/
GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj)
{
  GEDCOMerror error;
  (*obj) = malloc(sizeof(GEDCOMobject));

  if (fileName==NULL)
  {
    free(*obj);
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
    free(*obj);
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
      free(*obj);
      fclose(file);
      *obj = NULL;
      error.type = INV_FILE;
      error.line = -1;
      return error;
    }
  }
  else
  {
    free(*obj);
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
      free(*obj);
      fclose(file);
      *obj = NULL;
      error.type = INV_FILE;
      error.line = -1;
      return error;
    }
  }


  fseek(file,0,SEEK_SET);
  char string[256] = "";
  //char* token[256];
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

  while(fgets(string,500,file)!=NULL)
  {
    if(strlen(string)> 255)
    {
      free(*obj);
      fclose(file);
      *obj = NULL;
      error.type = INV_RECORD;
      error.line = listCount+1;
      return error;
    }
    if(strcmp(string,"\n")==0){
      continue;
    }
    int tokNum=0;
    tempStr = strtok(string,delim);
    strcpy(wholeList[listCount][tokNum],tempStr);
    tokNum++;

    while((tempStr = strtok(NULL,delim)) != NULL)
    {
      if(tokNum==1)
      {
        upperCase(&(tempStr));
      }
      strcpy(wholeList[listCount][tokNum],tempStr);

      //printf("wholeList[%d][%d]= %s\n",listCount,tokNum,wholeList[listCount][tokNum]);
      tokNum++;
      //token[tokNum] = wholeList[listCount][tokNum];
    }
    listCount++;
  }

  int temp = 0;
  for(int i =1; i<listCount;i++)
  {
    int diff = (atoi(wholeList[i][0]) - temp) ;
    if (diff>1 || diff<-3)
    {
      free(*obj);
      fclose(file);
      *obj = NULL;
      error.type = INV_RECORD;
      error.line = i+1;
      return error;
    }
    temp = atoi(wholeList[i][0]);
  }

//checking if head and trlr exist
  if(wholeList[0][0]!=NULL /*&& (strcmp(wholeList[0][0],"0")==0)*/)
  {
      if(wholeList[0][1] != NULL)
      {
        if(strcmp(wholeList[0][1],"HEAD") == 0)
        {
          headCheck = true;
          if(strcmp(wholeList[0][0],"0")!=0)
          {
            free(*obj);
            fclose(file);
            *obj = NULL;
            error.type = INV_HEADER;
            error.line = -1;
            return error;
          }
        }
      }
  }
  if(wholeList[listCount-1][0]!=NULL && (strcmp(wholeList[listCount-1][0],"0")==0))
  {
      if(wholeList[listCount-1][1] != NULL)
      {
        if(strcmp(wholeList[listCount-1][1],"TRLR") == 0)
        {
          trlrCheck = true;
        }
      }
  }


  if(headCheck==false || trlrCheck==false)
  {
    free(*obj);
    fclose(file);
    *obj = NULL;
    error.type = INV_GEDCOM;
    error.line = -1;
    return error;
  }

//header error checking
  bool sourChk,versChk, submChk,gedcChk,formChk,charChk;
  sourChk = versChk = submChk = gedcChk = formChk = charChk = false;
  for(int i =1; i<listCount;i++)
  {
    int j=0;

      if(strcmp(wholeList[i][j+1],"SOUR")==0)
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
  if(sourChk == false||versChk == false||submChk == false||gedcChk == false||formChk == false||charChk == false)
  {
    free(*obj);
    fclose(file);
    *obj = NULL;
    error.type = INV_HEADER;
    error.line = -1;
    return error;
  }
  //header parsing into data structure
  bool submitterCheck = false;
  if(sourChk ==true && versChk ==true && submChk== true && gedcChk==true && formChk==true && charChk==true)
  {
    (*obj)->header =  malloc(sizeof(Header));
    int j=0;
    int i = 1;
    (*obj)->submitter = malloc(sizeof(Submitter)/*+20* sizeof(char)*/);
    (*obj)->header->submitter = malloc(sizeof(Submitter));
    (*obj)->header->submitter->otherFields = initializeList(&printField,&deleteField,&compareFields);
    (*obj)->submitter->otherFields = initializeList(&printField,&deleteField,&compareFields);
    (*obj)->header->otherFields = initializeList(&printField,&deleteField,&compareFields);

    while ((strcmp(wholeList[i][j],"0" )!=0) && (wholeList[i][j]!=NULL))
    {
      if(strcmp(wholeList[i][j+1],"SOUR")==0)
      {
        if(wholeList[i][j+2] != NULL && strcmp(wholeList[i][j+2],"")!=0)
          strcpy((*obj)->header->source, wholeList[i][j+2]);
        while(strcmp(wholeList[i][0],"1" )!=0 && wholeList[i][0] !=NULL)
        {
          i++;
        }
      }
      else if((strcmp(wholeList[i][j+1],"GEDC")==0) /*&& (strcmp(wholeList[i+1][j+1],"VERS" )==0)*/)
      {
        i++;
        while(strcmp(wholeList[i][0],"1" )!=0 && wholeList[i][0] !=NULL)
        {
          if(strcmp(wholeList[i][1],"VERS" )==0)
          {
            (*obj)->header->gedcVersion = atof(wholeList[i][j+2]);
          }
          i++;
        }
        i--;
      }
      else if((strcmp(wholeList[i][j+1],"CHAR")==0))
      {
        if(wholeList[i][j+2] != NULL)
        {

          if(strcmp(wholeList[i][j+2],"ANSEL")==0)
          {
            (*obj)->header->encoding = ANSEL;
          }
          else if(strcmp(wholeList[i][j+2],"UTF8")==0 || strcmp(wholeList[i][j+2],"UTF-8")==0)
          {
            (*obj)->header->encoding = UTF8;
          }
          else if(strcmp(wholeList[i][j+2],"UNICODE")==0)
          {
            (*obj)->header->encoding = UNICODE;
          }
          else if(strcmp(wholeList[i][j+2],"ASCII")==0)
          {
            (*obj)->header->encoding = ASCII;
          }
        }
      }
      else if((strcmp(wholeList[i][j+1],"SUBM")==0))
      {
        if(wholeList[i][j+2] != NULL)
        {

          bool isAddr = false;
          strcpy((*obj)->submitter->address ,wholeList[i][j+2]);
          strcpy((*obj)->header->submitter->address ,wholeList[i][j+2]);

          for(int i =1; i<listCount;i++)
          {
            if((strcmp(wholeList[i][1],(*obj)->submitter->address)==0))
            {
              i++;
              submitterCheck = true;
              while ((strcmp(wholeList[i][0],"0" )!=0) && (wholeList[i][0]!=NULL))
              {
                if(strcmp(wholeList[i][1],"NAME")==0)
                {
                  strcpy((*obj)->submitter->submitterName,wholeList[i][2]);
                  strcpy((*obj)->header->submitter->submitterName,wholeList[i][2]);
                }
                else if(strcmp(wholeList[i][1],"ADDR")==0)
                {
                  isAddr = true;
                  strcpy((*obj)->submitter->address,wholeList[i][2]);
                  strcpy((*obj)->header->submitter->address,wholeList[i][2]);
                }
                else
                {
                  Field* field = malloc(sizeof(Field));
                  field->tag = malloc(sizeof(char) * 10);
                  field->value = malloc(sizeof(char) * 100);
                  if((wholeList[i][1] != NULL)&& (strcmp(wholeList[i][1],"" )!=0))
                    strcpy(field->tag, wholeList[i][1]);
                    if((wholeList[i][2] != NULL)&& (strcmp(wholeList[i][2],"" )!=0))
                    {
                      strcpy(field->value, wholeList[i][2]);
                      int w = 3;
                      while ((wholeList[i][w] != NULL)&& (strcmp(wholeList[i][w],"" )!=0)) {
                        strcat(field->value, " ");
                        strcat(field->value, wholeList[i][w]);
                        w++;
                      }
                    }
                  insertBack(&((*obj)->submitter->otherFields),field);
                  insertBack(&((*obj)->header->submitter->otherFields),field);

                  //Field* temp = (Field*)(*obj)->header->submitter->otherFields.tail->data;
                  //printf("woo sus:%s \n",temp->tag);
                }
                //(*obj)->header->submitter = (*obj)->submitter;
                i++;
              }
            }
          }
          if(isAddr ==false)
          {
            strcpy((*obj)->submitter->address ,"");
            strcpy((*obj)->header->submitter->address ,"");
          }
        }
      }
      else
      {
        if(wholeList[i][j] != NULL && (strcmp(wholeList[i][0],"\0" )!=0))
        {
          Field* field = malloc(sizeof(Field));
          field->tag = malloc(sizeof(char) * 10);
          field->value = malloc(sizeof(char) * 100);
          if((wholeList[i][1] != NULL)&& (strcmp(wholeList[i][1],"" )!=0))
            strcpy(field->tag, wholeList[i][1]);
          if((wholeList[i][2] != NULL)&& (strcmp(wholeList[i][2],"" )!=0))
          {
            strcpy(field->value, wholeList[i][2]);
            int w = 3;
            while ((wholeList[i][w] != NULL)&& (strcmp(wholeList[i][w],"" )!=0)) {
              strcat(field->value, " ");
              strcat(field->value, wholeList[i][w]);
              w++;
            }
          }

          insertBack(&((*obj)->header->otherFields),field);

          //Field* temp = (Field*)(*obj)->header->otherFields.tail->data;
          //printf("woo sus:%s \n",temp->tag);
        }
      }
      i++;
    }
  }

  if(submitterCheck == false)
  {
    free(*obj);
    fclose(file);
    *obj = NULL;
    error.type = INV_GEDCOM;
    error.line = -1;
    return error;
  }
//mallocing and initializing things


  (*obj)->families = initializeList(&printFamily,&deleteFamily,&compareFamilies);
  List points= initializeList(&printPoint,&deletePoint,&comparePoints);
  (*obj)->individuals = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);

  //individual and family parsing
  for (int i = 0; i < listCount; i++)
  {
    if(strcmp(wholeList[i][0],"0")==0 && strcmp(wholeList[i][2],"INDI")==0)
    {
      Individual* indi = malloc(sizeof(Individual));
      indi->givenName = malloc(sizeof(char) * 50);
      indi->surname = malloc(sizeof(char) *  50);
      strcpy(indi->givenName,"");
      strcpy(indi->surname,"");
      indi->events = initializeList(&printEvent,&deleteEvent,&compareEvents);
      indi->families = initializeList(&printFamily,&deleteFamily,&compareFamilies);
      indi->otherFields = initializeList(&printField,&deleteField,&compareFields);
      //TODO: STORE THE POINTER INTO SOMETHING AND POINT IT SOMEWHERE
      Point* point = malloc(sizeof(Point));
      strcpy(point->id,wholeList[i][1]);

      i++;
      while ((strcmp(wholeList[i][0],"0" )!=0) && (wholeList[i][0]!=NULL) && (strcmp(wholeList[i][0],"\0" )!=0))
      {
        if((strcmp(wholeList[i][1],"NAME" )==0))
        {
          int j =2;
          char name[75] = "";
          while(wholeList[i][j] !=NULL && strcmp(wholeList[i][j],"\0" )!=0)
          {
            strcat(name,wholeList[i][j]);
            j++;
          }
          int l = 0;
          int fnCount = 0;
          int lnCount = 0;
          char lastname[50] = "";
          char firstname[50] = "";
          int slashCount = 0;

          while(name[l] != '/' && name[l] != '\0')
          {
            firstname[fnCount] = name[l];
            fnCount++;
            l++;
          }
          if(name[l] == '/')
          {
            l++;
            slashCount++;
          }
          while(name[l] != '/' && name[l] != '\0')
          {
            lastname[lnCount] = name[l];
            lnCount++;
            l++;
          }
          if(name[l] == '/')
          {
            l++;
            slashCount++;
          }
          while(name[l] != '/' && name[l] != '\0')
          {
            firstname[fnCount] = name[l];
            fnCount++;
            l++;
          }
          if(name[l] == '/')
          {
            slashCount++;
          }
          if(slashCount == 2)
          {
            strcpy(indi->givenName,firstname);
            strcpy(indi->surname,lastname);
          }
        }
        else if(strcmp(wholeList[i][2],"\0")==0  && (strcmp(wholeList[i][0],"1")==0))
        {
          Event* event = malloc(sizeof(Event));
          event->date = calloc(25,sizeof(char));
          event->place = calloc(50,sizeof(char));
          event->otherFields = initializeList(&printField,&deleteField,&compareFields);
          strcpy(event->type,wholeList[i][1]);
          i++;

          while((strcmp(wholeList[i][0],"1" )!=0) && (strcmp(wholeList[i][0],"0" )!=0) && (strcmp(wholeList[i][0],"\0" )!=0))
          {
            if((strcmp(wholeList[i][0],"2" )==0) && (strcmp(wholeList[i][1],"DATE" )==0))
            {
              strcpy(event->date,wholeList[i][2]);
              int j=3;
              while(wholeList[i][j] !=NULL && strcmp(wholeList[i][j],"\0" )!=0)
              {
                strcat(event->date," ");
                strcat(event->date,wholeList[i][j]);
                j++;
              }
            }
            else if((strcmp(wholeList[i][0],"2" )==0) && (strcmp(wholeList[i][1],"PLAC" )==0))
            {
              strcpy(event->place,wholeList[i][2]);
              int j=3;
              while(wholeList[i][j] !=NULL && strcmp(wholeList[i][j],"\0" )!=0)
              {
                strcat(event->place," ");
                strcat(event->place,wholeList[i][j]);
                j++;
              }
            }
            else if((strcmp(wholeList[i][0],"2" )==0))
            {
              Field* field = malloc(sizeof(Field));
              field->tag = malloc(sizeof(char) * 10);
              field->value = malloc(sizeof(char) * 100);
              strcpy(field->tag,wholeList[i][1]);
              strcpy(field->value,wholeList[i][2]);
              int j=3;
              while(wholeList[i][j] !=NULL && strcmp(wholeList[i][j],"\0" )!=0)
              {
                strcat(field->value," ");
                strcat(field->value,wholeList[i][j]);
                j++;
              }
              insertBack(&(event->otherFields),field);
            }
            i++;
          }
          i--;
          insertBack(&(indi->events),event);
        }
        else if((strcmp(wholeList[i][0],"1" )==0) && (strcmp(wholeList[i][1],"FAMS" )==0))
        {

        }
        else if((strcmp(wholeList[i][0],"1" )==0) && (strcmp(wholeList[i][1],"FAMC" )==0))
        {

        }
        else
        {
          Field* field = malloc(sizeof(Field));
          field->tag = malloc(sizeof(char) * 10);
          field->value = malloc(sizeof(char) * 100);
          strcpy(field->tag,wholeList[i][1]);
          strcpy(field->value,wholeList[i][2]);
          int j=3;
          while(wholeList[i][j] !=NULL && strcmp(wholeList[i][j],"\0" )!=0)
          {
            strcat(field->value," ");
            strcat(field->value,wholeList[i][j]);
            j++;
          }
          insertBack(&(indi->otherFields),field);

        }
        i++;
      }
      point->pointer = indi;
      insertBack(&((*obj)->individuals),indi);
      insertBack(&(points),point);

      i--;
    }
    else if(strcmp(wholeList[i][0],"0")==0 && strcmp(wholeList[i][2],"FAM")==0)
    {
      Family * fam = malloc(sizeof(Family));
      fam->children = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
      fam->otherFields = initializeList(&printField,&deleteField,&compareFields);
      fam->events = initializeList(&printEvent,&deleteEvent,&compareEvents);
      i++;
      Point* temp = malloc(sizeof(Point));
      temp = NULL;
      while ((strcmp(wholeList[i][0],"0" )!=0) && (wholeList[i][0]!=NULL) && (strcmp(wholeList[i][0],"\0" )!=0))
      {

        if(strcmp(wholeList[i][1],"HUSB" )==0)
        {
          ListIterator iter = createIterator(points);
          //Point* temp = NULL;
        	while( (temp = nextElement(&iter)) != NULL)
          {
            if(strcmp(temp->id,wholeList[i][2])==0)
            {
              fam->husband = temp->pointer;
              //printf("HUSB %s\n",fam->husband->givenName);
              //person = *temp->pointer;
              break;
            }
          }
        }
        else if(strcmp(wholeList[i][1],"WIFE" )==0)
        {

          ListIterator iter = createIterator(points);
          //Point* temp = NULL;
        	while( (temp = nextElement(&iter)) != NULL)
          {
            if(strcmp(temp->id,wholeList[i][2])==0)
            {
              fam->wife = temp->pointer;
              //printf("WIFE %s\n",fam->wife->givenName);
              //person = temp->pointer;
              break;
            }
          }
        }
        else if(strcmp(wholeList[i][1],"CHIL" )==0)
        {
          ListIterator iter = createIterator(points);
          //Point* temp = NULL;
        	while( (temp = nextElement(&iter)) != NULL)
          {
            if(strcmp(temp->id,wholeList[i][2])==0)
            {
              insertBack(&(fam->children) , temp->pointer);
              //Individual* temp1 = fam->children.tail->data;
              //printf("CHILD %s\n",temp1->givenName );
              //person = temp->pointer;
              break;
            }
          }
        }
        else if(strcmp(wholeList[i][2],"\0")==0  && (strcmp(wholeList[i][0],"1")==0)) /*if(strcmp(wholeList[i][1],"MARR" )==0)*/
        {
          Event* famEvent = malloc(sizeof(Event));
          famEvent->date = calloc(25,sizeof(char));
          famEvent->place = calloc(50,sizeof(char));
          famEvent->otherFields = initializeList(&printField,&deleteField,&compareFields);
          strcpy(famEvent->type,wholeList[i][1]);
          i++;

          while((strcmp(wholeList[i][0],"1" )!=0) && (strcmp(wholeList[i][0],"0" )!=0) && (strcmp(wholeList[i][0],"\0" )!=0))
          {
            if((strcmp(wholeList[i][0],"2" )==0) && (strcmp(wholeList[i][1],"DATE" )==0))
            {
              strcpy(famEvent->date,wholeList[i][2]);
              int j=3;
              while(wholeList[i][j] !=NULL && strcmp(wholeList[i][j],"\0" )!=0)
              {
                strcat(famEvent->date," ");
                strcat(famEvent->date,wholeList[i][j]);
                j++;
              }
            }
            else if((strcmp(wholeList[i][0],"2" )==0) && (strcmp(wholeList[i][1],"PLAC" )==0))
            {
              if(strcmp(wholeList[i][2],"\0")!=0)
              {
                strcpy(famEvent->place,wholeList[i][2]);
                int j=3;
                while(wholeList[i][j] !=NULL && strcmp(wholeList[i][j],"\0" )!=0)
                {
                  strcat(famEvent->place," ");
                  strcat(famEvent->place,wholeList[i][j]);
                  j++;
                }
              }
            }
            else if((strcmp(wholeList[i][0],"2" )==0))
            {
              Field* field = malloc(sizeof(Field));
              field->tag = malloc(sizeof(char) * 10);
              field->value = malloc(sizeof(char) * 100);
              strcpy(field->tag,wholeList[i][1]);
              strcpy(field->value,wholeList[i][2]);
              int j=3;
              while(wholeList[i][j] !=NULL && strcmp(wholeList[i][j],"\0" )!=0)
              {
                strcat(field->value," ");
                strcat(field->value,wholeList[i][j]);
                j++;
              }
              insertBack(&(famEvent->otherFields),field);
            }
            i++;
          }
          i--;
          insertBack(&(fam->events),famEvent);

        }
        else
        {
          Field* field = malloc(sizeof(Field));
          field->tag = malloc(sizeof(char) * 10);
          field->value = malloc(sizeof(char) * 100);
          strcpy(field->tag,wholeList[i][1]);
          strcpy(field->value,wholeList[i][2]);
          int j=3;
          while(wholeList[i][j] !=NULL && strcmp(wholeList[i][j],"\0" )!=0)
          {
            strcat(field->value," ");
            strcat(field->value,wholeList[i][j]);
            j++;
          }
          insertBack(&(fam->otherFields),field);
        }
        i++;
        if(temp != NULL){
          Individual * ind = temp->pointer;
          insertBack(&(ind->families),fam);
        }
      }

      insertBack(&((*obj)->families),fam);
      i--;
    }


  }

  clearList(&points);
  for(int i =0 ; i<500;i++)
  {
    for(int j =0; j<256;j++)
    {
      free(wholeList[i][j]);
    }
  }
  fclose(file);
  error.type = OK;
  error.line = -1;
  return error;
}


////////////////////////////////////////////HELPER FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////////
char* printField(void* toBePrinted)
{
  Field * field = (Field *) toBePrinted;
  char* string = malloc(sizeof(char)*265);
  strcpy(string,field->tag);
  strcat(string," ");
  strcat(string, field->value);
  return string;
}

void deleteField(void* toBeDeleted)
{
  Field * field = (Field *) toBeDeleted;
  free(field->tag);
  free(field->value);
  free(field);
}

int compareFields(const void* first,const void* second)
{
  Field* tmpName1;
  Field* tmpName2;

  if (first == NULL || second == NULL){
      return -2;
  }

  tmpName1 = (Field*)first;
  tmpName2 = (Field*)second;

  char one[50];
  char two[50];
  strcpy(one,tmpName1->tag);
  strcat(one," ");
  strcat(one, tmpName1->value);
  strcpy(two,tmpName2->tag);
  strcat(two," ");
  strcat(two, tmpName2->value);

  if (strcmp(one, two) == 0){
    return 0;
  }
  else if (strcmp(one,two) > 0)
  {
      return 1;
  }
  else if (strcmp(one,two) < 0)
  {
      return -1;
  }
  return -2;
}

char* printIndividual(void* toBePrinted)
{
  Individual * indi = (Individual *) toBePrinted;
  char* string = malloc(sizeof(char)*265);
  strcpy(string,indi->givenName);
  strcat(string," ");
  strcat(string, indi->surname);
  return string;
}

void deleteIndividual(void* toBeDeleted)
{
  Individual * indi = (Individual *) toBeDeleted;
  free(indi->givenName);
  free(indi->surname);
  //free(indi->otherFields.head->data.tag);//may not need this?
  //free(indi->otherFields.head->data.value);//may not need this?
  free(indi);
}

int compareIndividuals(const void* first,const void* second)
{
  Individual * tmpName1;
  Individual * tmpName2;

  if (first == NULL || second == NULL){
      return -2;
  }

  tmpName1 = (Individual *)first;
  tmpName2 = (Individual *)second;

  char one[50];
  char two[50];

  strcpy(one,tmpName1->surname);
  strcat(one,",");
  strcat(one,tmpName1->givenName);
  strcpy(two,tmpName2->surname);
  strcat(two,",");
  strcat(two,tmpName2->givenName);
  //Elements are "equal" if their first names are equal
  if (strcmp(one,two) == 0){
    return 0;
  }
  else if (strcmp(one,two) > 0)
  {
      return 1;
  }
  else if (strcmp(one,two) < 0)
  {
      return -1;
  }
  return -2;
}

bool compareIndividualsBool(const void* first,const void* second)
{
  Individual * tmpName1;
  Individual * tmpName2;

  if (first == NULL || second == NULL){
      return false;
  }

  tmpName1 = (Individual *)first;
  tmpName2 = (Individual *)second;

  char one[50];
  char two[50];

  strcpy(one,tmpName1->surname);
  strcat(one,",");
  strcat(one,tmpName1->givenName);
  strcpy(two,tmpName2->surname);
  strcat(two,",");
  strcat(two,tmpName2->givenName);
  //Elements are "equal" if their first names are equal
  if (strcmp(one,two) == 0){
    return true;
  }
  else
  {
      return false;
  }
}

char* printEvent(void* toBePrinted)
{
  Event * event = (Event *) toBePrinted;
  char* string = malloc(sizeof(char)*265);
  strcpy(string,event->type);
  strcat(string," ");
  strcat(string, event->date);
  strcat(string," ");
  strcat(string,event->place);
  return string;
}

void deleteEvent(void* toBeDeleted)
{
  Event * event = (Event *) toBeDeleted;
  free(event->type);
  free(event->date);
  free(event->place);
  free(event);
}

int compareEvents(const void* first,const void* second)
{
  Event * tmpName1;
  Event * tmpName2;

  if (first == NULL || second == NULL){
      return -2;
  }

  tmpName1 = (Event *)first;
  tmpName2 = (Event *)second;

  //Elements are "equal" if their types names are equal
  if (strcmp((char*)tmpName1->type, (char*)tmpName2->type) == 0)
  {
    return 0;
  }
  else if(strcmp((char*)tmpName1->type, (char*)tmpName2->type) > 0)
  {
    return 1;
  }
  else if(strcmp((char*)tmpName1->type, (char*)tmpName2->type) < 0)
  {
    return -1;
  }

  return -2;
}


char* printFamily(void* toBePrinted)
{
  Family * fam = (Family *) toBePrinted;
  char* string = malloc(sizeof(char)*265);
  strcpy(string,printIndividual(fam->wife));
  strcat(string,"\n");
  strcat(string, printIndividual(fam->husband));
  return string;
}

void deleteFamily(void* toBeDeleted)
{
  //Family * fam = (Family *) toBeDeleted;
}

int compareFamilies(const void* first,const void* second)
{
  Family* tmpName1;
  Family * tmpName2;



  tmpName1 = (Family *)first;
  tmpName2 = (Family *)second;

  //Elements are "equal" if their types names are equal
  if (strcmp((char*)printIndividual(tmpName1->wife), (char*)printIndividual(tmpName2->wife)) == 0)
  {
    if (strcmp((char*)printIndividual(tmpName1->husband), (char*)printIndividual(tmpName2->husband)) == 0)
    {
      return 0;
    }
  }
  if(strcmp((char*)printIndividual(tmpName1->wife), (char*)printIndividual(tmpName2->wife)) > 0)
  {
    if (strcmp((char*)printIndividual(tmpName1->husband), (char*)printIndividual(tmpName2->husband)) > 0)
    {
      return 1;
    }
  }
  if(strcmp((char*)printIndividual(tmpName1->wife), (char*)printIndividual(tmpName2->wife)) < 0)
  {
    if (strcmp((char*)printIndividual(tmpName1->husband), (char*)printIndividual(tmpName2->husband)) < 0)
    {
      return -1;
    }
  }

  return -2;
}

char* printPoint(void* toBePrinted)
{
  Point * point = (Point *) toBePrinted;
  char* string = malloc(sizeof(char)*265);
  strcpy(string,point->id);
  return string;
}

void deletePoint(void* toBeDeleted)
{
  Point * point = (Point *) toBeDeleted;
  //free(point->pointer);
  free(point);
}

int comparePoints(const void* first,const void* second)
{
  Point* tmpName1;
  Point* tmpName2;

  if (first == NULL || second == NULL){
      return -2;
  }

  tmpName1 = (Point*)first;
  tmpName2 = (Point*)second;


  if (strcmp((char*)tmpName1->id, (char*)tmpName2->id) == 0){
    return 0;
  }
  else
  {
    return 1;
  }
}
///////////////////////////////////////////////////////end of helper functions ///////////////////////////////////////////

char * printGEDCOM(const GEDCOMobject* obj)
{
  if(obj == NULL)
  {
    return "invalid object";
  }
  char* longestString = malloc(sizeof(char)* 1000000);
  char convertedString[25];
  strcpy(longestString,"HEADER:\n");
  strcat(longestString,obj->header->source);
  strcat(longestString,"__");
  sprintf(convertedString,"%.1f",obj->header->gedcVersion);
  strcat(longestString,convertedString);
  strcat(longestString,"__");
  sprintf(convertedString,"%d",obj->header->encoding);
  strcat(longestString,convertedString);
  strcat(longestString,"\n");
  strcat(longestString,"SUBMITTER:\n");
  strcat(longestString,obj->header->submitter->submitterName);
  strcat(longestString,"__");
  strcat(longestString,obj->header->submitter->address);
  strcat(longestString,"__");
  ListIterator fuck = createIterator(obj->header->otherFields);
  Field* fucktemp = NULL;
  while( (fucktemp = nextElement(&fuck)) != NULL)
  {
    strcat(longestString,"\n");
    strcat(longestString,fucktemp->tag);
    strcat(longestString,"__");
    strcat(longestString,fucktemp->value);
  }


  ListIterator iter = createIterator(obj->individuals);
  Individual* temp = NULL;

  while( (temp = nextElement(&iter)) != NULL)
  {
    strcat(longestString,"\nINDIVIDUAL:\n");
    strcat(longestString,temp->givenName);
    strcat(longestString,"__");
    strcat(longestString,temp->surname);

    ListIterator eventIter = createIterator(temp->events);
    Event* tempEvent = NULL;
    while( (tempEvent = nextElement(&eventIter)) != NULL)
    {
      strcat(longestString,"\n  EVENT:");
      strcat(longestString,tempEvent->type);
      strcat(longestString,"__");
      strcat(longestString,tempEvent->date);
      strcat(longestString,"__");
      strcat(longestString,tempEvent->place);
    }

    ListIterator familiesIter = createIterator(temp->families);
    Family* tempFamily = NULL;
    while( (tempFamily = nextElement(&familiesIter)) != NULL)
    {
      strcat(longestString,"\n  IND FAM:");
      if(tempFamily->wife !=NULL)
        strcat(longestString,tempFamily->wife->givenName);
      strcat(longestString,"__");
      if(tempFamily->husband)
        strcat(longestString,tempFamily->husband->givenName);
    }

    ListIterator other1 = createIterator(temp->otherFields);
    Field* othtemp = NULL;
    while( (othtemp = nextElement(&other1)) != NULL)
    {
      strcat(longestString,"\nIND OTHER:");
      strcat(longestString,othtemp->tag);
      strcat(longestString,"__");
      strcat(longestString,othtemp->value);
    }
  }
  strcat(longestString,"\n");
  strcat(longestString,"\n");

  ListIterator famIter = createIterator(obj->families);
  Family* tempFam = NULL;

  while( (tempFam = nextElement(&famIter)) != NULL)
  {
    strcat(longestString," FAMILY:\n");
    strcat(longestString,"    Husband:");
    if(tempFam->husband != NULL)
      strcat(longestString,tempFam->husband->givenName);
    strcat(longestString,"__");
    if(tempFam->husband != NULL)
      strcat(longestString,tempFam->husband->surname);
    strcat(longestString,"\n");
    strcat(longestString,"    Wife:");
    if(tempFam->wife != NULL)
      strcat(longestString,tempFam->wife->givenName);
    strcat(longestString,"__");
    if(tempFam->wife != NULL)
      strcat(longestString,tempFam->wife->surname);
    strcat(longestString,"\n");

    ListIterator childIter = createIterator(tempFam->children);
    Individual* tempChild = NULL;

    while( (tempChild = nextElement(&childIter)) != NULL)
    {
      strcat(longestString,"     Child:");
      strcat(longestString,tempChild->givenName);
      strcat(longestString,"__");
      strcat(longestString,tempChild->surname);
      strcat(longestString,"\n");
    }
    ListIterator otherevnt = createIterator(tempFam->events);
    Event* othrevt = NULL;
    while( (othrevt = nextElement(&otherevnt)) != NULL)
    {
      strcat(longestString,"FAMEVENT\n");
      strcat(longestString,othrevt->type);
      strcat(longestString,"__");
      strcat(longestString,othrevt->date);
      strcat(longestString,"__");
      strcat(longestString,othrevt->place);
      strcat(longestString,"\n");
    }
    ListIterator other11 = createIterator(tempFam->otherFields);
    Field* othrtemp = NULL;
    while( (othrtemp = nextElement(&other11)) != NULL)
    {
      strcat(longestString,"OTHERFAM\n");
      strcat(longestString,othrtemp->tag);
      strcat(longestString,"__");
      strcat(longestString,othrtemp->value);
      strcat(longestString,"\n");
    }
  }
  return longestString;
}

void deleteGEDCOM(GEDCOMobject* obj)
{
  if(obj == NULL)
  {
    return;
  }

  ListIterator otherIter = createIterator(obj->header->otherFields);
  Field* tempField = NULL;
  while( (tempField = nextElement(&otherIter)) != NULL)
  {
    free(tempField->tag);
    free(tempField->value);
    free(tempField);
  }
  free(obj->header);

  ListIterator famIter = createIterator(obj->families);
  Family* tempFam = NULL;
  while( (tempFam = nextElement(&famIter)) != NULL)
  {
    ListIterator otherIter = createIterator(tempFam->otherFields);
    Field* tempField = NULL;
    while( (tempField = nextElement(&otherIter)) != NULL)
    {
      free(tempField->tag);
      free(tempField->value);
      free(tempField);
    }
    //free(tempFam->otherFields);
    free(tempFam);
  }
  //free(obj->families);

  ListIterator indIter = createIterator(obj->individuals);
  Individual* tempInd = NULL;
  while( (tempInd = nextElement(&indIter)) != NULL)
  {
    free(tempInd->givenName);
    free(tempInd->surname);

    ListIterator eventIter = createIterator(tempInd->events);
    Event* tempEvent = NULL;
    while( (tempEvent = nextElement(&eventIter)) != NULL)
    {
      free(tempEvent->date);
      free(tempEvent->place);
      ListIterator otherIter1 = createIterator(tempEvent->otherFields);
      Field* tempField1 = NULL;
      while( (tempField1 = nextElement(&otherIter1)) != NULL)
      {
        free(tempField1->tag);
        free(tempField1->value);
        //free(tempField1);
      }
      //free(tempEvent->families);
      //free(tempEvent);
    }

    ListIterator otherIter = createIterator(tempInd->otherFields);
    Field* tempField = NULL;
    while( (tempField = nextElement(&otherIter)) != NULL)
    {
      free(tempField->tag);
      free(tempField->value);
      //free(tempField);
    }
    free(tempInd);
  }
  //free(obj->individuals);
  free(obj);
}

char * printError(GEDCOMerror err)
{
  char * string = malloc(sizeof(char)* 25);
  if(err.type == 0)
  {
    strcpy(string,"OK ");
  }
  else if(err.type == 1)
  {
    strcpy(string,"invalid file ");
  }
  else if(err.type == 2)
  {
    strcpy(string,"invalid gedcom ");
  }
  else if(err.type == 3)
  {
    strcpy(string, "invalid header ");
  }
  else if(err.type == 4)
  {
    strcpy(string, "invalid record ");
  }
  else
  {
    strcpy(string, "other ");
  }
  strcat(string,"( line ");
  char integer[2];
  sprintf(integer, "%d",err.line);
  strcat(string,integer);
  strcat(string," )\n");
  return string;
}


Individual* findPerson(const GEDCOMobject* familyRecord, bool (*compare)(const void* first, const void* second), const void* person)
{
  if(familyRecord == NULL)
  {
    return NULL;
  }
  ListIterator iter = createIterator(familyRecord->individuals);
	void* elem;
  //customCompare = compareElement;
	bool comp;

	while((elem = nextElement(&iter)) != NULL){
		comp = compare(person,elem);
		if(comp==true)
			return elem;
	}

	return NULL;
}

List getDescendants(const GEDCOMobject* familyRecord, const Individual* person)
{
  List descendants = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
  if(familyRecord == NULL)
  {
    return descendants;
  }
  if(person == NULL)
  {
    return descendants;
  }
  ListIterator iter = createIterator(familyRecord->families);
  Family* elem;


  while( (elem = nextElement(&iter)) != NULL)
  {
    if(compareIndividuals(person,elem->husband)==0 || compareIndividuals(person,elem->wife)==0)
    {
        ListIterator child = createIterator(elem->children);
        Individual* kid;
        while( (kid = nextElement(&child)) != NULL)
        {
          insertBack(&descendants,kid);
        }
        break;
    }
  }


  ListIterator iter1 = createIterator(familyRecord->families);
  ListIterator iter2 = createIterator(descendants);
  Individual* grandkid;
  while( (grandkid = nextElement(&iter2)) != NULL)
  {
    Family* elem1;
    while( (elem1 = nextElement(&iter1)) != NULL)
    {
      if(compareIndividuals(grandkid,elem1->husband)==0 || compareIndividuals(grandkid,elem1->wife)==0)
      {
          ListIterator child1 = createIterator(elem1->children);
          Individual* kid1;
          while( (kid1 = nextElement(&child1)) != NULL)
          {
            insertBack(&descendants,kid1);
          }
          break;
      }
    }
  }

  return descendants;
}

/*****************************************************************************************************************************
************************************************Assignment 2 *****************************************************************
*****************************************************************************************************************************/
//TODO:DONT CALL VALIDATEGEDCOM FROM WRITEGEDCOM

GEDCOMerror writeGEDCOM(char* fileName, const GEDCOMobject* obj)
{
  GEDCOMerror error;

  if (obj==NULL || fileName==NULL)
  {
    error.type = WRITE_ERROR;///////////////////////////////////idk which error to put here
    error.line = -1;
    return error;
  }
  FILE* file = fopen(fileName,"w");

  int count = 0;

  char* writeList[50000];
  for(int i =0 ; i<50000;i++)
  {
    writeList[i] = malloc(sizeof(char)*50);
    strcpy(writeList[i] , "\0");
  }

  if(obj->header)
  {
    strcpy(writeList[count],"0 HEAD\n");
    count++;
    if(obj->header->source)
    {
      strcpy(writeList[count],"1 SOUR ");
      strcat(writeList[count],obj->header->source);
      strcat(writeList[count],"\n");
      count++;
    }
    strcpy(writeList[count],"1 GEDC\n");
    count++;
    if(obj->header->gedcVersion)
    {
      char convertedString[25];
      strcpy(writeList[count],"2 VERS ");
      sprintf(convertedString,"%.1f",obj->header->gedcVersion);
      strcat(writeList[count],convertedString);
      strcat(writeList[count],"\n");
      count++;
    }
    strcpy(writeList[count],"2 FORM LINEAGE-LINKED\n");
    count++;
    if(obj->header->encoding)
    {
      strcpy(writeList[count],"1 CHAR ");
      if(obj->header->encoding == 0)
      {
        strcat(writeList[count],"ANSEL\n");
      }
      else if(obj->header->encoding == 1)
      {
        strcat(writeList[count],"UTF-8\n");
      }
      else if(obj->header->encoding == 2)
      {
        strcat(writeList[count],"UNICODE\n");
      }
      else if(obj->header->encoding == 3)
      {
        strcat(writeList[count],"ASCII\n");
      }
      count++;
    }

    if(obj->header->submitter)
    {
      strcpy(writeList[count],"1 SUBM ");
      strcat(writeList[count],"@SUB1@");
      strcat(writeList[count],"\n");
      count++;
    }
  }
  if(obj->submitter)
  {
    strcpy(writeList[count],"0 ");
    strcat(writeList[count],"@SUB1@");
    strcat(writeList[count]," SUBM\n");
    count++;
    strcpy(writeList[count],"1 NAME ");
    strcat(writeList[count],obj->header->submitter->submitterName);
    strcat(writeList[count],"\n");
    count++;
  }

  List pointWrite= initializeList(&printPoint,&deletePoint,&comparePoints);
  ListIterator iter = createIterator(obj->individuals);
  Individual* temp = NULL;
  int individualCount = 1;
  while( (temp = nextElement(&iter)) != NULL)
  {
    Point* point = malloc(sizeof(Point));
    char integerStr[25];
    strcpy(writeList[count],"0 ");
    strcat(writeList[count],"@I00");
    sprintf(integerStr,"%d",individualCount);
    strcat(writeList[count],integerStr);
    strcat(writeList[count],"@ ");
    strcat(writeList[count],"INDI\n");
    strcpy(point->id,"@I00");
    strcat(point->id,integerStr);
    strcat(point->id,"@");
    point->pointer = temp;
    insertBack(&(pointWrite),point);
    count++;

    strcpy(writeList[count],"1 NAME ");
    strcat(writeList[count],temp->givenName);
    strcat(writeList[count]," /");
    strcat(writeList[count],temp->surname);
    strcat(writeList[count],"/\n");
    count++;

    ListIterator other1 = createIterator(temp->otherFields);
    Field* othtemp = NULL;
    while( (othtemp = nextElement(&other1)) != NULL)
    {
      if(strcmp(othtemp->tag,"GIVN")==0 || strcmp(othtemp->tag,"SURN")==0)
      {
        strcpy(writeList[count],"2 ");
        strcat(writeList[count],othtemp->tag);
        strcat(writeList[count]," ");
        strcat(writeList[count],othtemp->value);
        strcat(writeList[count],"\n");
        count++;
      }
    }

    ListIterator other11 = createIterator(temp->otherFields);
    Field* othtemp1 = NULL;
    while( (othtemp1 = nextElement(&other11)) != NULL)
    {
      if(strcmp(othtemp1->tag,"GIVN")==0 || strcmp(othtemp1->tag,"SURN")==0)
      {
        continue;
      }
      else{
        strcpy(writeList[count],"1 ");
        strcat(writeList[count],othtemp1->tag);
        strcat(writeList[count]," ");
        strcat(writeList[count],othtemp1->value);
        strcat(writeList[count],"\n");
        count++;
      }

    }

    ListIterator eventIter = createIterator(temp->events);
    Event* tempEvent = NULL;
    while( (tempEvent = nextElement(&eventIter)) != NULL)
    {
      strcpy(writeList[count],"1 ");
      strcat(writeList[count],tempEvent->type);
      strcat(writeList[count],"\n");
      count++;
      if(tempEvent->date && tempEvent->date[0] != '\0')
      {
        strcpy(writeList[count],"2 DATE ");
        strcat(writeList[count],tempEvent->date);
        strcat(writeList[count],"\n");
        count++;
      }
      if(tempEvent->place != NULL && tempEvent->place[0] != '\0')
      {
        strcpy(writeList[count],"2 PLAC ");
        strcat(writeList[count],tempEvent->place);
        strcat(writeList[count],"\n");
        count++;
      }
    }
    individualCount++;
  }

  ListIterator famIter = createIterator(obj->families);
  Family* tempFam = NULL;
  int famCount =1;
  while( (tempFam = nextElement(&famIter)) != NULL)
  {
    char integerStr[25];
    strcpy(writeList[count],"0 ");
    strcat(writeList[count],"@F00");
    sprintf(integerStr,"%d",famCount);
    strcat(writeList[count],integerStr);
    strcat(writeList[count],"@ ");
    strcat(writeList[count],"FAM\n");
    count++;

    if(tempFam->husband != NULL)
    {
      ListIterator husbIter = createIterator(pointWrite);
      Point* tempHus = NULL;
      while( (tempHus = nextElement(&husbIter)) != NULL)
      {
        if(tempHus->pointer==tempFam->husband)
        {
          //fams insertion into individual
          int lineNum = -1;
          char findStr[25];
          strcpy(findStr,"0 ");
          strcat(findStr,tempHus->id);
          strcat(findStr," INDI");
          for(int i=0;i<=count;i++)
          {
            if(strstr(writeList[i],findStr)!= NULL)
            {
              lineNum = i;
              break;
            }
          }
          lineNum++;
          while(strstr(writeList[lineNum],"0 @") ==NULL)
          {
            lineNum++;
          }
          for(int i=count;i>=lineNum;i--)
          {
            strcpy(writeList[i],writeList[i-1]);
          }
          strcpy(writeList[lineNum],"1 FAMS @F00");
          strcat(writeList[lineNum],integerStr);
          strcat(writeList[lineNum],"@\n");
          count++;


          strcpy(writeList[count],"1 HUSB ");
          strcat(writeList[count],tempHus->id);
          strcat(writeList[count],"\n");
          count++;
          break;
        }
      }
    }
    if(tempFam->wife != NULL)
    {
      ListIterator wifeIter = createIterator(pointWrite);
      Point* tempWife = NULL;
      while( (tempWife = nextElement(&wifeIter)) != NULL)
      {
        if(tempWife->pointer==tempFam->wife)
        {
          //fams insertion into individual
          int lineNum = -1;
          char findStr[25];
          strcpy(findStr,"0 ");
          strcat(findStr,tempWife->id);
          strcat(findStr," INDI");
          for(int i=0;i<=count;i++)
          {
            if(strstr(writeList[i],findStr)!= NULL)
            {
              lineNum = i;
              break;
            }
          }
          lineNum++;
          while(strstr(writeList[lineNum],"0 @") ==NULL)
          {
            lineNum++;
          }
          for(int i=count;i>=lineNum;i--)
          {
            strcpy(writeList[i],writeList[i-1]);
          }
          strcpy(writeList[lineNum],"1 FAMS @F00");
          strcat(writeList[lineNum],integerStr);
          strcat(writeList[lineNum],"@\n");
          count++;


          strcpy(writeList[count],"1 WIFE ");
          strcat(writeList[count],tempWife->id);
          strcat(writeList[count],"\n");
          count++;
        }
      }
    }

    ListIterator eventIter = createIterator(tempFam->events);
    Event* tempEvent = NULL;
    while( (tempEvent = nextElement(&eventIter)) != NULL)
    {
      strcpy(writeList[count],"1 ");
      strcat(writeList[count],tempEvent->type);
      strcat(writeList[count],"\n");
      count++;
      if(tempEvent->date && tempEvent->date[0] != '\0')
      {
        strcpy(writeList[count],"2 DATE ");
        strcat(writeList[count],tempEvent->date);
        strcat(writeList[count],"\n");
        count++;
      }
      if(tempEvent->place[0] != '\0' && strcmp(tempEvent->place,"\0")!=0 && strcmp(tempEvent->place,"")!=0)
      {
        strcpy(writeList[count],"2 PLAC ");
        strcat(writeList[count],tempEvent->place);
        strcat(writeList[count],"\n");
        count++;
      }
    }

    ListIterator childIter = createIterator(tempFam->children);
    Individual* tempChild = NULL;
    while( (tempChild = nextElement(&childIter)) != NULL)
    {
      ListIterator pointIter = createIterator(pointWrite);
      Point* tempPoint = NULL;
      while( (tempPoint = nextElement(&pointIter)) != NULL)
      {
        if(tempPoint->pointer==tempChild)
        {
          //fams insertion into individual
          int lineNum = -1;
          char findStr[25];
          strcpy(findStr,"0 ");
          strcat(findStr,tempPoint->id);
          strcat(findStr," INDI");
          for(int i=0;i<=count;i++)
          {
            if(strstr(writeList[i],findStr)!= NULL)
            {
              lineNum = i;
              break;
            }
          }
          lineNum++;
          while(strstr(writeList[lineNum],"0 @") ==NULL)
          {
            lineNum++;
          }
          for(int i=count;i>=lineNum;i--)
          {
            strcpy(writeList[i],writeList[i-1]);
          }
          strcpy(writeList[lineNum],"1 FAMC @F00");
          strcat(writeList[lineNum],integerStr);
          strcat(writeList[lineNum],"@\n");
          count++;


          strcpy(writeList[count],"1 CHIL ");
          strcat(writeList[count],tempPoint->id);
          strcat(writeList[count],"\n");
          count++;
        }
      }
    }
    famCount++;
  }
  strcpy(writeList[count],"0 TRLR\n");

  for(int i=0;i<=count;i++)
  {
    fprintf(file,writeList[i]);
  }
  for(int i =0 ; i<50000;i++)
  {
    free(writeList[i]);
  }
  fclose(file);
  error.type = OK;
  error.line = -1;
  return error;
}


ErrorCode validateGEDCOM(const GEDCOMobject* obj)
{
  if(obj==NULL)
  {
    return OTHER_ERROR;
  }
  else if(obj->header == NULL)
  {
    return INV_GEDCOM;
  }
  else if(obj->submitter == NULL)
  {
    return INV_GEDCOM;
  }
  else if(obj->header->source == NULL)
  {
    return INV_HEADER;
  }
  else if(strlen(obj->header->source)>200)
  {
    return INV_RECORD;
  }
  else if(obj->header->gedcVersion == 0)
  {
    return INV_HEADER;
  }
  else if(obj->header->encoding>3 || obj->header->encoding<0)
  {
    return INV_HEADER;
  }
  else if(obj->header->submitter == NULL)
  {
    return INV_HEADER;
  }
  else if(obj->submitter->submitterName == NULL)
  {
    return INV_RECORD;
  }
  else if(strlen(obj->submitter->submitterName)>200)
  {
    return INV_RECORD;
  }
  else{
    ListIterator iter = createIterator(obj->families);
    Family* temp = NULL;
    for(int i=0;i<getLength(obj->families);i++)
    {
      temp = nextElement(&iter);
      if(temp == NULL)
      {
        return INV_RECORD;
      }

      ListIterator events = createIterator(temp->events);
      Event* tempE = NULL;
      while( (tempE = nextElement(&events)) != NULL)
      {
        if(strlen(tempE->date)>200)
        {
          return INV_RECORD;
        }
        if(strlen(tempE->place)>200)
        {
          return INV_RECORD;
        }
      }

      ListIterator other = createIterator(temp->otherFields);
      Field* tempO = NULL;
      while( (tempO = nextElement(&other)) != NULL)
      {
        if(strlen(tempO->tag)>200)
        {
          return INV_RECORD;
        }
        if(strlen(tempO->value)>200)
        {
          return INV_RECORD;
        }
      }
    }

    ListIterator iter2 = createIterator(obj->individuals);
    Individual* temp2 = NULL;
    for(int i=0;i<getLength(obj->families);i++)
    {
      temp2 = nextElement(&iter2);
      if(temp2 == NULL)
      {
        return INV_RECORD;
      }
      if(strlen(temp2->givenName)>200)
      {
        return INV_RECORD;
      }
      if(strlen(temp2->surname)>200)
      {
        return INV_RECORD;
      }
      ListIterator events = createIterator(temp2->events);
      Event* tempE = NULL;
      while( (tempE = nextElement(&events)) != NULL)
      {
        if(strlen(tempE->date)>200)
        {
          return INV_RECORD;
        }
        if(strlen(tempE->place)>200)
        {
          return INV_RECORD;
        }
      }

      ListIterator other = createIterator(temp2->otherFields);
      Field* tempO = NULL;
      while( (tempO = nextElement(&other)) != NULL)
      {
        if(strlen(tempO->tag)>200)
        {
          return INV_RECORD;
        }
        if(strlen(tempO->value)>200)
        {
          return INV_RECORD;
        }
      }

    }

    return OK;
  }
}

List getDescendantListN(const GEDCOMobject* familyRecord, const Individual* person, unsigned int maxGen)
{
  List generations = initializeList(&printGeneration,&deleteGeneration,&compareGenerations);
  List* descendants = malloc(sizeof(List));
  *descendants = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
  //List tempList = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
  if(familyRecord == NULL)
  {
    return generations;
  }
  if(person == NULL)
  {
    return generations;
  }
  if(maxGen==0)
  {
    maxGen = 10;
  }

  ListIterator iter = createIterator(familyRecord->families);
  Family* elem;
  while( (elem = nextElement(&iter)) != NULL)
  {
    if(compareIndividuals(person, elem->husband)==0 || compareIndividuals(person,elem->wife)==0)
    {
        /*ListIterator child = createIterator(elem->children);
        Individual* kid;
        while( (kid = nextElement(&child)) != NULL)
        {
          printf("RECURSION: %s %s\n",kid->givenName,kid->surname);

          if(maxGen>0)
          {
            printf("MAXGEN%d\n",maxGen );
            tempList = getDescendantListN(familyRecord,kid,maxGen-1);
            ListIterator idk = createIterator(tempList);
            Individual* man;
            while( (man = nextElement(&idk)) != NULL)
            {
              if(findElement(descendants,compareIndividualsBool,man)==NULL)
                insertSorted(&descendants,man);
            }
          }
          insertSorted(descendants,kid);
        }*/
          if(compareIndividuals(person, elem->husband)==0){
            insertBack(descendants,elem->husband);
          }
          else{
            insertBack(descendants,elem->wife);
          }
          break;

    }
  }
  //insertBack(&generations,descendants);
  getDescendantListRecursion(familyRecord,&generations,*descendants,maxGen);
  return generations;
}

void getDescendantListRecursion(const GEDCOMobject* familyRecord,List* generations, List people, unsigned int maxGen)
{
  List* descendants = malloc(sizeof(List));
  *descendants = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
  /*if(maxGen<=0)
  {
    return NULL;
  }*/
  ListIterator iter = createIterator(familyRecord->families);
  Family* elem;
  int count =0;
  while( (elem = nextElement(&iter)) != NULL)
  {
    ListIterator iter2 = createIterator(people);
    Individual* elem2;
    while( (elem2 = nextElement(&iter2)) != NULL)
    {
      if(elem2== elem->husband|| elem2==elem->wife)
      {
         ListIterator child = createIterator(elem->children);
         Individual* kid;
         while( (kid = nextElement(&child)) != NULL)
         {
           //printf("%s%s has kid %s%s\n",elem2->givenName,elem2->surname,kid->givenName,kid->surname );
           count++;
           insertSorted(descendants,kid);
         }
      }
    }
  }
  if(count>0)
  {
    insertFront(generations,descendants);
    getDescendantListRecursion(familyRecord,generations,*descendants,maxGen-1);
  }

}

List getAncestorListN(const GEDCOMobject* familyRecord, const Individual* person, int maxGen)
{
  List generations = initializeList(&printGeneration,&deleteGeneration,&compareGenerations);
  List* descendants = malloc(sizeof(List));
  *descendants = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);

  if(familyRecord == NULL)
  {
    return generations;
  }
  if(person == NULL)
  {
    return generations;
  }
  if(maxGen==0)
  {
    maxGen = 10;
  }

  ListIterator iter = createIterator(familyRecord->families);
  Family* elem;
  while( (elem = nextElement(&iter)) != NULL)
  {
    ListIterator iter1 = createIterator(elem->children);
    Individual* elem1;
    while( (elem1 = nextElement(&iter1)) != NULL)
    {
      if(compareIndividuals(person, elem1)==0)
      {
          insertBack(descendants,elem1);
          //insertBack(descendants,elem->wife);
          break;
      }
    }
  }
  getAncestorListRecursion(familyRecord,&generations,*descendants,maxGen);
  return generations;
}

void getAncestorListRecursion(const GEDCOMobject* familyRecord,List* generations, List people, unsigned int maxGen)
{
  List* descendants = malloc(sizeof(List));
  *descendants = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
  /*if(maxGen<=0)
  {
    return NULL;
  }*/
  int count = 0;
  ListIterator iter = createIterator(familyRecord->families);
  Family* elem;
  while( (elem = nextElement(&iter)) != NULL)
  {
    ListIterator iter1 = createIterator(elem->children);
    Individual* elem1;
    while( (elem1 = nextElement(&iter1)) != NULL)
    {
      ListIterator iter2 = createIterator(people);
      Individual* elem2;
      while( (elem2 = nextElement(&iter2)) != NULL)
      {
        if(elem2== elem1)
        {
          //printf("%s%s has parents %s%s and %s%s\n",elem2->givenName,elem2->surname,elem->husband->givenName,elem->husband->surname,elem->wife->givenName,elem->wife->surname );
            insertBack(descendants,elem->husband);
            insertBack(descendants,elem->wife);
            count++;
            break;
        }
      }
    }
  }
  if(count>0)
  {
    insertFront(generations,descendants);
    getAncestorListRecursion(familyRecord,generations,*descendants,maxGen-1);
  }

}
///////////////////////////////////////////////A2 Helper functions//////////////
char* printGeneration(void* toBePrinted)
{
  char* string = malloc(sizeof(char)*2000);

  /*ListIterator gen = createIterator((List)toBePrinted);
  Individual* ind;
  while( (ind = nextElement(&gen)) != NULL)
  {
    strcpy(string,ind->givenName);
    strcat(string," ");
    strcat(string,ind->surname);
  }
*/
  return string;
}

void deleteGeneration(void* toBeDeleted)
{
  clearList((List*) toBeDeleted);
}

int compareGenerations(const void* first,const void* second)
{/*
  List tmpName1;
  List tmpName2;

  if (first == NULL || second == NULL){
      return -2;
  }

  tmpName1 = (List)first;
  tmpName2 = (List)second;
  bool same = false;
  ListIterator gen = createIterator(tmpName1);
  Individual* ind;
  while( (ind = nextElement(&gen)) != NULL)
  {
    same = false;
    ListIterator gen1 = createIterator(tmpName2);
    Individual* ind1;
    while( (ind1 = nextElement(&gen1)) != NULL)
    {
      if(ind == ind1)
      {
        same = true;
      }
    }
    if(same == false)
    {
      return 1;
    }
  }*/
  return 0;
}
/////////////////////////////////////////////////helper functions end////////////////



char* indToJSON(const Individual* ind)
{
  char* longestString = malloc(sizeof(char)* 100);
  char convertedString[200];
  if(ind == NULL)
  {
    strcpy(longestString,"");
    return longestString;
  }
  strcpy(longestString,"{");
  strcat(longestString,"\"givenName\":\"");
  if(ind->givenName!=NULL)
  {
    strcpy(convertedString,ind->givenName);
    for (int i = 0; i < strlen(convertedString); i++)
    {
      if(convertedString[i] == '\"')
      {
        int len = strlen(convertedString);
        for (int j = len+1; j > i; j--)
        {
          convertedString[j] = convertedString[j-1];
        }
        convertedString[i] = '\\';
        i++;
      }
    }
    strcat(longestString,convertedString);
  }

  strcat(longestString,"\",\"surname\":\"");

  if(ind->surname!=NULL)
  {
    strcpy(convertedString,ind->surname);
    for (int i = 0; i < strlen(convertedString); i++)
    {
      if(convertedString[i] == '\"')
      {
        int len = strlen(convertedString);
        for (int j = len+1; j > i; j--)
        {
          convertedString[j] = convertedString[j-1];
        }
        convertedString[i] = '\\';
        i++;
      }
    }
    strcat(longestString,convertedString);
  }
  strcat(longestString,"\"}");

  return longestString;
}

Individual* JSONtoInd(const char* str)
{
  if(str == NULL)
  {
    return NULL;
  }
  Individual* indi = malloc(sizeof(Individual));
  indi->givenName = malloc(sizeof(char) * 50);
  indi->surname = malloc(sizeof(char) *  50);
  strcpy(indi->givenName,"");
  strcpy(indi->surname,"");
  char* st= malloc(sizeof(char) * 150);
  strcpy(st,str);
  char* string = strtok(st,"\"");
  char givn[50];
  char sur[50];
  strcpy(givn,"");
  strcpy(sur,"");
  if(strcmp(string,"{")!=0)
  {
    return NULL;
  }
  bool hasFN = false;
  bool hasLN = false;
  while((string=strtok(NULL,"\""))!=NULL)
  {
    if(strcmp("givenName",string)==0)
    {
      hasFN = true;
      string=strtok(NULL,"\"");
      if(strcmp(":",string)==0)
      {
        while(true)
        {
          string=strtok(NULL,"\"");
          strcat(givn,string);
          if(string[strlen(string-1)]!='\\')
          {
            break;
          }
        }
        for (int i = 0; i < strlen(givn); i++)
        {
          if(givn[i]=='\\')
          {
            int j = i;
            while(givn[j]!='\0')
            {
              givn[j]=givn[j+1];
            }
          }
        }
        strcpy(indi->givenName,givn);
        if(strcmp(givn,",")==0)
        {
          strcpy(indi->givenName,"");
        }

      }
    }
    if(strcmp("surname",string)==0)
    {
      hasLN = true;
      string=strtok(NULL,"\"");
      if(strcmp(":",string)==0)
      {
        while(true)
        {
          string=strtok(NULL,"\"");
          strcat(sur,string);
          if(string[strlen(string-1)]!='\\')
          {
            break;
          }
        }
        for (int i = 0; i < strlen(sur); i++)
        {
          if(givn[i]=='\\')
          {
            int j = i;
            while(sur[j]!='\0')
            {
              sur[j]=sur[j+1];
            }
          }
        }
        strcpy(indi->surname,sur);
        if(strcmp(sur,"}")==0)
        {
          strcpy(indi->surname,"");
        }
      }
    }
  }
  free(st);
  if(hasLN==false || hasFN==false)
  {
    return NULL;
  }
  return indi;
}



GEDCOMobject* JSONtoGEDCOM(const char* str)
{
  if(str ==NULL)
  {
    return NULL;
  }
  GEDCOMobject* obj = malloc(sizeof(GEDCOMobject));
  obj->families = initializeList(&printFamily,&deleteFamily,&compareFamilies);
  obj->individuals = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
  Header* header = malloc(sizeof(Header));
  header->otherFields = initializeList(&printField,&deleteField,&compareFields);
  Submitter* submitter = malloc(sizeof(submitter));
  submitter->otherFields = initializeList(&printField,&deleteField,&compareFields);

  char token[50] = " ";
  int strPos =0;
  int tokPos = 0;
  while ((str[strPos] != '\0') && (strPos<strlen(str)))
  {
    token[tokPos] = str[strPos];
    tokPos++;
    strPos++;
    if(str[strPos-1]=='\\' && str[strPos]=='\"')
    {
      tokPos--;
    }
    if(str[strPos-1]!='\\' && str[strPos]=='\"')
    {

      if(strcmp(token,"subName")==0)///////////////////////////////////////////TODO:change this if needed
      {
        tokPos = 0;
        memset(token,0,sizeof(token));
        strPos++;
        while ((str[strPos] != '\0') || (strPos<strlen(str)))
        {
          token[tokPos] = str[strPos];
          tokPos++;
          strPos++;
          if(str[strPos-1]=='\\' && str[strPos]=='\"')
          {
            tokPos--;
          }
          if(str[strPos-1]!='\\' && str[strPos]=='\"')
          {
            break;
          }
        }

        if(strcmp(token,":")==0)
        {
          if(str[strPos]=='\"')
          {
            strPos++;
            tokPos = 0;
            memset(token,0,sizeof(token));
            while (str[strPos]!='\"')
            {
              if(str[strPos]=='\\' && str[strPos+1]=='\"')
              {
                token[tokPos] = str[strPos];
                tokPos++;
                strPos++;
              }
              token[tokPos] = str[strPos];
              tokPos++;
              strPos++;
            }
            strcpy(submitter->submitterName,token);
          }
        }
      }
      else if(strcmp(token,"subAddress")==0)////////////////////////////////////??TODO:change this if needed
      {
        tokPos = 0;
        memset(token,0,sizeof(token));
        strPos++;
        while ((str[strPos] != '\0') || (strPos<strlen(str)))
        {
          token[tokPos] = str[strPos];
          tokPos++;
          strPos++;
          if(str[strPos-1]=='\\' && str[strPos]=='\"')
          {
            tokPos--;
          }
          if(str[strPos-1]!='\\' && str[strPos]=='\"')
          {
            break;
          }
        }

        if(strcmp(token,":")==0)
        {
          if(str[strPos]=='\"')
          {
            strPos++;
            tokPos = 0;
            memset(token,0,sizeof(token));
            while (str[strPos]!='\"')
            {
              if(str[strPos]=='\\' && str[strPos+1]=='\"')
              {
                token[tokPos] = str[strPos];
                tokPos++;
                strPos++;
              }
              token[tokPos] = str[strPos];
              tokPos++;
              strPos++;
            }
            strcpy(submitter->address,token);
          }
        }
      }
      else if(strcmp(token,"source")==0)////////////////////////////////////??TODO:change this if needed
      {
        tokPos = 0;
        memset(token,0,sizeof(token));
        strPos++;
        while ((str[strPos] != '\0') || (strPos<strlen(str)))
        {
          token[tokPos] = str[strPos];
          tokPos++;
          strPos++;
          if(str[strPos-1]=='\\' && str[strPos]=='\"')
          {
            tokPos--;
          }
          if(str[strPos-1]!='\\' && str[strPos]=='\"')
          {
            break;
          }
        }

        if(strcmp(token,":")==0)
        {
          if(str[strPos]=='\"')
          {
            strPos++;
            tokPos = 0;
            memset(token,0,sizeof(token));
            while (str[strPos]!='\"')
            {
              if(str[strPos]=='\\' && str[strPos+1]=='\"')
              {
                token[tokPos] = str[strPos];
                tokPos++;
                strPos++;
              }
              token[tokPos] = str[strPos];
              tokPos++;
              strPos++;
            }
            strcpy(header->source,token);
          }
        }
      }
      else if(strcmp(token,"gedcVersion")==0)////////////////////////////////////??TODO:change this if needed
      {
        tokPos = 0;
        memset(token,0,sizeof(token));
        strPos++;
        while ((str[strPos] != '\0') || (strPos<strlen(str)))
        {
          token[tokPos] = str[strPos];
          tokPos++;
          strPos++;
          if(str[strPos-1]=='\\' && str[strPos]=='\"')
          {
            tokPos--;
          }
          if(str[strPos-1]!='\\' && str[strPos]=='\"')
          {
            break;
          }
        }

        if(strcmp(token,":")==0)
        {
          if(str[strPos]=='\"')
          {
            strPos++;
            tokPos = 0;
            memset(token,0,sizeof(token));
            while (str[strPos]!='\"')
            {
              if(str[strPos]=='\\' && str[strPos+1]=='\"')
              {
                token[tokPos] = str[strPos];
                tokPos++;
                strPos++;
              }
              token[tokPos] = str[strPos];
              tokPos++;
              strPos++;
            }
            header->gedcVersion=atof(token);
          }
        }
      }
      else if(strcmp(token,"encoding")==0)////////////////////////////////////??TODO:change this if needed
      {
        tokPos = 0;
        memset(token,0,sizeof(token));
        strPos++;
        while ((str[strPos] != '\0') || (strPos<strlen(str)))
        {
          token[tokPos] = str[strPos];
          tokPos++;
          strPos++;
          if(str[strPos-1]=='\\' && str[strPos]=='\"')
          {
            tokPos--;
          }
          if(str[strPos-1]!='\\' && str[strPos]=='\"')
          {
            break;
          }
        }

        if(strcmp(token,":")==0)
        {
          if(str[strPos]=='\"')
          {
            strPos++;
            tokPos = 0;
            memset(token,0,sizeof(token));
            while (str[strPos]!='\"')
            {
              if(str[strPos]=='\\' && str[strPos+1]=='\"')
              {
                token[tokPos] = str[strPos];
                tokPos++;
                strPos++;
              }
              token[tokPos] = str[strPos];
              tokPos++;
              strPos++;
            }
            if(strcmp(token,"ANSEL")==0)
            {
              header->encoding = ANSEL;
            }
            else if(strcmp(token,"UTF8")==0 || strcmp(token,"UTF-8")==0)
            {
              header->encoding = UTF8;
            }
            else if(strcmp(token,"UNICODE")==0)
            {
              header->encoding = UNICODE;
            }
            else if(strcmp(token,"ASCII")==0)
            {
              header->encoding = ASCII;
            }
          }
        }
      }

      tokPos = 0;
      memset(token,0,sizeof(token));
      strPos++;
    }


  }
  header->submitter = submitter;
  obj->header = header;
  obj->submitter = submitter;
  return obj;
}

void addIndividual(GEDCOMobject* obj, const Individual* toBeAdded)
{
  if(obj ==NULL || toBeAdded == NULL)
  {
    return;
  }
  insertBack(&(obj->individuals),(void*)toBeAdded);
}

char* iListToJSON(List iList)
{
  char* longestString = malloc(sizeof(char)* 1500);
  strcpy(longestString,"[");
  ListIterator iter = createIterator(iList);
  Individual* ind = NULL;
  if((ind = nextElement(&iter)) != NULL)
  {
    strcat(longestString,indToJSON(ind));
  }
  while( (ind = nextElement(&iter)) != NULL)
  {
    strcat(longestString,",");
    strcat(longestString,indToJSON(ind));
  }
  strcat(longestString,"]");
  return longestString;
}

char* gListToJSON(List gList)
{
  char* longestString = malloc(sizeof(char)* 15000);
  if(gList.head == NULL)
  {
    return longestString;
  }

  strcpy(longestString,"[");
  ListIterator iter = createIterator(gList);
  List* ind = NULL;
  if((ind = nextElement(&iter)) != NULL)
  {
    strcat(longestString,iListToJSON(*ind));
  }
  while( (ind = nextElement(&iter)) != NULL)
  {
    strcat(longestString,",");
    strcat(longestString,iListToJSON(*ind));
  }
  strcat(longestString,"]");
  return longestString;
}
