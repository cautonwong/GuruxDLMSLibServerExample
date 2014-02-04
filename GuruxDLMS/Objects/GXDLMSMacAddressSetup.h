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

#pragma once

#include "IGXDLMSBase.h"
#include "GXDLMSObject.h"
#include "../GXHelpers.h"

class CGXDLMSMacAddressSetup : public CGXDLMSObject
{
	basic_string<char> m_MacAddress;
public:
    /**  
     Constructor.
    */
	CGXDLMSMacAddressSetup() : CGXDLMSObject(OBJECT_TYPE_MAC_ADDRESS_SETUP, "0.0.25.2.0.255")
    {        
    }

    /**  
     Constructor.

     @param ln Logican Name of the object.
    */
    CGXDLMSMacAddressSetup(basic_string<char> ln) : CGXDLMSObject(OBJECT_TYPE_MAC_ADDRESS_SETUP, ln)
    {
    }

    /**  
     Constructor.

     @param ln Logican Name of the object.
     @param sn Short Name of the object.
    */
	CGXDLMSMacAddressSetup(int sn) : CGXDLMSObject(OBJECT_TYPE_MAC_ADDRESS_SETUP, sn)
    {
        
    }

    /** 
     Value of COSEM Data object.
    */
    basic_string<char> GetMacAddress()
    {
        return m_MacAddress;
    }
    void SetMacAddress(basic_string<char> value)
    {
        m_MacAddress = value;
    }

	// Returns amount of attributes.
	int GetAttributeCount()
	{
		return 2;
	}

    // Returns amount of methods.
	int GetMethodCount()
	{
		return 0;
	}

	void GetAttributeIndexToRead(vector<int>& attributes)
	{
		//LN is static and read only once.
		if (CGXOBISTemplate::IsLogicalNameEmpty(m_LN))
        {
            attributes.push_back(1);
		}
		//MacAddress
        if (!IsRead(2))
        {
            attributes.push_back(2);
        }
	}

	int GetDataType(int index, DLMS_DATA_TYPE& type)
    {
		if (index == 1)
		{
			type = DLMS_DATA_TYPE_OCTET_STRING;			
		}
        else if (index == 2)
        {
            type = DLMS_DATA_TYPE_OCTET_STRING;			
        }
		else
		{
			return ERROR_CODES_INVALID_PARAMETER;
		}
		return ERROR_CODES_OK;
	}


	// Returns value of given attribute.
	int GetValue(int index, unsigned char* parameters, int length, CGXDLMSVariant& value)
    {
		if (index == 1)
		{
			GXHelpers::AddRange(value.byteArr, m_LN, 6);
			value.vt = DLMS_DATA_TYPE_OCTET_STRING;
			return ERROR_CODES_OK;
		}
		if (index == 2)
        {
			basic_string<char> add = GetMacAddress();
			GXHelpers::Replace(add, ":", ".");			
			vector<unsigned char> data;
			int ret = CGXOBISTemplate::SetData(data, DLMS_DATA_TYPE_OCTET_STRING, add);
			value = data;
			return ret;
        }
		return ERROR_CODES_INVALID_PARAMETER;
	}

	// Set value of given attribute.
	int SetValue(int index, CGXDLMSVariant& value)
    {
		if (index == 1)
		{			
			if (value.vt != DLMS_DATA_TYPE_OCTET_STRING || value.GetSize() != 6)
			{
				return ERROR_CODES_INVALID_PARAMETER;
			}
			memcpy(m_LN, &value.byteArr[0], 6);
			return ERROR_CODES_OK;
		}
		else if (index == 2)
        {
			CGXDLMSVariant newValue;
			int ret = CGXDLMSClient::ChangeType(value.byteArr, DLMS_DATA_TYPE_OCTET_STRING, newValue);
			if (ret != ERROR_CODES_OK)
			{
				return ret;
			}
			basic_string<char> add = newValue.ToString();
			GXHelpers::Replace(add, ".", ":");
            SetMacAddress(add);
			return ERROR_CODES_OK;
        }
		return ERROR_CODES_INVALID_PARAMETER;
	}
};
