//#include <stdlib.h>
//#include <string>
//#include "naomngt.h"
//#include <fstream>
#include "imbdd.h"
int main(){  
  IMbdd bdd("cassiopee");
  bdd.load_bdd_configuration("bdd/cassiopee","imconfig.xml");
  bdd.show_bdd_configuration();
  bdd.write_bdd_configuration(".","imconfig.xml");
  return 0;
}
