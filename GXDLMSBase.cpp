//
// --------------------------------------------------------------------------
//  Gurux Ltd
// 
//
//
// Filename:        $HeadURL$
//
// Version:         $Revision$,
//                  $Date$
//                  $Author$
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------
//
//  DESCRIPTION
//
// This file is a part of Gurux Device Framework.
//
// Gurux Device Framework is Open Source software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License 
// as published by the Free Software Foundation; version 2 of the License.
// Gurux Device Framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
//
// More information of Gurux products: http://www.gurux.org
//
// This code is licensed under the GNU General Public License v2. 
// Full text may be retrieved at http://www.gnu.org/licenses/gpl-2.0.txt
//---------------------------------------------------------------------------

#include "stdafx.h"
#include "GXDLMSBase.h"
#include "GuruxDLMS/GXDateTime.h"
#include "GuruxDLMS/Objects/GXDLMSData.h"
#include "GuruxDLMS/Objects/GXDLMSRegister.h"
#include "GuruxDLMS/Objects/GXDLMSClock.h"
#include "GuruxDLMS/Objects/GXDLMSTcpUdpSetup.h"
#include "GuruxDLMS/Objects/GXDLMSProfileGeneric.h"
#include "GuruxDLMS/Objects/GXDLMSAutoConnect.h"
#include "GuruxDLMS/Objects/GXDLMSIECOpticalPortSetup.h"
#include "GuruxDLMS/Objects/GXDLMSActivityCalendar.h"    		
#include "GuruxDLMS/Objects/GXDLMSDemandRegister.h"
#include "GuruxDLMS/Objects/GXDLMSRegisterMonitor.h"
#include "GuruxDLMS/Objects/GXDLMSActionSchedule.h"
#include "GuruxDLMS/Objects/GXDLMSSapAssignment.h"
#include "GuruxDLMS/Objects/GXDLMSAutoAnswer.h"
#include "GuruxDLMS/Objects/GXDLMSModemConfiguration.h"
#include "GuruxDLMS/Objects/GXDLMSMacAddressSetup.h"
#include "GuruxDLMS/Objects/GXDLMSModemInitialisation.h"
#include "GuruxDLMS/Objects/GXDLMSActionSet.h"

/// <summary>
/// Generic initialize for all servers.
/// </summary>
/// <param name="server"></param>
int CGXDLMSBase::Init(int port)
{
	m_Media.SetProtocol(GX_NW_TCPIP);
	m_Media.SetPort(port);
	m_Media.SetTrace(GX_TRACE_LEVEL_VERBOSE);
	m_Media.IsServer(true);
    m_Media.AddListener(this);
	int ret;
    if ((ret = m_Media.Open()) != 0)
	{
		printf("Media open failed %d", ret);
		return ret;
	}	
	///////////////////////////////////////////////////////////////////////
    //Add Logical Device Name. 123456 is meter serial number.
	///////////////////////////////////////////////////////////////////////
	// COSEM Logical Device Name is defined as an octet-string of 16 octets.
	// The first three octets uniquely identify the manufacturer of the device and it corresponds
	// to the manufacturer's identification in IEC 62056-21.
	// The following 13 octets are assigned by the manufacturer.
	//The manufacturer is responsible for guaranteeing the uniqueness of these octets.
	CGXDLMSVariant id2(serialNo.c_str());
	id2.ChangeType(DLMS_DATA_TYPE_UINT32);
	std::string tmp("GRX");
	for(int a = serialNo.length(); a < 13; ++a)
	{
		tmp.append("0");
	}
	tmp.append(serialNo);
	CGXDLMSVariant id(tmp.c_str());
	id.ChangeType(DLMS_DATA_TYPE_OCTET_STRING);
	CGXDLMSData* d = new CGXDLMSData("0.0.42.0.0.255", id);
	//d->GetAttributes().push_back(CGXDLMSAttribute(2, DLMS_DATA_TYPE_OCTET_STRING));
	GetItems().push_back(d);
	// Electricity ID 1
	d = new CGXDLMSData("1.1.0.0.0.255", tmp.c_str());
	d->GetAttributes().push_back(CGXDLMSAttribute(2, DLMS_DATA_TYPE_STRING));
	GetItems().push_back(d);
	// Electricity ID 2
	d = new CGXDLMSData("1.1.0.0.1.255", id2);
	d->GetAttributes().push_back(CGXDLMSAttribute(2, DLMS_DATA_TYPE_UINT32));
	GetItems().push_back(d);	
    //Add Last avarage.
    CGXDLMSRegister* pRegister = new CGXDLMSRegister("1.1.21.25.0.255");
    //Set access right. Client can't change Device name.
    pRegister->SetAccess(2, ACCESSMODE_READ);
    GetItems().push_back(pRegister);	
    //Add default clock. Clock's Logical Name is 0.0.1.0.0.255.
	CGXDLMSClock* pClock = new CGXDLMSClock();
	CGXDateTime begin(-1, 9, 1, -1, -1, -1, -1);
	pClock->SetBegin(begin);
	CGXDateTime end(-1, 3, 1, -1, -1, -1, -1);
    pClock->SetEnd(end);
    GetItems().push_back(pClock);
    //Add Tcp/Udp setup. Default Logical Name is 0.0.25.0.0.255.
    GetItems().push_back(new CGXDLMSTcpUdpSetup());
	/*
    ///////////////////////////////////////////////////////////////////////
    //Add Load profile.
    CGXProfileGenericObject* pPG = new CGXProfileGenericObject("1.0.99.1.0.255");
    //Set capture period to 60 second.
    pPG->SetCapturePeriod(60);
    //Maximum row count.
    pPG->SetProfileEntries(100);
    pPG->SetSortMethod(GX_SORT_METHOD_FIFO);
    pPG->SetSortObject(pClock);
    //Add colums.
    //Set saved attribute index.
    //pClock->SetSelectedAttributeIndex(2);
	pPG->GetCaptureObjects().push_back(pClock);
    //Set saved attribute index.
    //pPG->SetSelectedAttributeIndex(2);
    pPG->GetCaptureObjects().push_back(pRegister);
    GetItems().push_back(pPG);
	*/

	///////////////////////////////////////////////////////////////////////
    //Add Auto connect object.
    CGXDLMSAutoConnect* pAC = new CGXDLMSAutoConnect();
    pAC->SetMode(AUTOCONNECTMODE_AUTO_DIALLING_ALLOWED_ANYTIME);
    pAC->SetRepetitions(10);
    pAC->SetRepetitionDelay(60);
    //Calling is allowed between 1am to 6am.
	pAC->GetCallingWindow().push_back(std::make_pair(CGXDateTime(-1, -1, -1, 1, 0, 0, -1), CGXDateTime(-1, -1, -1, 6, 0, 0, -1)));
    pAC->GetDestinations().push_back("www.gurux.org");
    GetItems().push_back(pAC);
	///////////////////////////////////////////////////////////////////////
    //Add Activity Calendar object.
    CGXDLMSActivityCalendar* pActivity = new CGXDLMSActivityCalendar();
    pActivity->SetCalendarNameActive("Active");
	pActivity->GetSeasonProfileActive().push_back(CGXDLMSSeasonProfile("Summer time", CGXDateTime(-1, 3, 31, -1, -1, -1, -1), ""));
	pActivity->GetWeekProfileTableActive().push_back(CGXDLMSWeekProfile("Monday", 1, 1, 1, 1, 1, 1, 1));
	CGXDLMSDayProfile aDp;
	aDp.SetDayId(1);
	aDp.GetDaySchedules().push_back(CGXDLMSDayProfileAction(CGXDateTime::Now(), "test", 1));
	pActivity->GetDayProfileTableActive().push_back(aDp);
    pActivity->SetCalendarNamePassive("Passive");
    pActivity->GetSeasonProfilePassive().push_back(CGXDLMSSeasonProfile("Winter time", CGXDateTime(-1, 10, 30, -1, -1, -1, -1), ""));
    pActivity->GetWeekProfileTablePassive().push_back(CGXDLMSWeekProfile("Tuesday", 1, 1, 1, 1, 1, 1, 1));

	CGXDLMSDayProfile passive;
	passive.SetDayId(1);
	passive.GetDaySchedules().push_back(CGXDLMSDayProfileAction(CGXDateTime::Now(), "0.0.1.0.0.255", 1));
	pActivity->GetDayProfileTablePassive().push_back(passive);
	CGXDateTime dt(CGXDateTime::Now());
	pActivity->SetTime(dt);
    GetItems().push_back(pActivity);

    ///////////////////////////////////////////////////////////////////////
    //Add Optical Port Setup object.
    CGXDLMSIECOpticalPortSetup* pOptical = new CGXDLMSIECOpticalPortSetup();
    pOptical->SetDefaultMode(OPTICAL_PROTOCOL_MODE_DEFAULT);
    pOptical->SetProposedBaudrate(BAUDRATE_9600);
    pOptical->SetDefaultBaudrate(BAUDRATE_300);
	pOptical->SetResponseTime(LOCAL_PORT_RESPONSE_TIME_200_MS);
    pOptical->SetDeviceAddress("Gurux");
    pOptical->SetPassword1("Gurux1");
    pOptical->SetPassword2("Gurux2");
    pOptical->SetPassword5("Gurux5");
    GetItems().push_back(pOptical);

    ///////////////////////////////////////////////////////////////////////
    //Add Demand Register object.
    CGXDLMSDemandRegister* pDr = new CGXDLMSDemandRegister();
    pDr->SetLogicalName("0.0.1.0.0.255");
    pDr->SetCurrentAvarageValue(10);
    pDr->SetLastAvarageValue(20);
    pDr->SetStatus(1);
	pDr->SetStartTimeCurrent(CGXDateTime::Now());
	pDr->SetCaptureTime(CGXDateTime::Now());
    pDr->SetPeriod(10);
    pDr->SetNumberOfPeriods(1);
    GetItems().push_back(pDr);

    ///////////////////////////////////////////////////////////////////////
    //Add Register Monitor object.
    CGXDLMSRegisterMonitor* pRm = new CGXDLMSRegisterMonitor();
    pRm->SetLogicalName("0.0.1.0.0.255");
    CGXDLMSVariant threshold;	
	vector<CGXDLMSVariant> thresholds;		
	threshold.Add("Gurux1", 6);
	thresholds.push_back(threshold);
	threshold.Clear();
	threshold.Add("Gurux2", 6);
	thresholds.push_back(threshold);
	pRm->SetThresholds(thresholds);
	CGXDLMSMonitoredValue mv;
	mv.Update(pRegister, 2);
    pRm->SetMonitoredValue(mv);
	CGXDLMSActionSet action;
	string ln;
	pRm->GetLogicalName(ln);
	action.GetActionDown().SetLogicalName(ln);
	action.GetActionDown().SetScriptSelector(1);
	pRm->GetLogicalName(ln);
	action.GetActionUp().SetLogicalName(ln);
	action.GetActionUp().SetScriptSelector(1);
	vector<CGXDLMSActionSet> actions;
	actions.push_back(action);
    pRm->SetActions(actions);
    GetItems().push_back(pRm);

    ///////////////////////////////////////////////////////////////////////
    //Add action schedule object.
    CGXDLMSActionSchedule* pActionS = new CGXDLMSActionSchedule();
    pActionS->SetLogicalName("0.0.1.0.0.255");
    pActionS->SetExecutedScriptLogicalName("1.2.3.4.5.6");
    pActionS->SetExecutedScriptSelector(1);
	pActionS->SetType(SINGLE_ACTION_SCHEDULE_TYPE1);
	pActionS->GetExecutionTime().push_back(CGXDateTime::Now());
    GetItems().push_back(pActionS);

    ///////////////////////////////////////////////////////////////////////
    //Add SAP Assignment object.
    CGXDLMSSapAssignment* pSap = new CGXDLMSSapAssignment();
	std::map<int, basic_string<char> > list;
    list[1] = "Gurux";
    list[16] = "Gurux-2";
	pSap->SetSapAssignmentList(list);
    GetItems().push_back(pSap);

    ///////////////////////////////////////////////////////////////////////
    //Add Auto Answer object.
    CGXDLMSAutoAnswer* pAa = new CGXDLMSAutoAnswer();
    pAa->SetMode(AUTO_CONNECT_MODE_EMAIL_SENDING);
	pAa->GetListeningWindow().push_back(std::pair<CGXDateTime, CGXDateTime>(CGXDateTime(-1, -1, -1, 6, -1, -1, -1), CGXDateTime(-1, -1, -1, 8, -1, -1, -1)));
    pAa->SetStatus(AUTO_ANSWER_STATUS_INACTIVE);
    pAa->SetNumberOfCalls(0);
	pAa->SetNumberOfRingsInListeningWindow(1);
    pAa->SetNumberOfRingsOutListeningWindow(2);    
    GetItems().push_back(pAa);

    ///////////////////////////////////////////////////////////////////////
    //Add Modem Configuration object.
    CGXDLMSModemConfiguration* pMc = new CGXDLMSModemConfiguration();
	pMc->SetCommunicationSpeed(BAUDRATE_38400);
	CGXDLMSModemInitialisation init;
	vector<CGXDLMSModemInitialisation> initialisationStrings;
    init.SetRequest("AT");
    init.SetResponse("OK");
    init.SetDelay(0);
	initialisationStrings.push_back(init);
    pMc->SetInitialisationStrings(initialisationStrings);    
    GetItems().push_back(pMc);

    ///////////////////////////////////////////////////////////////////////
    //Add Mac Address Setup object.
    CGXDLMSMacAddressSetup* pMac = new CGXDLMSMacAddressSetup();
    pMac->SetMacAddress("00:11:22:33:44:55:66");
    GetItems().push_back(pMac);
    ///////////////////////////////////////////////////////////////////////
    //Server must initialize after all objects are added.
	ret = CGXDLMSServerBase::Initialize();
	if (ret != ERROR_CODES_OK)
	{
		return ret;
	}
	/*
    //Add rows after Initialize.
    Object[][] rows = new Object[][]{new Object[]{java.util.Calendar.getInstance().getTime(), 10}};
    pg.setBuffer(rows);
	*/
	return ERROR_CODES_OK;
}

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
int CGXDLMSBase::OnRead(CGXDLMSObject* pItem, int index, CGXDLMSVariant& value, DLMS_DATA_TYPE& type)
{
	//Let framework handle Logical Name read.
	if (index == 1)
	{
		return ERROR_CODES_FALSE;
	}	
	//Framework will handle Association objects automatically.
	if (pItem->GetObjectType() == OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME ||
		pItem->GetObjectType() == OBJECT_TYPE_ASSOCIATION_SHORT_NAME ||
		//Framework will handle profile generic automatically.
		pItem->GetObjectType() == OBJECT_TYPE_PROFILE_GENERIC)                
    {
        return ERROR_CODES_FALSE;
    }
	DLMS_DATA_TYPE ui, dt;
	pItem->GetUIDataType(index, ui);
	pItem->GetDataType(index, dt);
	if ((ui == DLMS_DATA_TYPE_DATETIME || 
        dt == DLMS_DATA_TYPE_DATETIME) &&
		pItem->GetObjectType() != OBJECT_TYPE_CLOCK)
    {
		value = CGXDateTime::Now();
        return ERROR_CODES_OK;
    }
	else if (pItem->GetObjectType() == OBJECT_TYPE_CLOCK)
	{
		if (index == 2)
		{
			value = CGXDateTime::Now();
			return ERROR_CODES_OK;
		}
	}
	else if (pItem->GetObjectType() == OBJECT_TYPE_REGISTER_MONITOR)
    {
		CGXDLMSRegisterMonitor* pRm = (CGXDLMSRegisterMonitor*) pItem;
		if (index == 2)
		{
			//Initialize random seed.
			srand ((unsigned int)time(NULL)); 		
			value.vt = DLMS_DATA_TYPE_ARRAY;			
			pRm->GetThresholds().clear();
			pRm->GetThresholds().push_back(rand() % 100 + 1);
			CGXDLMSVariant null;
			return pRm->GetValue(index, 0, null, value);
		}
    }
    else
    {
		CGXDLMSVariant null;
		int ret = ((IGXDLMSBase*) pItem)->GetValue(index, 0, null, value);
		if (ret != ERROR_CODES_OK)
		{
			return ret;
		}
        //If data is not assigned and value type is unknown return number.            
		DLMS_DATA_TYPE tp;
		pItem->GetDataType(index, tp);
		if (value.vt == DLMS_DATA_TYPE_NONE &&
			(tp == DLMS_DATA_TYPE_NONE ||
            tp == DLMS_DATA_TYPE_INT8 || 
            tp == DLMS_DATA_TYPE_INT16 || 
            tp == DLMS_DATA_TYPE_INT32 || 
            tp == DLMS_DATA_TYPE_INT64 || 
            tp == DLMS_DATA_TYPE_UINT8 || 
            tp == DLMS_DATA_TYPE_UINT16 || 
            tp == DLMS_DATA_TYPE_UINT32 || 
            tp == DLMS_DATA_TYPE_UINT64))
		{
			//Initialize random seed.
			srand ((unsigned int)time(NULL)); 
			value = rand() % 100 + 1;
			return ERROR_CODES_OK;
		}
		else if (value.vt == DLMS_DATA_TYPE_NONE && tp == DLMS_DATA_TYPE_STRING)
        {
			value = "Gurux";
            return ERROR_CODES_OK;
		}
    }
	//Use current value.
	return ERROR_CODES_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
int CGXDLMSBase::OnWrite(CGXDLMSObject* pItem, int index, int selector, CGXDLMSVariant& value)
{
	return ERROR_CODES_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
int CGXDLMSBase::OnAction(CGXDLMSObject* pItem, int index, CGXDLMSVariant& data)
{
	return ERROR_CODES_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
int CGXDLMSBase::OnInvalidConnection()
{
	printf("Invalid connection.\r\n");
	return ERROR_CODES_OK;
}
