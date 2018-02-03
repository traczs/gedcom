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

  while(fgets(string,300,file)!=NULL)
  {
    if(strlen(string)> 256)
    {
      free(*obj);
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
/*  for(int i =0; i<listCount;i++)
  {
    int j = 0;
    while (strcmp(wholeList[i][j],"\0" )!=0)
    {
      printf("MYLIST[%d][%d]= %s\n",i,j,wholeList[i][j]);
      j++;
    }
  }*/

//checking if head and trlr exist
  if(wholeList[0][0]!=NULL && (strcmp(wholeList[0][0],"0")==0))
  {
      if(wholeList[0][1] != NULL)
      {
        if(strcmp(wholeList[0][1],"HEAD") == 0)
        {
          headCheck = true;
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
  //header parsing into data structure
  if(sourChk ==true && versChk ==true && submChk== true && gedcChk==true && formChk==true && charChk==true)
  {
    (*obj)->header =  malloc(sizeof(Header));


    int j=0;
    int i = 1;
    (*obj)->submitter = malloc(sizeof(Submitter)+20* sizeof(char));
    (*obj)->submitter->otherFields = initializeList(&printField,&deleteField,&compareFields);
    (*obj)->header->otherFields = initializeList(&printField,&deleteField,&compareFields);

    while ((strcmp(wholeList[i][j],"0" )!=0) && (wholeList[i][j]!=NULL))
    {
      if(strcmp(wholeList[i][j+1],"SOUR")==0)
      {
        if(wholeList[i][j+2] != NULL)
          strcpy((*obj)->header->source, wholeList[i][j+2]);
      }
      else if((strcmp(wholeList[i][j+1],"GEDC")==0) && (strcmp(wholeList[i+1][j+1],"VERS" )==0))
      {
          (*obj)->header->gedcVersion = atof(wholeList[i+1][j+2]);
      }
      else if((strcmp(wholeList[i][j+1],"GEDC")==0) && (strcmp(wholeList[i+2][j+1],"VERS" )==0))
      {
          (*obj)->header ->gedcVersion = atof(wholeList[i+2][j+2]);
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

          strcpy((*obj)->submitter->address ,wholeList[i][j+2]);

          for(int i =1; i<listCount;i++)
          {
            if((strcmp(wholeList[i][1],(*obj)->submitter->address)==0))
            {
              i++;
              while ((strcmp(wholeList[i][0],"0" )!=0) && (wholeList[i][0]!=NULL))
              {
                if(strcmp(wholeList[i][1],"NAME")==0)
                {
                  strcpy((*obj)->submitter->submitterName,wholeList[i][2]);
                }
                else
                {
                  Field* field = malloc(sizeof(Field));
                  field->tag = malloc(sizeof(char) * 10);
                  field->value = malloc(sizeof(char) * 100);
                  if(wholeList[i][1] != NULL)
                    strcpy(field->tag, wholeList[i][1]);
                  if(wholeList[i][2] != NULL)
                    strcpy(field->value, wholeList[i][2]);

                  insertBack(&((*obj)->submitter->otherFields),field);
                  (*obj)->header->submitter = (*obj)->submitter;
                  //Field* temp = (Field*)(*obj)->header->submitter->otherFields.tail->data;
                  //printf("woo sus:%s \n",temp->tag);
                }
                i++;
              }
            }
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
          if(wholeList[i][1] != NULL)
            strcpy(field->tag, wholeList[i][1]);
          if(wholeList[i][2] != NULL)
            strcpy(field->value, wholeList[i][2]);

          insertBack(&((*obj)->header->otherFields),field);

          //Field* temp = (Field*)(*obj)->header->otherFields.tail->data;
          //printf("woo sus:%s \n",temp->tag);
        }
      }
      i++;
    }
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
          char name[75] = " ";
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
                strcat(event->date," ");
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
      i++;
      Point* temp = malloc(sizeof(Point));
      while ((strcmp(wholeList[i][0],"0" )!=0) && (wholeList[i][0]!=NULL) && (strcmp(wholeList[i][0],"\0" )!=0))
      {
        temp = NULL;
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
      }

      if(temp != NULL){
        Individual * ind = temp->pointer;
        insertBack(&(ind->families),fam);
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
  strcat(longestString,obj->submitter->submitterName);

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
      strcat(longestString,"\n        EVENT:");
      strcat(longestString,tempEvent->type);
      strcat(longestString,"__");
      strcat(longestString,tempEvent->date);
      strcat(longestString,"__");
      strcat(longestString,tempEvent->place);
    }
  }
  strcat(longestString,"\n");
  strcat(longestString,"\n");

  ListIterator famIter = createIterator(obj->families);
  Family* tempFam = NULL;

  while( (tempFam = nextElement(&famIter)) != NULL)
  {
    strcat(longestString," FAMILY:\n");
    strcat(longestString,"    Husband: ");
    if(tempFam->husband != NULL)
      strcat(longestString,tempFam->husband->givenName);
    strcat(longestString,"__");
    if(tempFam->husband != NULL)
      strcat(longestString,tempFam->husband->surname);
    strcat(longestString,"\n");
    strcat(longestString,"    Wife: ");
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
  ListIterator iter = createIterator(familyRecord->individuals);
	void* elem;
  //customCompare = compareElement;
	bool comp;

  if((elem = nextElement(&iter)) == NULL)
    return NULL;

	do{
		comp = compare(person,elem);
		if(comp==true)
			return elem;
	}while( (elem = nextElement(&iter)) != NULL);

	return NULL;
}

List getDescendants(const GEDCOMobject* familyRecord, const Individual* person)
{
  List descendants = initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
  ListIterator iter = createIterator(familyRecord->families);
  Family* elem;

  if(person == NULL)
  {
      return descendants;
  }


  while( (elem = nextElement(&iter)) != NULL)
  {
    if((strcmp(person->givenName,elem->husband->givenName)==0) || (strcmp(person->givenName,elem->wife->givenName)==0))
    {
      if((strcmp(person->givenName,elem->husband->surname)==0) || (strcmp(person->givenName,elem->wife->surname)==0))
      {
        ListIterator child = createIterator(elem->children);
        Individual* kid;
        while( (kid = nextElement(&child)) != NULL)
        {
          insertBack(&descendants,kid);
        }
      }
    }
  }
  return descendants;
}
