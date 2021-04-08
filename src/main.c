#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h>
#include <string.h>
#include "botconf.h"
#include "apiserver.h"
#include "trice_structs.h"
#include "bot.h"
#include "version.h"

struct tournamentBot {
    struct Config config;
    struct triceBot b;
    struct apiServer server;
    int running;
};

void stopAll(struct tournamentBot *bot) {
    printf("Stopping bot\n"); 
    
    bot->running = 0;    
    stopServer(&bot->server);
    stopBot(&bot->b);
    
    freeServer(&bot->server);
    freeBot(&bot->b);
}

#define LEN 1024
void startConsoleListener (struct tournamentBot *bot) {
    int listening = 1;
    char *commandBuffer = (char *) malloc(sizeof(char) * LEN);
    
    while (listening) {
        fgets(commandBuffer, LEN, stdin);
        
        //Parse command
        if (strncmp ("exit", commandBuffer, LEN) == 0) {
            listening = 0;
        }
    }
    
    free(commandBuffer);    
    stopAll(bot);
}

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#define MACRO_DEBUG_FOR_EVENT(fn, type)\
void DebugFor##fn (struct triceBot *b, type event) {\
    time_t rawtime;\
    struct tm *info;\
    char buffer[80];\
    time(&rawtime);\
    info = localtime( &rawtime );\
    strftime(buffer, 80, "%x - %H:%M:%S %Z", info);\
    \
    printf("[DEBUG] / %s : %s\n", buffer, event.DebugString().c_str());\
}

//Server events
MACRO_DEBUG_FOR_EVENT(onEventServerIdentifictaion,
                      Event_ServerIdentification)
MACRO_DEBUG_FOR_EVENT(onEventServerCompleteList,
                      Event_ServerCompleteList)
MACRO_DEBUG_FOR_EVENT(onEventServerMessage,
                      Event_ServerMessage)
MACRO_DEBUG_FOR_EVENT(onEventServerShutdown, 
                      Event_ServerShutdown)
MACRO_DEBUG_FOR_EVENT(onEventUserMessage,
                      Event_UserMessage)
MACRO_DEBUG_FOR_EVENT(onEventListRooms, 
                      Event_ListRooms)
MACRO_DEBUG_FOR_EVENT(onEventAddToList, 
                      Event_AddToList)
MACRO_DEBUG_FOR_EVENT(onEventRemoveFromList, 
                      Event_RemoveFromList)
MACRO_DEBUG_FOR_EVENT(onEventUserJoined,  
                      Event_UserJoined)
MACRO_DEBUG_FOR_EVENT(onEventUserLeft, 
                      Event_UserLeft)
MACRO_DEBUG_FOR_EVENT(onEventGameJoined, 
                      Event_GameJoined)
MACRO_DEBUG_FOR_EVENT(onEventNotifyUser, 
                      Event_NotifyUser)
MACRO_DEBUG_FOR_EVENT(onEventReplayAdded, 
                      Event_ReplayAdded)

//Room events
MACRO_DEBUG_FOR_EVENT(onEventJoinRoom,
                      Event_JoinRoom)
MACRO_DEBUG_FOR_EVENT(onEventLeaveRoom,
                      Event_LeaveRoom)
MACRO_DEBUG_FOR_EVENT(onEventRoomSay,
                      Event_RoomSay)

#define MACRO_DEBUG_FOR_EVENT_CALL(fn) set_##fn(&DebugFor##fn, b);

void addDebugFunctions(struct triceBot *b) {
    //Server events
    MACRO_DEBUG_FOR_EVENT_CALL (onEventServerIdentifictaion)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventServerCompleteList)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventServerMessage)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventServerShutdown)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventUserMessage)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventListRooms)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventAddToList)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventRemoveFromList)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventUserJoined)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventUserLeft)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventGameJoined)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventNotifyUser)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventReplayAdded)
    
    //Room events
    MACRO_DEBUG_FOR_EVENT_CALL (onEventJoinRoom)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventLeaveRoom)
    MACRO_DEBUG_FOR_EVENT_CALL (onEventRoomSay)
}

#endif

int main (int argc, char * args[]) {
    printf("INFO: %s\n-> by djpiper28 see %s for git repo.\n",           
           PROG_NAME, GITHUB_REPO);
    printf("-> Version %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
    printf("Starting bot\n");
    
    mg_log_set("0");
        
    struct tournamentBot bot;
    bot.running = 1;
    readConf(&bot.config);    
    initBot(&bot.b, bot.config);
    initServer(&bot.server, &bot.b, bot.config);
    
    startServer(&bot.server);
    startBot(&bot.b);
    
    #if DEBUG
    addDebugFunctions(&bot.b);
    #endif
    
    startConsoleListener(&bot);
}
