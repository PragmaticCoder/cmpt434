#include "commandLine.h"
#include "dictionary.h"
#include "dbg.h"
int 
Command_interface(int argc, char *argv[], char *retBuffer)
{
    char valueBuffer[20];
    char keyBuffer[20];

    for( ; *argv ; argv++) {
        log_info("cmd : %s\n", *argv);
        if(!strcmp(*argv, "add")) {			
          log_info("\tKey : %s\t\tValue : %s\n", *(argv + 1), *(argv + 2));
          if((argc > 2) && (Dictionary_add(*(argv + 1), *(argv + 2)) != 0)) {			
              sprintf(retBuffer, "%s - Key Pair Successfully added\n", *(argv + 1));
          }
          else{
              sprintf(retBuffer, "%s - Key Pair Failed to add\n", *(argv + 1));
              return 0;
          }
          argv += 2;
          argc -= 2;
          return 1;
      } 
      else if(!strcmp(*argv, "getvalue")) {
          log_info("\tKey : %s\t\t", *(argv + 1));			
          if((argc > 1) && (Dictionary_getValue(*(argv + 1), valueBuffer) != 0)) {
              sprintf(retBuffer, "Value : %s\n", valueBuffer);
          }
          else {
              sprintf(retBuffer, "\nRequested Key is not present\n");
              return 0;
          }
          argc -= 1;
          argv += 1;
          return 1;
      }
      else if(!strcmp(*argv, "getall")) {
          char buffer[64];
          retBuffer[0] = 0;
          int end = Dictionary_getAll(keyBuffer, valueBuffer);
          while(end != 0){
              sprintf(buffer, "\tKey : %s\t\tValue : %s\n", keyBuffer, valueBuffer);
              strcat(retBuffer, buffer);
              end = Dictionary_getAll(keyBuffer, valueBuffer);
          }
          return 1;
      }
      else if(!strcmp(*argv, "remove")) {
          log_info("\tKey : %s\n", *(argv+1));
          if(Dictionary_remove(*(argv + 1)) != 0) {
              sprintf(retBuffer, "Key Successfully Deleted\n");
          }
          else {
              sprintf(retBuffer, "Failed delete the Key\n");
              return 0;
          }
          argc -= 1;
          argv += 1;
          return 1;
      }
      else if(!strcmp(*argv, "quit")) {
          return -1;
      }
      argc--;
    }
    return 0;	
}
