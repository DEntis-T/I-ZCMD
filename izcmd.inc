/*******************************************************************************************************************************
 Improved ZCMD (iZCMD)

 This include is just a re-make of the ZCMD Command Processor ( V0.3.1 31/10/2009) originnaly developed by Zeex.

 Version: 0.2.3.2 (July 12 2021)

 GitHub Link: https://github.com/DEntisT-SAMP/I-ZCMD/

 Credits:
 Zeex for ZCMD (the very idea of making publics for each command was introduced by Zeex in ZCMD)
 Yashas (updated the include)
 DEntisT (implemented more functonality into the include)
*******************************************************************************************************************************/
#include <a_samp>

#if defined _I_ZCMD_INCLUDED
	#endinput
#endif
#define _I_ZCMD_INCLUDED
/******************************************************************************************************************************/
#define MAX_CLIENT_MSG_LENGTH (144)
#define MAX_FUNC_NAME (32)

#define CMD_SUCCESS 1
#define CMD_FAILURE 0
/******************************************************************************************************************************/
#if defined IZCMD_CUSTOM_PREFIX
	#if !defined IZCMD_SET_CUSTOM_PREFIX
 		#error IZCMD ERROR - Please specify custom command prefix using: #define IZCMD_SET_CUSTOM_PREFIX < your_prefix >
 	#endif
#endif
#if !defined IZCMD_YSI_HOOKING && !defined IZCMD_ALS_HOOKING
 	#error IZCMD ERROR - Please specify function hooking method required for IZCMD. Use #define IZCMD_YSI_HOOKING either #define IZCMD_ALS_HOOKING.
#endif
#if defined IZCMD_YSI_HOOKING && defined IZCMD_ALS_HOOKING
 	#error IZCMD ERROR - Please specify function hooking method required for IZCMD. Use #define IZCMD_YSI_HOOKING either #define IZCMD_ALS_HOOKING.
#endif
/******************************************************************************************************************************/
#if defined IZCMD_YSI_HOOKING

 	#if !defined _INC_y_commands
 		#warning -------------------------------------------------------------------------
		#warning IZCMD Warning - Could not find YSI_Visual\y_commands include! 
		#warning To use IZCMD_YSI_HOOKING hooking feature, firstly include YSI on github.com/pawn-lang/YSI-Includes 
 		#warning ... or use IZCMD_ALS_HOOKING feature instead.
		#warning -------------------------------------------------------------------------
	#endif

#endif
/******************************************************************************************************************************/
#if defined IZCMD_ABBREVIATION_ENABLE

	#define ABRT:%1[%2](%3); \
		cmd_%1(%3); public cmd_%1(%3) \ 
		{ \
			return cmd_%2 ; \
		}
		
#endif
/******************************************************************************************************************************/
#if defined IZCMD_COMMAND_PERMISSIONS

	#define CheckIfCmdCanPerform(%0); \
		if(! %0 ) return CMD_FAILURE;
	
	#define CheckIfPlayerCanPerformCmd(%0,%1); \
		static izcmdmsg = random(sizeof(IZCMD_COMMAND_CANT_PERFORM)),static izcmdstring[300]; \
		format(izcmdstring, sizeof(izcmdstring), "%s", IZCMD_COMMAND_CANT_PERFORM[izcmdmsg]); \
		if(! %1 ) return SendClientMessage(%0,-1,izcmdstring);
		
	#define IZCMD_FAIL_REPORT_STRING(%0); \
		static IZCMD_COMMAND_CANT_PERFORM[][] = { %0 };

#endif
/******************************************************************************************************************************/
#if defined IZCMD_CUSTOM_PREFIX

		#define CPCMD:%1(%2) \
			forward cpcmd_%1(%2); \
			public cpcmd_%1(%2)

		#if defined IZCMD_ALS_HOOKING
		public OnPlayerText(playerid, text[])
		#endif
		if defined IZCMD_YSI_HOOKING
		hook OnPlayerText(playerid,text[])
		#endif
		{
			new funcname[MAX_FUNC_NAME];
			format(funcname,sizeof(funcname),"cpcmd_%s",text);
			if(funcidx(funcname) != -1)
			{
				CallLocalFunction(funcname, "is", playerid,text);
			}

			new cmdstring[MAX_FUNC_NAME];
			format(cmdstring,MAX_FUNC_NAME,""IZCMD_CUSTOM_PREFIX"%s",text);

    		if(!strcmp(text, cmdstring, true, MAX_FUNC_NAME))
    		{	
        		return CMD_SUCCESS;
    		}
   			return CMD_FAILURE;
		}

		#if defined IZCMD_ALS_HOOKING

		#if defined _ALS_OnPlayerText
		    #undef OnPlayerText
		#else
		    #define _ALS_OnPlayerText
		#endif
		#define OnPlayerText izcmd_OnPlayerText
		forward izcmd_OnPlayerText(playerid, cmdtext[]);

		#endif

#endif
/******************************************************************************************************************************/
#define COMMAND:%1(%2)          \
			forward cmd_%1(%2); \
			public cmd_%1(%2)

#define CMD:%1(%2) \
			COMMAND:%1(%2)

#define command(%1,%2,%3) \
			COMMAND:%1(%2, %3)

#define cmd(%1,%2,%3) \
			COMMAND:%1(%2, %3)

#if !defined isnull
        #define isnull(%1) ((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif
/******************************************************************************************************************************/
#if defined OnPlayerCommandReceived
	forward OnPlayerCommandReceived(playerid,cmdtext[]);
#endif
#if defined OnPlayerCommandPerformed
	forward OnPlayerCommandPerformed(playerid,cmdtext[], success);
#endif
/******************************************************************************************************************************/
#if defined IZCMD_ALS_HOOKING
public OnPlayerCommandText(playerid, cmdtext[])
#endif
if defined IZCMD_YSI_HOOKING
hook OnPlayerCommandText(playerid,cmdtext[])
#endif
{
	#if defined OnPlayerCommandReceived
	    if(!OnPlayerCommandReceived(playerid, cmdtext)) return 1;
	#endif
	
	static funcname[MAX_CLIENT_MSG_LENGTH] = "cmd_";

	#if defined IZCMD_ENABLE_CASE_SENSITIVITY
		static pos;

		if((pos = strfind(cmdtext, " ", false, 1)) == -1)
		{
      		funcname[4] = 0;
			strcat(funcname, cmdtext[1], MAX_FUNC_NAME - 4);

		   	#if defined OnPlayerCommandPerformed
		 		return OnPlayerCommandPerformed(playerid, cmdtext, CallLocalFunction(funcname, "is", playerid, "\1"));
			#else
				return CallLocalFunction(funcname, "is", playerid, "\1");
			#endif
		}
		else
		{
			funcname[4] = 0;
			strcat(funcname, cmdtext[1], pos + 4);

	    	while (cmdtext[pos] == ' ') pos++;

			if(cmdtext[pos])
			{
		  		#if defined OnPlayerCommandPerformed
					return OnPlayerCommandPerformed(playerid, cmdtext, CallLocalFunction(funcname, "is", playerid, cmdtext[pos]));
				#else
					return CallLocalFunction(funcname, "is", playerid, cmdtext[pos]);
				#endif
			}
			else
			{
			   	#if defined OnPlayerCommandPerformed
					return OnPlayerCommandPerformed(playerid, cmdtext, CallLocalFunction(funcname, "is", playerid, "\1"));
				#else
					return CallLocalFunction(funcname, "is", playerid, "\1");
				#endif
			}
		}
	#else
		new pos = 1, tmp;

		while((tmp = cmdtext[pos]) > ' ')
			if('A' <= tmp <= 'Z') funcname[pos++ + 3] = tmp | 0x20;
			else funcname[pos++ + 3] = tmp;

		funcname[pos + 3] = 0;

		while (cmdtext[pos] == ' ') pos++;
	
		if(cmdtext[pos])
		{
	  		#if defined OnPlayerCommandPerformed
				return OnPlayerCommandPerformed(playerid, cmdtext, CallLocalFunction(funcname, "is", playerid, cmdtext[pos]));
			#else
				return CallLocalFunction(funcname, "is", playerid, cmdtext[pos]);
			#endif
		}
		else
		{
		   	#if defined OnPlayerCommandPerformed
				return OnPlayerCommandPerformed(playerid, cmdtext, CallLocalFunction(funcname, "is", playerid, "\1"));
			#else
				return CallLocalFunction(funcname, "is", playerid, "\1");
			#endif
		}
	#endif
}
if defined IZCMD_ALS_HOOKING

	#if defined _ALS_OnPlayerCommandText
	    #undef OnPlayerCommandText
	#else
	    #define _ALS_OnPlayerCommandText
	#endif
	#define OnPlayerCommandText izcmd_OnPlayerCommandText
	forward izcmd_OnPlayerCommandText(playerid, cmdtext[]);

#endif
