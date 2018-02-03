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
  free(temp);

  deleteGEDCOM(obj);
  return 0;
}
