#pragma once

#include <sstream>
#include <commctrl.h>
#include "resource.h"
#include "controls.h"

static std::string licenseTags[] =
{
	"ACD-AGENT",
	"ADD-DIRECTORY-NUMBER",
	"ANALOGUE-EXTENSION",
	"ANCD-FEATURE",
	"ANCD-V24-INTERFACE",
	"APP-LINK-INTERFACE",
	"BASE-STATION-DECT",
	"CAS-EXTENSION",
	"CAS-REX-INTERFACE",
	"CCM-ETH-INTERFACE",
	"CCM-V24-INTERFACE",
	"CIL-HDU-INTERFACE",
	"CORDLESS-EXTENSION",
	"CTT-EXTENSION",
	"DIGITAL-EXTENSION",
	"DISA-NUMBER",
	"DTMF-RECEIVER-MOB-EXT",
	"DUPLICATED-CS",
	"DUPLICATED-GS",
	"EMERGENCY-NOTIFICATION",
	"EM-ETH-INTERFACE",
	"EM-V24-INTERFACE",
	"EXTERNAL-LINE-APNSS",
	"EXTERNAL-LINE-CAS-ANA",
	"EXTERNAL-LINE-CAS-DIG",
	"EXTERNAL-LINE-CCSS7",
	"EXTERNAL-LINE-DASS",
	"EXTERNAL-LINE-DPNSS",
	"EXTERNAL-LINE-H323",
	"EXTERNAL-LINE-H323-SERV",
	"EXTERNAL-LINE-ISDN",
	"EXTERNAL-LINE-ISDN-NET",
	"EXTERNAL-LINE-ISDN-USR",
	"EXTERNAL-LINE-QSIG",
	"FIXED-REMOTE-EXTENSION",
	"FREESEATING",
	"GENERIC-EXT-SMS",
	"GROUP-SWITCH-LINK",
	"IP-EXTENSION",
	"ISDN-TERMINAL-INTERFACE",
	"IS-ETH-INTERFACE",
	"IS-V24-INTERFACE",
	"LICENSE-FILE",
	"LINE-INTERFACE-MODULE",
	"MOBILE-EXTENSION",
	"NET-SERVICES",
	"OM-ETH-INTERFACE",
	"OM-MML-PORTS",
	"OM-V24-INTERFACE",
	"OPERATOR-EXTENSION",
	"PAGING-LOOP",
	"PAGING-PARALLEL",
	"PAGING-SERIAL",
	"PERSONAL-NUMBER-USER",
	"RADIO-EXTENSION",
	"REPEAT-IND-DIV-USER",
	"RVA-EXTERNAL",
	"RVA-INTERNAL",
	"SMDR-ETH-INTERFACE",
	"SMDR-V24-INTERFACE",
	"TRAFFIC-MEASUREMENT",
	"VCU-FAX-III-PORT",
	"VM-ETH-INTERFACE",
	"VM-V24-INTERFACE",
	"VOICE-MAILBOX",
	"VOICE-PORT",
	"VOICE-SYSTEM",
	"VOIP-ETH-INTERFACE"
};

static std::string signalTemplate[] =
{
	"SGSAI:SIGAR=1,STYPE=SWSW,LEVEL=A,LIM=1,SIGNAM=LICENSEREQ,UNIT=LISH;",
	"SGSAC:SIGAR=1,BYTE=00-EE&01-EE&02-EE&03-01&04-C2&05-01;",
	"SGSAC:SIGAR=1,BYTE=06-",
	"SGSAC:SIGAR=1,BYTE=07-00&08-",
	"SGSAC:SIGAR=1,BYTE=09-",
	"SGSAC:SIGAR=1,BYTE=",
	"SGSAP:SIGAR=1;",
	"SGSSI:SIGAR=1;",
	"HIEDP;",
	"/* The number of licence objects is contained in BYTE 07 and 08. Edit these for more than 10. */"
};

class Spinner : public SimpleControl
{
public:
	Spinner(HWND hwndParent, int id, BOOL initialState = TRUE)
		: SimpleControl(hwndParent, id, initialState)
	{}
	void setLimits(short upper, short lower)
	{
		SendMessage(hwnd, UDM_SETRANGE, (WPARAM)0, (LPARAM)MAKELONG((short)upper, (short)lower));
		SendMessage(hwnd, UDM_SETPOS, (WPARAM)0, (LPARAM)MAKELONG((short)lower, 0));
	}
	LRESULT getSelection()
	{
		return SendMessage(hwnd, UDM_GETPOS, (WPARAM)0, (LPARAM)0);
	}
};

class ComboBox : public SimpleControl
{
public:
	ComboBox(HWND hwndParent, int id, BOOL initialState = TRUE)
		: SimpleControl(hwndParent, id, initialState)
	{}
	void addStrings()
	{
		// populate drop down list from licenseTags array
		int top = sizeof(licenseTags) / sizeof(licenseTags[0]);
		for (int i = 0; i < top; i++)
		{
			SendMessage(hwnd, CB_ADDSTRING, 0,
				reinterpret_cast<LPARAM>((LPCTSTR)licenseTags[i].c_str()));
		}
		SendMessage(hwnd, CB_SETCURSEL, 0, 0); // select first item on the list
	}
	LRESULT getSelection()
	{
		return SendMessage(hwnd, CB_GETCURSEL, 0, 0);
	}
};
