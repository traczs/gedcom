#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GEDCOMparser.h"

int main(int argc, char *argv[]) {


  GEDCOMobject* obj;
  GEDCOMerror error = createGEDCOM(argv[1], &obj);
  printf("ERROR:\n %d -- %d\n",error.line,error.type);
  return 0;
}
