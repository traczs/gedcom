#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GEDCOMparser.h"

int main(int argc, char *argv[]) {


  GEDCOMobject* obj;
  GEDCOMerror error = createGEDCOM(argv[1], &obj);
  printf("%s\n",printError(error));
  char * temp = printGEDCOM(obj);
  printf("     GEDCOM:\n%s\n", temp);

  printf("VALIDATION :%d\n",validateGEDCOM(obj) );

  GEDCOMerror error2 = writeGEDCOM("output.ged", obj);
  printf("%s\n",printError(error2));

  /*ListIterator child = createIterator(obj->individuals);
  Individual* kid;
  List list;
  while((kid = nextElement(&child))!=NULL)
  {
    if((strcmp(kid->givenName,"Richard")==0)&&strcmp(kid->surname,"Shakespeare")==0)
     list = getAncestorListN(obj, kid,1);
  }

  printf("%s",gListToJSON(list));*/

  //printf("JSON:%s\n",iListToJSON(list) );
  /*ListIterator qwe = createIterator(list);
  Individual* sks;
  while( (sks = nextElement(&qwe)) != NULL)
  {
    printf("LIST: %s\n",sks->givenName );
  }*/

  /*GEDCOMobject* sus=  JSONtoGEDCOM("{\"source\":\"Blah\",\"gedcVersion\":\"5.5\",\"encoding\":\"ASCII\",\"submitterName\":\"Some dude\",\"address\":\"nowhere\"}");
  if(sus)
  {
    printf("mainc obj:%s\n",sus->submitter->address );
  }
  addIndividual(sus,JSONtoInd("{\"givenName\":\"Gurjap\",\"surname\":\"\"}"));
  addIndividual(sus,JSONtoInd("{\"givenName\":\"Jurgap\",\"surname\":\"yellow\"}"));
  ListIterator qwe = createIterator(sus->individuals);
  Individual* sks;
  while( (sks = nextElement(&qwe)) != NULL)
  {
    printf("LIST: |%s|\n",sks->surname );
  }
  deleteGEDCOM(sus);*/
  free(temp);
  deleteGEDCOM(obj);
  return 0;
}
